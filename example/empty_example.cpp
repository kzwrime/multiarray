#include <multiarray/multiarray.hpp>
#include <visit_struct/visit_struct.hpp>

void test_at() {
  ma::multiarray<int *, 3> a1;
  a1.pointer = static_cast<int *>(malloc(sizeof(double) * 24));
  a1.set(static_cast<int *>(malloc(sizeof(int) * 24)), 2, 4, 3);
  std::cout << "size = " << a1.size() << "\n";
  for (int i = 0; i < 30; i++) a1.pointer[i] = i;
  for (int k = 0; k < 3; k++) {
    for (int j = 0; j < 4; j++) {
      for (int i = 0; i < 2; i++) {
        printf("%6d ", a1.at_colm(i, j, k));
      }
      printf("\n");
    }
    printf("\n");
  }
  printf("---------------------\n");
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 3; k++) {
        printf("%6d ", a1.at_row(i, j, k));
      }
      printf("\n");
    }
    printf("\n");
  }
  printf("---------------------\n");

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 3; k++) {
        printf("%6d ", a1(i, j, k));
      }
      printf("\n");
    }
    printf("\n");
  }
  printf("---------------------\n");
  printf("---------------------\n");
  ma::multiarray_s<int *, 3, true, 7, 1, 1> b1(a1);

  for (int k = 1; k <= 3; k++) {
    for (int j = 1; j <= 4; j++) {
      for (int i = 7; i <= 8; i++) {
        printf("%6d ", b1.at_colm(i, j, k));
      }
      printf("\n");
    }
    printf("\n");
  }
  printf("---------------------\n");
  for (int i = 7; i <= 8; i++) {
    for (int j = 1; j <= 4; j++) {
      for (int k = 1; k <= 3; k++) {
        printf("%6d ", b1.at_row(i, j, k));
      }
      printf("\n");
    }
    printf("\n");
  }
  printf("---------------------\n");

  for (int i = 7; i <= 8; i++) {
    for (int j = 1; j <= 4; j++) {
      for (int k = 1; k <= 3; k++) {
        printf("%6d ", b1(i, j, k));
      }
      printf("\n");
    }
    printf("\n");
  }
  printf("---------------------\n");
  printf("---------------------\n");
}

auto main() -> int {
  test_at();
  return 0;
}
