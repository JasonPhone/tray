#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ImGui/imgui.h"
#include "ImGui/backend/imgui_impl_glfw.h"
#include "ImGui/backend/imgui_impl_opengl3.h"
#include "gui/utility.h"
#include "gui/Shader.h"
#include "file_dialog/file_dialog.h"

#include <iostream>
#include <ctime>
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
const int window_w = 900, window_h = 800;
int image_w = 200, image_h = 200;

double last_render_time = 0;
double time_cost = 0;
std::string file_path;
SceneLoader sloader;
void render_file(const char*);

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
  printf("Glfw Error %d: %s\n", error, description);
}

static bool open_scene_file(const char* path) {
  file_path = path;
  bool stat = sloader.reload(path);
  if (stat) {
    image_w = sloader.get_resulotion().x;
    image_h = sloader.get_resulotion().y;
    return true;
  } else {
    SError("Error loading scene file.");
    return false;
  }
}
static void save_image() {
  if (sloader.get_camera()) {
    string name = sloader.get_camera()->m_film->m_filename;
    stbi_write_jpg(name.c_str(), image_w, image_h, 3, image, 95);
  }
}

int main(int argc, char* argv[]) {
  fill(image, image + sizeof(image), 0);

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      time_t st = time(NULL);
      render_file(argv[i]);
      time_t ed = time(NULL);
      SInfo(string_format("Rendering done, %d seconds used.", int(ed - st)));
    }
    return 0;
  }
  for (int i = 0; i < image_h; i++) {
    for (int j = 0; j < image_w; j++) {
      image[(i * image_w + j) * 3 + 0] = 1.0 * i / image_h * 255;
      image[(i * image_w + j) * 3 + 1] = 1.0 * j / image_w * 255;
      image[(i * image_w + j) * 3 + 2] = 255;
    }
  }

  // Setup window
  printf("setup\n");
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) return 1;

  // Decide GL+GLSL versions
  // GL 3.0 + GLSL 130
  printf("choose version\n");
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE);  // use core profile
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create window with graphics context
  printf("create window context\n");
  GLFWwindow* window = glfwCreateWindow(window_w, window_h, "TRay", NULL, NULL);
  if (window == NULL) return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync

  // Initialize OpenGL loader
  bool err = gladLoadGL() == 0;
  if (err) {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return 1;
  }

  // Setup Dear ImGui context
  printf("imgui context\n");
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  printf("binding\n");
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Rander result as background texture.
  Shader shader{"./shader.vs", "./shader.fs"};
  // vao and vbo
  // clang-format off
  float unique_vert[] = {
  // ---- position ---- - tex coord -
     0.8f,  0.8f, 0.0f, 1.0f, 0.0f,
     0.8f, -0.8f, 0.0f, 1.0f, 1.0f,
    -0.8f, -0.8f, 0.0f, 0.0f, 1.0f,
    -0.8f,  0.8f, 0.0f, 0.0f, 0.0f
  };
  GLuint indices[] = {0, 1, 3, 1, 2, 3};
  // clang-format on
  GLuint EBO, VAO, VBO;
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  // buffer VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(unique_vert), unique_vert,
               GL_STATIC_DRAW);
  // vertex attrib: position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // vertex attrib: color
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // buffer EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  // render loop
  GLuint texture0 = 0;
  int logo_w, logo_h, nchnl = 0;
  uint8_t* logo = stbi_load("./logo.jpg", &logo_w, &logo_h, &nchnl, 3);
  texture0 = create_texture(logo, logo_w, logo_h);
  shader.use();
  shader.set_int("texture0", 0);

  printf("main loop\n");
  // Main loop
  // GUI state.
  bool file_opened = false;
  bool file_loaded = false;
  bool rendering = false;
  bool show_logo = true;
  bool done_rendering = false;
  bool m_fileDialogOpen;
  ImFileDialogInfo m_fileDialogInfo;

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Update image.
    double time = glfwGetTime();
    if (rendering && time - last_render_time > 0.03) {
      done_rendering =
          sloader.get_integrator()->render_step(*(sloader.get_scene()));
      time_cost += glfwGetTime() - time;
      rendering = !done_rendering;
      sloader.get_camera()->m_film->write_image(1.0, image);
      glDeleteTextures(1, &texture0);
      texture0 = create_texture(image, image_w, image_h);
      last_render_time = time;
    }
    // Logo
    if (show_logo) {
      glDeleteTextures(1, &texture0);
      texture0 = create_texture(logo, logo_w, logo_h);
    }

    // Redraw image.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      ImGui::Begin("Panel");

      // Open file.
      if (ImGui::Button("Open")) {
        rendering = false;
        done_rendering = false;
        // Open file callback.
        m_fileDialogOpen = true;
        m_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
        m_fileDialogInfo.title = "Open Scene File";
        m_fileDialogInfo.fileName = "test.json";
        m_fileDialogInfo.directoryPath = std::filesystem::current_path();
      }
      if (ImGui::FileDialog(&m_fileDialogOpen, &m_fileDialogInfo)) {
        // Result path in: m_fileDialogInfo.resultPath
        file_opened = file_loaded =
            open_scene_file(m_fileDialogInfo.resultPath.string().c_str());
      }
      ImGui::SameLine();
      if (file_path.empty())
        ImGui::Text("No active scene file.");
      else
        ImGui::Text("Scene file: %s", file_path.c_str());

      // Reload file.
      if (ImGui::Button("Reload")) {
        if (file_opened) {
          file_loaded = open_scene_file(file_path.c_str());
          rendering = false;
          done_rendering = false;
          time_cost = 0;
          show_logo = true;
        }
      }
      ImGui::SameLine();
      if (file_opened) {
        if (file_loaded)
          ImGui::Text("File loaded.");
        else
          ImGui::Text("File not loaded.");
      } else {
        ImGui::Text("No active scene file.");
      }
      // Render.
      if (ImGui::Button("Render")) {
        if (file_loaded) {
          rendering = !rendering;
          show_logo = false;
        }
      }
      ImGui::SameLine();
      if (done_rendering) {
        ImGui::Text("Done. %.3f seconds.", time_cost);
      } else if (rendering) {
        ImGui::Text("Rendering. %.3f seconds.", time_cost);
      } else {
        ImGui::Text("Paused. %.3f seconds.", time_cost);
      }

      // Export image.
      if (ImGui::Button("Export")) {
        if (rendering || done_rendering) save_image();
      }
      ImGui::SameLine();
      if (file_loaded) {
        ImGui::Text("%s", sloader.get_camera()->m_film->m_filename.c_str());
      } else {
        ImGui::Text("Load a scene file to configure the scene.");
      }

      ImGui::Text("%.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    int show_w = show_logo ? logo_w : image_w;
    int show_h = show_logo ? logo_h : image_h;
    int view_h = std::min(show_h * 2.0, window_h * 0.8);
    int view_w = 1.0 * show_w / show_h * view_h;
    glViewport((display_w - view_w) / 2, (display_h - view_h) / 2, view_w,
               view_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  printf("clean\n");
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

void render_file(const char* path) {
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