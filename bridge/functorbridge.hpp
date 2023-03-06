template <typename R, typename... Args>
class FunctorBridge {
 public:
  virtual ~FunctorBridge() {}
  virtual FunctorBridge* clone() const = 0;
  virtual R invoke(Args... args) const = 0;
  virtual bool equals(FunctorBridge const* fb) const = 0;
};
