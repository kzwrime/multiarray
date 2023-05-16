#include <cassert>

#include "multiarray/multiarray.hpp"

void test_at() {
  ma::multiarray<int *, 3> a1;
  a1.pointer = static_cast<int *>(malloc(sizeof(double) * 24));
  a1.set(static_cast<int *>(malloc(sizeof(int) * 24)), 2, 4, 3);
  std::cout << "size = " << a1.size() << "\n";
  assert(a1.size() == 24);
}

auto main() -> int {
  auto const result = ma::name();

  assert(result == "multiarray");
  test_at();

  return 0;
}
