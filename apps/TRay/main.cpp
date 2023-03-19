#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "stb_image.h"
// #include "core/TRay.h"
#include "core/geometry/Geometry.h"
#include "core/shape/shapes.h"

using namespace TRay;
using namespace std;


int main() {
  std::cout << std::acos(1);
  Transform t1(translate(Vector3f(1, 0, 0)));
  Transform t2(translate(Vector3f(-1, 0, 0)));
  Sphere s(t1, t2, false, 3);
  cout << s.area() << endl;
  return 0;
}
