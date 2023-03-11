#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "spdlog/spdlog.h"
#include "core/geometry/geometry.h"
#include <iostream>

int main() {
  TRay::Vector3f v(1, 2, 3), u(3, 1, 1);
  v.x = 2;
  v[0] = 3;
  TRay::Vector3f w = TRay::max(u, v);
  FINFO("info {}", 4);
  SCRITICAL("noooooo");
  return 0;
}
