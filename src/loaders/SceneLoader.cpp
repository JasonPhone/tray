#include <nlohmann/json.hpp>
#include <fstream>
#include "loaders/SceneLoader.h"
#include "loaders/KeyVal.h"
#include "accelerators/accelerators.h"
#include "textures/textures.h"
#include "materials/materials.h"
#include "shapes/shapes.h"
#include "lights/lights.h"
#include "filters/filters.h"
#include "cameras/cameras.h"
#include "samplers/samplers.h"
#include "integrators/integrators.h"
#include "core/Film.h"

using json = nlohmann::json;
void get_xyz(const json &js, Float *x, Float *y, Float *z) {
  if (x == nullptr || y == nullptr || z == nullptr) return;
  *x = js[0].get<Float>();
  *y = js[1].get<Float>();
  *z = js[2].get<Float>();
}
void get_xy(const json &js, Float *x, Float *y) {
  if (x == nullptr || y == nullptr) return;
  *x = js[0].get<Float>();
  *y = js[1].get<Float>();
}

namespace TRay {
bool SceneLoader::reload() {
  // Clean.
  // ------
  transforms.clear();
  colors.clear();
  spectrum_textures.clear();
  float_textures.clear();
  materials.clear();
  shapes.clear();
  dalights.clear();
  primitive_list.clear();
  light_list.clear();

  // Check.
  // ------
  std::ifstream f(m_file_path);
  if (!f) {
    SError("TRay::scene_from_json: Failed to open scene file " + m_file_path);
    return false;
  }
  // Load json.
  // ----------
  json scene_file = json::parse(f);
  if (!scene_file.is_object()) {
    SError("TRay::scene_from_json: Not an object in scene file" + m_file_path);
    return false;
  }
  bool stat = true;
  // Configure the scene.
  // --------------------
  // TODO stat check.
  stat = stat && do_transforms(scene_file);
  stat = stat && do_colors(scene_file);
  stat = stat && do_textures(scene_file);
  stat = stat && do_materials(scene_file);
  stat = stat && do_shapes(scene_file);
  stat = stat && do_lights(scene_file);
  for (const auto &[name, light] : dalights) {
    light_list.push_back(light);
  }

  stat = stat && do_primitives(scene_file);
  stat = stat && do_accel(scene_file[Key::Accelerator]);
  m_scene = std::make_shared<Scene>(m_accel, light_list);

  stat = stat && do_camera(scene_file[Key::Camera]);
  stat = stat && do_sampler(scene_file[Key::Sampler]);
  stat = stat && do_integrator(scene_file[Key::Integrator]);

  return true;
}

bool SceneLoader::do_transforms(const json &scene_file) {
  // TODO More Checks.
  // if (!scene_file.contains("transforms") ||
  //     !scene_file["transforms"].is_array()) {
  //   SError("TRay::scene_from_json: No transforms array in scene file" +
  //          std::string(path));
  //   return false;
  // }
  // "transforms"
  for (const auto &trans : scene_file[Key::Transforms]) {
    std::string name = trans[Key::Name].get<std::string>();
    std::string seq = "";
    Transform T, M;
    for (const auto &m : trans[Key::Sequence]) {
      std::string tp = m[Key::Type].get<std::string>();
      if (tp == Val::Translate) {
        seq = "translate " + seq;
        Float x = 0, y = 0, z = 0;
        get_xyz(m[Key::Value], &x, &y, &z);
        M = translate(Vector3f(x, y, z));
        T = M * T;
      } else if (tp == Val::LookAt) {
        seq = "look_at " + seq;
        Point3f pos(0, 0, 0), look(0, 0, 1);
        Vector3f up(0, 1, 0);
        get_xyz(m[Key::Value][Key::Position], &pos.x, &pos.y, &pos.z);
        get_xyz(m[Key::Value][Key::Look], &look.x, &look.y, &look.z);
        get_xyz(m[Key::Value][Key::Up], &up.x, &up.y, &up.z);
        M = look_at(pos, look, up);
        T = M * T;
      } else {
        SWarn("Unknow Transform type " + tp);
      }
    }
    SInfo("Got Transform " + name + " with:\n\tsequence (" + seq + ")");
    transforms[name] = std::make_shared<Transform>(T);
  }
  return true;
}
bool SceneLoader::do_colors(const json &scene_file) {
  // "colors"
  for (const auto &clr : scene_file[Key::Colors]) {
    std::string name = clr[Key::Name].get<std::string>();
    std::string tp = clr[Key::Type].get<std::string>();
    Spectrum c(0.0);
    if (tp == Val::RGB) {
      Float rgb[3] = {0, 0, 0};
      get_xyz(clr[Key::Value], rgb, rgb + 1, rgb + 2);
      c = Spectrum::from_RGB(rgb);
    } else {
      SWarn("Unknow Spectrum type " + tp);
    }
    SInfo("Got Spectrum " + name + " with:\n\t value " + c.to_string());
    colors[name] = std::make_shared<Spectrum>(c);
  }
  return true;
}
bool SceneLoader::do_textures(const json &scene_file) {
  // "textures"
  // Two lists but share names.
  for (const auto &tex : scene_file[Key::Textures]) {
    std::string name = tex[Key::Name].get<std::string>();
    std::string tp = tex[Key::Type].get<std::string>();
    std::string rtp = tex[Key::Return].get<std::string>();
    if (tp == Val::Constant) {
      if (rtp == Val::Spectrum) {
        std::string val = tex[Key::Value].get<std::string>();
        ConstantTexture<Spectrum> text{*colors[val]};
        spectrum_textures[name] =
            std::make_shared<ConstantTexture<Spectrum>>(text);
      } else if (rtp == Val::Float) {
        Float val = tex[Key::Value].get<Float>();
        ConstantTexture<Float> text{val};
        float_textures[name] = std::make_shared<ConstantTexture<Float>>(text);
      } else {
        SWarn("Unknown return type " + rtp);
      }
    } else {
      SWarn("Unknown Texture type " + tp);
    }
    SInfo("Got Texture " + name + " with:\n\ttype " + tp + "\n\treturn type " +
          rtp);
  }
  return true;
}
bool SceneLoader::do_materials(const json &scene_file) {
  // "materials"
  for (const auto &mat : scene_file[Key::Materials]) {
    std::string name = mat[Key::Name].get<std::string>();
    std::string tp = mat[Key::Type].get<std::string>();
    if (tp == Val::MatteMaterial) {
      std::string diffuse = mat[Key::Diffuse].get<std::string>();
      std::string sigma = mat[Key::Sigma].get<std::string>();
      std::shared_ptr<MatteMaterial> matt = nullptr;
      matt = std::make_shared<MatteMaterial>(spectrum_textures[diffuse],
                                             float_textures[sigma]);
      materials[name] = matt;
      SInfo("Got Material " + name + " with:\n\ttype " + tp);
    } else {
      SWarn("Unknown Material type " + tp);
    }
  }
  return true;
}
bool SceneLoader::do_shapes(const json &scene_file) {
  for (const auto &shp : scene_file[Key::Shapes]) {
    std::string name = shp[Key::Name].get<std::string>();
    std::string tp = shp[Key::Type].get<std::string>();
    if (tp == Val::Sphere) {
      std::string trans_name = shp[Key::Transform].get<std::string>();
      std::shared_ptr<Transform> trans = transforms[trans_name];
      bool flip = shp[Key::FlipNormal].get<bool>();
      Float radius = shp[Key::Radius].get<Float>();
      Sphere s{*trans, trans->inverse(), flip, radius};
      shapes[name] = std::make_shared<Sphere>(s);
      SInfo("Got Shape " + name + " with:\n\ttype " + tp + "\n\tradius " +
            string_format("%f ", radius));
    } else if (tp == Val::MeshPlain) {
      bool flip = shp[Key::FlipNormal].get<bool>();
      Transform trans;
      int n_triangles = 0, n_vertices = 0;
      std::vector<int> vertex_indices;
      std::vector<Point3f> vertices;
      for (const auto &v : shp[Key::Vertex]) {
        Float x = 0, y = 0, z = 0;
        get_xyz(v, &x, &y, &z);
        vertices.push_back({x, y, z});
        n_vertices++;
      }
      for (const auto &i : shp[Key::Index]) {
        int idx = 0;
        idx = i.get<int>();
        vertex_indices.push_back(idx);
        n_triangles++;
      }
      n_triangles /= 3;
      std::vector<std::shared_ptr<Shape>> triangles =
          create_triangle_mesh(trans, trans.inverse(), flip, n_triangles,
                               &vertex_indices[0], n_vertices, &vertices[0]);
      shape_lists[name] =
          std::make_shared<std::vector<std::shared_ptr<Shape>>>(triangles);
      SInfo("Got Shape " + name + " with:\n\ttype " + tp +
            "\n\tnumber of triangles " + string_format("%d ", n_triangles) +
            "\n\tnumber of vertices " + string_format("%d ", n_vertices));
    } else {
      SWarn("Unknown Shape type " + tp);
    }
  }
  return true;
}
bool SceneLoader::do_lights(const json &scene_file) {
  for (const auto &lit : scene_file[Key::Lights]) {
    std::string name = lit[Key::Name].get<std::string>();
    std::string tp = lit[Key::Type].get<std::string>();
    if (tp == Val::DiffuseArea) {
      std::string trans_name = lit[Key::Transform].get<std::string>();
      std::shared_ptr<Transform> trans = transforms[trans_name];
      std::string emit_name = lit[Key::Emit].get<std::string>();
      std::shared_ptr<Spectrum> emit = colors[emit_name];
      int n_samples = lit[Key::NSamples].get<int>();
      std::string shape_name = lit[Key::Shape].get<std::string>();
      std::shared_ptr<Shape> shape = shapes[shape_name];
      DiffuseAreaLight light{*trans, *emit, n_samples, shape};
      dalights[name] = std::make_shared<DiffuseAreaLight>(light);
      SInfo("Got Light " + name + " with:\n\ttype " + tp + "\n\temit " +
            emit_name + "\n\tshape " + shape_name);
    } else {
      // TODO other lights
      SWarn("Unknown Light type " + tp);
    }
  }
  return true;
}
bool SceneLoader::do_primitives(const json &scene_file) {
  SInfo("Parsing primitives");
  for (const auto &pri : scene_file[Key::Primitives]) {
    std::string tp = pri[Key::Type].get<std::string>();
    if (tp == Val::Geometric) {
      std::string shape_name = pri[Key::Shape].get<std::string>();
      std::string mat_name = pri[Key::Material].get<std::string>();
      std::shared_ptr<DiffuseAreaLight> light =
          pri.contains(Key::Light)
              ? dalights[pri[Key::Light].get<std::string>()]
              : nullptr;
      std::shared_ptr<Material> mat = materials[mat_name];
      if (shapes.count(shape_name)) {
        std::shared_ptr<Shape> shape = shapes[shape_name];
        primitive_list.push_back(std::make_shared<GeometricPrimitive>(
            GeometricPrimitive{shape, mat, light}));
        SInfo("Got Primitive with:\n\ttype " + tp + "\n\tshape " + shape_name +
              "\n\tmaterial " + mat_name);
      } else if (shape_lists.count(shape_name)) {
        std::shared_ptr<std::vector<std::shared_ptr<Shape>>> shape_vec =
            shape_lists[shape_name];
        for (const auto &s : *shape_vec) {
          primitive_list.push_back(std::make_shared<GeometricPrimitive>(
              GeometricPrimitive{s, mat, light}));
        }
        SInfo("Got Primitive list with:\n\ttype " + tp + "\n\tshape " +
              shape_name + "\n\tmaterial " + mat_name);
      } else {
        SWarn("Unknown Shape/Shape array name " + tp);
      }
    } else {
      SWarn("Unknown Primitive type " + tp);
    }
  }
  return true;
}
bool SceneLoader::do_accel(const json &accel_file) {
  // "accelerator"
  std::string accel_type = accel_file[Key::Type].get<std::string>();
  if (accel_type == Val::LinearAccel) {
    m_accel = std::make_shared<LinearAccel>(primitive_list);
  } else {
    SWarn("Unknown Aggregate name " + accel_type);
    return false;
  }
  SDebug("done parsing accel");
  return true;
}
bool SceneLoader::do_camera(const json &camera_file) {
  // "camera"
  std::string camera_type = camera_file[Key::Type].get<std::string>();
  std::shared_ptr<Camera> camera = nullptr;
  if (camera_type == Val::PerspectiveCamera) {
    PEEK(camera_type);
    const auto trans =
        transforms[camera_file[Key::Transform].get<std::string>()];
    Point2f scr0, scr1;
    get_xy(camera_file[Key::Screen][0], &scr0.x, &scr0.y);
    get_xy(camera_file[Key::Screen][1], &scr1.x, &scr1.y);
    Bound2f screen{scr0, scr1};
    Float shutter0 = 0, shutter1 = 0;
    get_xy(camera_file[Key::ShutterTime], &shutter0, &shutter1);
    Float lensr = 0, focald = 0, fov = 0;
    lensr = camera_file[Key::LensRadius].get<Float>();
    focald = camera_file[Key::FocalDistance].get<Float>();
    fov = camera_file[Key::FOV].get<Float>();
    const auto &film_file = camera_file[Key::Film];
    std::string film_name = film_file[Key::Name].get<std::string>();
    Float w = 0, h = 0;
    get_xy(film_file[Key::Resolution], &w, &h);
    Point2i resolution{int(w), int(h)};
    m_width = w, m_height = h;
    Point2f crop0, crop1;
    get_xy(film_file[Key::Crop][0], &crop0.x, &crop0.y);
    get_xy(film_file[Key::Crop][1], &crop1.x, &crop1.y);
    Bound2f crop{crop0, crop1};
    std::shared_ptr<Film> film = nullptr;
    std::string filter_type = film_file[Key::Filter].get<std::string>();
    if (filter_type == Val::NoFilter) {
      NoFilter filter = NoFilter{};
      film = std::make_shared<Film>(Film{resolution, crop,
                                         std::make_unique<NoFilter>(filter),
                                         film_name.c_str()});
    } else if (filter_type == Val::BoxFilter) {
      Float fx = 1, fy = 1;
      get_xy(film_file[Key::FilterRadius], &fx, &fy);
      BoxFilter filter = BoxFilter{Vector2f{fx, fy}};
      film = std::make_shared<Film>(Film{resolution, crop,
                                         std::make_unique<BoxFilter>(filter),
                                         film_name.c_str()});
    } else {
      SWarn("Unknown Filter type " + filter_type);
    }
    PerspectiveCamera camera = PerspectiveCamera{
        *trans, screen, shutter0, shutter1, lensr, focald, fov, film};
    m_camera = std::make_shared<PerspectiveCamera>(camera);
    SInfo("Got PerspectiveCamera with:\n\tresolution " +
          resolution.to_string());
  } else {
    SWarn("Unknown Camera type " + camera_type);
    return false;
  }
  SDebug("done parsing camera");
  return true;
}
bool SceneLoader::do_sampler(const json &sampler_file) {
  std::string sam_type = sampler_file[Key::Type].get<std::string>();
  if (sam_type == Val::StratifiedSampler) {
    Float sppx = 0, sppy = 0, sdim = 0;
    get_xy(sampler_file[Key::SamplePerPixel], &sppx, &sppy);
    sdim = sampler_file[Key::SampleDimension].get<Float>();
    bool jitter = sampler_file[Key::Jitter].get<bool>();
    m_sampler = std::make_shared<StratifiedSampler>(
        StratifiedSampler{int(sppx), int(sppy), int(sdim), jitter});
    SInfo("Got StratifiedCamera with:\n\tspp " +
          string_format(" x %d, y %d", sppx, sppy) + "\n\tdim " +
          string_format("%d", sdim) + "\n\tjitter " +
          string_format("%d", jitter));
  } else {
    SWarn("Unknown Sampler type " + sam_type);
    return false;
  }
  SDebug("done parsing sampler");
  return true;
}
bool SceneLoader::do_integrator(const json &integrator_file) {
  std::string itr_type = integrator_file[Key::Type].get<std::string>();
  if (itr_type == Val::PathIntegrator) {
    int max_depth = 0;
    max_depth = integrator_file[Key::MaxDepth].get<int>();
    m_integrator = std::make_shared<PathIntegrator>(
        PathIntegrator{max_depth, m_camera, m_sampler});
    SInfo("Got PathIntegrator with:\n\tmax depth " +
          string_format("%d", max_depth));
  } else {
    SWarn("Unknown Sampler type " + itr_type);
    return false;
  }
  SDebug("done parsing integrator");
  return true;
}
}  // namespace TRay