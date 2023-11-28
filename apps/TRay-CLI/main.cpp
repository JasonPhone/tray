#include <ctime>
#include <iostream>

#include "core/Integrator.h"
#include "core/TRay.h"
#include "core/imageio.h"
#include "core/statistics.h"
#include "loaders/SceneLoader.h"

using namespace TRay;
using namespace std;

uint8_t image[1000 * 1000 * 3];

std::string file_path;
SceneLoader sloader;
void render_file(const char *);

int main(int argc, char *argv[]) {
  fill(image, image + sizeof(image), 0);

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      time_t st = time(NULL);
      render_file(argv[i]);
      time_t ed = time(NULL);
      SInfo(string_format("Rendering done, %d seconds used.", int(ed - st)));
      TRay::PrintStats(std::cout);
    }
    return 0;
  }

  return 0;
}

void render_file(const char *path) {
  if (!sloader.reload(path)) {
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