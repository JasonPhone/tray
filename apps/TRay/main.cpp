#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "core/geometry/geometry.h"
#include <iostream>

int main() {
  TRay::Mat4x4 a;
  TRay::Point3f p(0, 0, 0);
  TRay::Transform t1(a);
  TRay::Transform t2(TRay::translate({1, 1, 0}) * t1);
  TRay::AnimateTransform at(t1, 0, t2, 1);
  std::cout << t1(p) << " --> " << t2(p) << std::endl;
  std::cout << at(p, 0.5) << std::endl;
  
  return 0;
}
