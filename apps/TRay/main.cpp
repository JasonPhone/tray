#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "core/geometry/geometry.h"
#include <iostream>

int main() {
  TRay::Bound2i b({1, 2}, {5, 6});
  TRay::Bound2iIterator r(b);
  int c = 0;
  for (auto p : r) {
    std::cout << p ;
  }
  return 0;
}
