#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "stb_image.h"
// #include "core/TRay.h"
#include "core/geometry/geometry.h"
#include "shapes/Sphere.h"

using namespace TRay;
using namespace std;


int main() {
  Transform t1(translate(Vector3f(2, 0, 0)));
  Transform t2(translate(Vector3f(-2, 0, 0)));
  Sphere s(t1, t2, false, 1);
  Ray r(Point3f(0, 0, 0), Vector3f(0, 0, 1));
  cout << s.intersect_test(r);
  return 0;
}
