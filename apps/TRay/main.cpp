#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "core/geometry/geometry.h"
#include <iostream>

int main() {
  Float m[4][4] = {
    {1, 0, 1, 2},
    {0, 0, 1, 1},
    {1, 0, 1, 0},
    {0, 1, 1, 1}
  };
  TRay::Mat4x4 a(m);
  std::cout << a;
  TRay::Mat4x4 b = TRay::mat4x4_inverse(a);
  std::cout << b;
  std::cout << std::acos(-1);
  return 0;
}
