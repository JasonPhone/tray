#pragma once
#include "core/TRay.h"
#include "core/Scene.h"
#include "loaders/meshloading.h"
#include <nlohmann/json_fwd.hpp>

namespace TRay {
class SceneLoader {
 public:
  SceneLoader(const char* path) : m_file_path(path) {}
  std::shared_ptr<Scene> get_scene() const { return m_scene; }
  std::shared_ptr<Integrator> get_integrator() const { return m_integrator; }
  Point2i get_resulotion() const { return Point2i(m_width, m_height); }
  bool load() { return reload(); }
  bool reload();

 private:
  std::string m_file_path;
  std::vector<std::shared_ptr<Primitive>> primitive_list;
  std::vector<std::shared_ptr<Light>> light_list;
  std::shared_ptr<Aggregate> m_accel = nullptr;
  std::shared_ptr<Scene> m_scene = nullptr;
  std::shared_ptr<Camera> m_camera = nullptr;
  std::shared_ptr<Sampler> m_sampler = nullptr;
  std::shared_ptr<Integrator> m_integrator = nullptr;
  int m_width, m_height;

#define VEC_OF_SHARED(T) std::vector<std::shared_ptr<T>>
  std::map<std::string, std::shared_ptr<Transform>> transforms;
  std::map<std::string, std::shared_ptr<Spectrum>> colors;
  std::map<std::string, std::shared_ptr<Texture<Spectrum>>> spectrum_textures;
  std::map<std::string, std::shared_ptr<Texture<Float>>> float_textures;
  std::map<std::string, std::shared_ptr<Material>> materials;
  std::map<std::string, std::shared_ptr<VEC_OF_SHARED(Shape)>> shapes;
  std::map<std::string, std::shared_ptr<VEC_OF_SHARED(DiffuseAreaLight)>> dalights;
#undef VEC_OF_SHARED

  using json = nlohmann::json;
  bool do_transforms(const json& scene_file);
  bool do_colors(const json& scene_file);
  bool do_textures(const json& scene_file);
  bool do_materials(const json& scene_file);
  bool do_shapes(const json& scene_file);
  bool do_lights(const json& scene_file);
  bool do_primitives(const json& scene_file);
  bool do_accel(const json& accel_file);
  bool do_sampler(const json& sampler_file);
  bool do_camera(const json& camera_file);
  bool do_integrator(const json& integrator_file);
};
}  // namespace TRay