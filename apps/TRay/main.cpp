#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "core/TRay.h"
#include "core/imageio.h"
#include "core/geometry/geometry.h"
#include "core/primitives/primitives.h"
#include "core/spectrum/spectrum.h"
#include "shapes/shapes.h"
#include "accelerators/accelerators.h"
#include "textures/textures.h"
#include "materials/materials.h"
#include "lights/lights.h"
#include "cameras/cameras.h"
#include "samplers/samplers.h"
#include "integrators/integrators.h"
#include "filters/filters.h"
#include "core/Scene.h"
#include "core/Film.h"
#include "core/reflection/BSDF.h"
#include "loaders/SceneLoader.h"

using namespace TRay;
using namespace std;

uint8_t image[1000 * 1000 * 3];
void render_file(const char*);

int main(int argc, char* argv[]) {
  fill(image, image + sizeof(image), 0);
  if (argc > 1) {
    render_file(argv[1]);
  }
  // Process command-line args.
  // Init from command-line args.
  // Read from json, build scene.
  // Clean up.
  return 0;
}

void render_file(const char* path) {
  SceneLoader sloader{path};
  if (!sloader.reload()) {
    SError("Error loading scene file " + string(path));
    return;
  }
  auto scene = sloader.get_scene();
  auto integrator = sloader.get_integrator();
  integrator->render(*scene);
  auto resolution = sloader.get_resulotion();

  string fname = sloader.get_camera()->m_film->m_filename;
  sloader.get_camera()->m_film->write_image(1.0, image);
  cout << "writing into " << fname << endl;
  stbi_write_jpg(fname.c_str(), resolution.x, resolution.y, 3, image, 95);
}
