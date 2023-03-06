#include <iostream>
#include <type_traits>  // for true_type and false_type
#include <utility>      // for declval()
#include <exception>
#include <vector>


// bridge/isequalitycomparable.hpp
template <typename T>
class IsEqualityComparable {
 private:
  // test convertibility of == and ! == to bool:
  static void* conv(bool);  // to check convertibility to bool
  template <typename U>
  static std::true_type test(
      decltype(conv(std::declval<U const&>() == std::declval<U const&>())),
      decltype(conv(!(std::declval<U const&>() == std::declval<U const&>()))));
  // fallback:
  template <typename U>
  static std::false_type test(...);

 public:
  static constexpr bool value = decltype(test<T>(nullptr, nullptr))::value;
};

// bridge/tryequals.hpp
template <typename T, bool EqComparable = IsEqualityComparable<T>::value>
struct TryEquals {
  static bool equals(T const& x1, T const& x2) { return x1 == x2; }
};

class NotEqualityComparable : public std::exception {};

template <typename T>
struct TryEquals<T, false> {
  static bool equals(T const& x1, T const& x2) {
    throw NotEqualityComparable();
  }
};

// bridge/functorbridge.hpp
template <typename R, typename... Args>
class FunctorBridge {
 public:
  virtual ~FunctorBridge() {}
  virtual FunctorBridge* clone() const = 0;
  virtual R invoke(Args... args) const = 0;
  virtual bool equals(FunctorBridge const* fb) const = 0;
};

// bridge/functionptr.hpp
// primary template:
template <typename Signature>
class FunctionPtr;

// partial specialization:
template <typename R, typename... Args>
class FunctionPtr<R(Args...)> {
 private:
  FunctorBridge<R, Args...>* bridge;

 public:
  // constructors:
  FunctionPtr() : bridge(nullptr) {}
  FunctionPtr(FunctionPtr const& other);  // see functionptr-cpinv.hpp
  FunctionPtr(FunctionPtr& other)
      : FunctionPtr(static_cast<FunctionPtr const&>(other)) {}
  FunctionPtr(FunctionPtr&& other) : bridge(other.bridge) {
    other.bridge = nullptr;
  }
  // construction from arbitrary function objects:
  template <typename F>
  FunctionPtr(F&& f);  // see functionptr-init.hpp

  // assignment operators:
  FunctionPtr& operator=(FunctionPtr const& other) {
    FunctionPtr tmp(other);
    swap(*this, tmp);
    return *this;
  }
  FunctionPtr& operator=(FunctionPtr&& other) {
    delete bridge;
    bridge = other.bridge;
    other.bridge = nullptr;
    return *this;
  }
  // construction and assignment from arbitrary function objects:
  template <typename F>
  FunctionPtr& operator=(F&& f) {
    FunctionPtr tmp(std::forward<F>(f));
    swap(*this, tmp);
    return *this;
  }

  // destructor:
  ~FunctionPtr() { delete bridge; }

  friend void swap(FunctionPtr& fp1, FunctionPtr& fp2) {
    std::swap(fp1.bridge, fp2.bridge);
  }
  explicit operator bool() const { return bridge == nullptr; }

  // invocation:
  R operator()(Args... args) const;  // see functionptr-cpinv.hpp

  friend bool operator==(FunctionPtr const& f1, FunctionPtr const& f2) {
    if (!f1 || !f2) {
      return !f1 && !f2;
    }
    return f1.bridge->equals(f2.bridge);
  }

  friend bool operator!=(FunctionPtr const& f1, FunctionPtr const& f2) {
    return !(f1 == f2);
  }
};

// bridge/specificfunctorbridge.hpp
template <typename Functor, typename R, typename... Args>
class SpecificFunctorBridge : public FunctorBridge<R, Args...> {
  Functor functor;

 public:
  template <typename FunctorFwd>
  SpecificFunctorBridge(FunctorFwd&& functor)
      : functor(std::forward<FunctorFwd>(functor)) {}

  virtual SpecificFunctorBridge* clone() const override {
    return new SpecificFunctorBridge(functor);
  }

  virtual R invoke(Args... args) const override {
    return functor(std::forward<Args>(args)...);
  }

  virtual bool equals(FunctorBridge<R, Args...> const* fb) const override {
    if (auto specFb = dynamic_cast<SpecificFunctorBridge const*>(fb)) {
      return TryEquals<Functor>::equals(functor, specFb->functor);
    }
    //functors with different types are never equal:
    return false;
  }
};

// bridge/functionptr-init.hpp
template<typename R, typename... Args>
template<typename F>
FunctionPtr<R(Args...)>::FunctionPtr(F&& f)
: bridge(nullptr)
{
  using Functor = typename std::decay<F>::type;
  using Bridge = SpecificFunctorBridge<Functor, R, Args...>;
  bridge = new Bridge(std::forward<F>(f));
}

// bridge/functionptr-cpinv.hpp
template<typename R, typename... Args>
FunctionPtr<R(Args...)>::FunctionPtr(FunctionPtr const& other)
: bridge(nullptr)
{
  if (other.bridge) {
    bridge = other.bridge->clone();
  }
}

template<typename R, typename... Args>
R FunctionPtr<R(Args...)>::operator()(Args... args) const
{
  return bridge->invoke(std::forward<Args>(args)...);
}

// ---------------------------------------------------------------

void forUpTo(int n, FunctionPtr<void(int)> f)
{
  for (int i = 0; i != n; ++i)
  {
    f(i);  // call passed function f for i
  }
}

void printInt(int i)
{
  std::cout << i << " ";
}

int main()
{
  std::vector<int> values;

  // insert values from 0 to 4:
  forUpTo(5,
    [&values](int i) {
      values.push_back(i);
    });

  // print elements:
  forUpTo(5, printInt);  // prints 0 1 2 3 4
  std::cout << "\n";

  return 0;
}

