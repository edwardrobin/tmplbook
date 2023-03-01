#include <iostream>

#include "isdefaultconstructible3.hpp"


int main() {

  std::cout << IsDefaultConstructibleT<int>::value << std::endl;
  return 0;
}

