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
#define VEC_OF_SHARED(T) std::vector<std::shared_ptr<T>>

using json = nlohmann::json;
void get_float(const json &js, Float *x, Float *y, Float *z) {
  if (x == nullptr || y == nullptr || z == nullptr) return;
  *x = js[0].get<Float>();
  *y = js[1].get<Float>();
  *z = js[2].get<Float>();
}
void get_float(const json &js, Float *x, Float *y) {
  if (x == nullptr || y == nullptr) return;
  *x = js[0].get<Float>();
  *y = js[1].get<Float>();
}
void get_float(const json &js, Float *x) {
  if (x == nullptr) return;
  *x = js.get<Float>();
}

namespace TRay {
bool SceneLoader::reload(const char* path) {
  // Clean.
  // ------
  transforms.clear();
  colors.clear();
  spectrum_textures.clear();
  float_textures.clear();
  materials.clear();
  shapes.clear();
  alights.clear();

  primitive_list.clear();
  light_list.clear();

  // Check.
  // ------
  m_file_path = std::string(path);
  std::ifstream f(m_file_path);
  if (!f) {
    SError("TRay::scene_from_json: Failed to open scene file " + m_file_path);
    return false;
  }
  // Load json.
  // ----------
  json scene_file = json::parse(f);
  f.close();
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
  SInfo("Loading transforms");
  for (const auto &trans : scene_file[Key::Transforms]) {
    std::string name = trans[Key::Name].get<std::string>();
    std::string seq = "";
    Transform T, M;
    for (const auto &m : trans[Key::Sequence]) {
      std::string tp = m[Key::Type].get<std::string>();
      if (tp == Val::Rotate) {
        seq = Val::Rotate + " " + seq;
        Float theta = 0;
        theta = m[Key::Value][Key::Theta].get<Float>();
        Vector3f axis{0, 1, 0};
        get_float(m[Key::Value][Key::Axis], &axis.x, &axis.y, &axis.z);
        M = rotate(theta, axis);
        T = M * T;
      } else if (tp == Val::Translate) {
        seq = Val::Translate + " " + seq;
        Float x = 0, y = 0, z = 0;
        get_float(m[Key::Value], &x, &y, &z);
        M = translate(Vector3f(x, y, z));
        T = M * T;
      } else if (tp == Val::Scale) {
        seq = Val::Scale + " " + seq;
        Float x = 0, y = 0, z = 0;
        get_float(m[Key::Value], &x, &y, &z);
        M = scale(x, y, z);
        T = M * T;
      } else if (tp == Val::LookAt) {
        seq = Val::LookAt + " " + seq;
        Point3f pos(0, 0, 0), look(0, 0, 1);
        Vector3f up(0, 1, 0);
        get_float(m[Key::Value][Key::Position], &pos.x, &pos.y, &pos.z);
        get_float(m[Key::Value][Key::Look], &look.x, &look.y, &look.z);
        get_float(m[Key::Value][Key::Up], &up.x, &up.y, &up.z);
        M = look_at(pos, look, up);
        T = M * T;
      } else {
        SWarn("Unknow Transform type " + tp);
      }
    }
    SInfo("Got Transform " + name + " with:\n\tsequence (" + seq + ")");
    transforms[name] = std::make_shared<Transform>(T);
  }
  SInfo("Transforms loaded");
  return true;
}
bool SceneLoader::do_colors(const json &scene_file) {
  // "colors"
  SInfo("Loading colors");
  for (const auto &clr : scene_file[Key::Colors]) {
    std::string name = clr[Key::Name].get<std::string>();
    std::string tp = clr[Key::Type].get<std::string>();
    Spectrum c(0.0);
    if (tp == Val::RGB) {
      Float rgb[3] = {0, 0, 0};
      get_float(clr[Key::Value], rgb, rgb + 1, rgb + 2);
      c = Spectrum::from_RGB(rgb);
    } else {
      SWarn("Unknow Spectrum type " + tp);
    }
    SInfo("Got Spectrum " + name + " with:\n\t value " + c.to_string());
    colors[name] = std::make_shared<Spectrum>(c);
  }
  SInfo("Colors loaded");
  return true;
}
bool SceneLoader::do_textures(const json &scene_file) {
  // "textures"
  SInfo("Loading textures");
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
    } else if (tp == Val::Grid3D) {
      if (rtp == Val::Spectrum) {
        const auto &values = tex[Key::Value];
        std::string ca = values[Key::ColorA].get<std::string>();
        std::string cb = values[Key::ColorB].get<std::string>();
        Vector3f interval;
        get_float(values[Key::Interval], &interval.x, &interval.y, &interval.z);
        Spectrum &color_a{*colors[ca]}, &color_b{*colors[cb]};
        spectrum_textures[name] =
            std::make_shared<Grid3DTexture>(color_a, color_b, interval);
      } else {
        SWarn("Unknown return type " + rtp);
      }
    } else {
      SWarn("Unknown Texture type " + tp);
    }
    SInfo("Got Texture " + name + " with:\n\ttype " + tp + "\n\treturn type " +
          rtp);
  }
  SInfo("Textures loaded");
  return true;
}
bool SceneLoader::do_materials(const json &scene_file) {
  // "materials"
  SInfo("Loading materials");
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
    } else if (tp == Val::MirrorMaterial) {
      std::string diffuse = mat[Key::Diffuse].get<std::string>();
      std::shared_ptr<MirrorMaterial> matt = nullptr;
      matt = std::make_shared<MirrorMaterial>(spectrum_textures[diffuse]);
      materials[name] = matt;
      SInfo("Got Material " + name + " with:\n\ttype " + tp);
    } else {
      SWarn("Unknown Material type " + tp);
    }
  }
  SInfo("Materials loaded");
  return true;
}
bool SceneLoader::do_shapes(const json &scene_file) {
  SInfo("Loading shapes");
  for (const auto &shp : scene_file[Key::Shapes]) {
    std::string name = shp[Key::Name].get<std::string>();
    std::string tp = shp[Key::Type].get<std::string>();
    if (tp == Val::Sphere) {
      std::string trans_name = shp[Key::Transform].get<std::string>();
      std::shared_ptr<Transform> trans = transforms[trans_name];
      bool flip = shp[Key::FlipNormal].get<bool>();
      Float radius = shp[Key::Radius].get<Float>();
      if (shapes.find(name) == shapes.end())
        shapes[name] = std::make_shared<VEC_OF_SHARED(Shape)>();
      shapes[name]->push_back(std::make_shared<Sphere>(
          Sphere{*trans, trans->inverse(), flip, radius}));
      SInfo("Got Shape " + name + " with:\n\ttype " + tp + "\n\tradius " +
            string_format("%f ", radius));
    } else if (tp == Val::MeshPlain) {
      Transform trans;
      if (shp.contains(Key::Transform)) {
        std::string trans_name = shp[Key::Transform].get<std::string>();
        SInfo("applying transform " + trans_name);
        std::shared_ptr<Transform> transs = transforms[trans_name];
        trans = trans * (*transs);
      }
      bool flip = shp[Key::FlipNormal].get<bool>();
      int n_triangles = 0, n_vertices = 0;
      std::vector<int> vertex_indices;
      std::vector<Point3f> vertices;
      for (const auto &v : shp[Key::Vertex]) {
        Float x = 0, y = 0, z = 0;
        get_float(v, &x, &y, &z);
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
      if (shapes.find(name) == shapes.end())
        shapes[name] = std::make_shared<VEC_OF_SHARED(Shape)>();
      auto &vec = shapes[name];
      for (const auto &tri : triangles) vec->push_back(tri);
      SInfo("Got Shape " + name + " with:\n\ttype " + tp +
            "\n\tnumber of triangles " + string_format("%d ", n_triangles) +
            "\n\tnumber of vertices " + string_format("%d ", n_vertices));
    } else if (tp == Val::MeshObj) {
      Transform trans;
      if (shp.contains(Key::Transform)) {
        std::string trans_name = shp[Key::Transform].get<std::string>();
        SInfo("applying transform " + trans_name);
        std::shared_ptr<Transform> transs = transforms[trans_name];
        trans = trans * (*transs);
      }
      bool flip = shp[Key::FlipNormal].get<bool>();
      std::string file_path = shp[Key::File].get<std::string>();
      std::vector<std::shared_ptr<Shape>> triangles;
      bool stat =
          load_triangle_mesh(trans, flip, &triangles, file_path.c_str(), "");
      if (stat) {
        shapes[name] = std::make_shared<VEC_OF_SHARED(Shape)>(triangles);
        SInfo("Got Shape " + name + " with:\n\ttype " + tp);
      } else {
        SError("Error loading triangle mesh from " + file_path);
        return stat;
      }
    } else {
      SWarn("Unknown Shape type " + tp);
    }
  }
  SInfo("Shapes loaded");
  return true;
}
bool SceneLoader::do_lights(const json &scene_file) {
  SInfo("Loading lights");
  for (const auto &lit : scene_file[Key::Lights]) {
    std::string name = lit[Key::Name].get<std::string>();
    std::string tp = lit[Key::Type].get<std::string>();
    PEEK(name);
    PEEK(tp);
    if (tp == Val::DiffuseArea) {
      std::string trans_name = lit[Key::Transform].get<std::string>();
      std::shared_ptr<Transform> trans = transforms[trans_name];
      std::string emit_name = lit[Key::Emit].get<std::string>();
      std::shared_ptr<Spectrum> emit = colors[emit_name];
      int n_samples = lit[Key::NSamples].get<int>();
      std::string shape_name = lit[Key::Shape].get<std::string>();
      const auto &shape_list = shapes[shape_name];
      for (const auto &shape : *shape_list) {
        DiffuseAreaLight light{*trans, *emit, n_samples, shape};
        if (alights.find(name) == alights.end())
          alights[name] = std::make_shared<VEC_OF_SHARED(AreaLight)>();
        alights[name]->push_back(std::make_shared<DiffuseAreaLight>(light));
        light_list.push_back(std::make_shared<DiffuseAreaLight>(light));
      }
      SInfo("Got Light " + name + " with:\n\ttype " + tp + "\n\temit " +
            emit_name + "\n\tshape " + shape_name);
    } else if (tp == Val::Distant) {
      std::string trans_name = lit[Key::Transform].get<std::string>();
      std::shared_ptr<Transform> trans = transforms[trans_name];
      std::string emit_name = lit[Key::Emit].get<std::string>();
      std::shared_ptr<Spectrum> emit = colors[emit_name];
      Vector3f dir;
      get_float(lit[Key::Direction], &dir.x, &dir.y, &dir.z);
      DistantLight light = DistantLight{*trans, *emit, dir};
      light_list.push_back(std::make_shared<DistantLight>(light));
      SInfo("Got Light " + name + " with:\n\ttype " + tp + "\n\temit " +
            emit_name + "\n\twi " + dir.to_string());
    } else {
      // TODO other lights
      SWarn("Unknown Light type " + tp);
    }
  }
  SInfo("Lights loaded");
  return true;
}
bool SceneLoader::do_primitives(const json &scene_file) {
  SInfo("Loading primitives");
  for (const auto &pri : scene_file[Key::Primitives]) {
    std::string tp = pri[Key::Type].get<std::string>();
    if (tp == Val::Geometric) {
      std::string shape_name = pri[Key::Shape].get<std::string>();
      std::string mat_name = pri[Key::Material].get<std::string>();
      std::shared_ptr<Material> mat = materials[mat_name];
      const auto &shape_list = shapes[shape_name];
      if (pri.contains(Key::Light)) {
        // Primitive with lights.
        std::string light_name = pri[Key::Light].get<std::string>();
        const auto &light_list = alights[light_name];
        SInfo("Got Primitive light with:\n\ttype " + tp + "\n\tshape " +
              shape_name + "\n\tmaterial " + mat_name + "\n\tlight " +
              light_name);
        if (shape_list->size() == light_list->size()) {
          for (size_t i = 0; i < shape_list->size(); i++) {
            primitive_list.push_back(std::make_shared<GeometricPrimitive>(
                GeometricPrimitive{(*shape_list)[i], mat, (*light_list)[i]}));
          }
        } else {
          SWarn("Number of lights and shapes cannot match.");
        }
      } else {
        // Primitive without lights.
        SInfo("Got Primitive list with:\n\ttype " + tp + "\n\tshape " +
              shape_name + "\n\tmaterial " + mat_name);
        for (const auto &shape : *shape_list) {
          primitive_list.push_back(std::make_shared<GeometricPrimitive>(
              GeometricPrimitive{shape, mat, nullptr}));
        }
      }
    } else {
      SWarn("Unknown Primitive type " + tp);
    }
  }
  SInfo("Primitives loaded");
  return true;
}
bool SceneLoader::do_accel(const json &accel_file) {
  // "accelerator"
  SInfo("Loading accelerator");
  std::string accel_type = accel_file[Key::Type].get<std::string>();
  if (accel_type == Val::LinearAccel) {
    m_accel = std::make_shared<LinearAccel>(primitive_list);
  } else {
    SWarn("Unknown Aggregate name " + accel_type);
    return false;
  }
  SInfo("Accelerator loaded");
  return true;
}
bool SceneLoader::do_camera(const json &camera_file) {
  // "camera"
  SInfo("Loading camera");
  std::string camera_type = camera_file[Key::Type].get<std::string>();
  std::shared_ptr<Camera> camera = nullptr;
  if (camera_type == Val::PerspectiveCamera) {
    PEEK(camera_type);
    const auto trans =
        transforms[camera_file[Key::Transform].get<std::string>()];
    Point2f scr0, scr1;
    get_float(camera_file[Key::Screen][0], &scr0.x, &scr0.y);
    get_float(camera_file[Key::Screen][1], &scr1.x, &scr1.y);
    Bound2f screen{scr0, scr1};
    Float shutter0 = 0, shutter1 = 0;
    get_float(camera_file[Key::ShutterTime], &shutter0, &shutter1);
    Float lensr = 0, focald = 0, fov = 0;
    lensr = camera_file[Key::LensRadius].get<Float>();
    focald = camera_file[Key::FocalDistance].get<Float>();
    fov = camera_file[Key::FOV].get<Float>();
    const auto &film_file = camera_file[Key::Film];
    std::string film_name = film_file[Key::Name].get<std::string>();
    Float w = 0, h = 0;
    get_float(film_file[Key::Resolution], &w, &h);
    Point2i resolution{int(w), int(h)};
    m_width = w, m_height = h;
    Point2f crop0, crop1;
    get_float(film_file[Key::Crop][0], &crop0.x, &crop0.y);
    get_float(film_file[Key::Crop][1], &crop1.x, &crop1.y);
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
      get_float(film_file[Key::FilterRadius], &fx, &fy);
      BoxFilter filter = BoxFilter{Vector2f{fx, fy}};
      film = std::make_shared<Film>(Film{resolution, crop,
                                         std::make_unique<BoxFilter>(filter),
                                         film_name.c_str()});
    } else {
      SWarn("Unknown Filter type " + filter_type);
    }
    PerspectiveCamera camera = PerspectiveCamera{
        trans->inverse(), screen, shutter0, shutter1, lensr, focald, fov, film};
    m_camera = std::make_shared<PerspectiveCamera>(camera);
    SInfo("Got PerspectiveCamera with:\n\tresolution " +
          resolution.to_string());
  } else {
    SWarn("Unknown Camera type " + camera_type);
    return false;
  }
  SDebug("Camera loaded");
  return true;
}
bool SceneLoader::do_sampler(const json &sampler_file) {
  SDebug("Loading sampler");
  std::string sam_type = sampler_file[Key::Type].get<std::string>();
  if (sam_type == Val::StratifiedSampler) {
    Float sppx = 0, sppy = 0, sdim = 0;
    get_float(sampler_file[Key::SamplePerPixel], &sppx, &sppy);
    sdim = sampler_file[Key::SampleDimension].get<Float>();
    bool jitter = sampler_file[Key::Jitter].get<bool>();
    m_sampler = std::make_shared<StratifiedSampler>(
        StratifiedSampler{int(sppx), int(sppy), int(sdim), jitter});
    SInfo(
        string_format("Got StratifiedSampler with:"
                      "\n\tspp x %d, y %d"
                      "\n\tdim %d"
                      "\n\tjitter %d",
                      int(sppx), int(sppy), int(sdim), jitter));
  } else if (sam_type == Val::RandomSampler) {
    Float spp = 0, sdim = 0;
    get_float(sampler_file[Key::SamplePerPixel], &spp);
    get_float(sampler_file[Key::SampleDimension], &sdim);
    m_sampler =
        std::make_shared<RandomSampler>(RandomSampler{int(spp), int(sdim)});
    SInfo(
        string_format("Got RandomSampler with:"
                      "\n\tspp %d"
                      "\n\tdim %d",
                      int(spp), int(sdim)));
  } else if (sam_type == Val::HaltonSampler) {
    Float spp = 0;
    get_float(sampler_file[Key::SamplePerPixel], &spp);
    m_sampler = std::make_shared<HaltonSampler>(
        HaltonSampler{int(spp), m_camera->m_film->sample_bound()});
    SInfo(
        string_format("Got HaltonSampler with:"
                      "\n\tspp %d",
                      int(spp)));
  } else if (sam_type == Val::ZeroTwoSampler) {
    Float spp = 0, sdim = 0;
    get_float(sampler_file[Key::SamplePerPixel], &spp);
    get_float(sampler_file[Key::SampleDimension], &sdim);
    m_sampler =
        std::make_shared<ZeroTwoSampler>(ZeroTwoSampler{int(spp), int(sdim)});
    SInfo(
        string_format("Got ZeroTwoSampler with:"
                      "\n\tspp %d"
                      "\n\tdim %d",
                      int(spp), int(sdim)));
  } else if (sam_type == Val::MaxMinDisSampler) {
    Float spp = 0, sdim = 0;
    get_float(sampler_file[Key::SamplePerPixel], &spp);
    get_float(sampler_file[Key::SampleDimension], &sdim);
    m_sampler = std::make_shared<MaxMinDisSampler>(
        MaxMinDisSampler{int(spp), int(sdim)});
    SInfo(
        string_format("Got MaxMinDisSampler with:"
                      "\n\tspp %d"
                      "\n\tdim %d",
                      int(spp), int(sdim)));
  } else if (sam_type == Val::SobolSampler) {
    Float spp = 0;
    get_float(sampler_file[Key::SamplePerPixel], &spp);
    m_sampler = std::make_shared<SobolSampler>(
        SobolSampler{int(spp), m_camera->m_film->sample_bound()});
    SInfo(
        string_format("Got SobolSampler with:"
                      "\n\tspp %d",
                      int(spp)));
  } else {
    SWarn("Unknown Sampler type " + sam_type);
    return false;
  }
  SDebug("Sampler loaded");
  return true;
}
bool SceneLoader::do_integrator(const json &integrator_file) {
  SDebug("Loading integrator");
  std::string itr_type = integrator_file[Key::Type].get<std::string>();
  if (itr_type == Val::PathIntegrator) {
    int max_depth = 0;
    max_depth = integrator_file[Key::MaxDepth].get<int>();
    m_integrator = std::make_shared<PathIntegrator>(
        PathIntegrator{max_depth, m_camera, m_sampler});
    SInfo("Got PathIntegrator with:\n\tmax depth " +
          string_format("%d", max_depth));
  } else if (itr_type == Val::DirectIntegrator) {
    int max_depth = 0;
    max_depth = integrator_file[Key::MaxDepth].get<int>();
    m_integrator = std::make_shared<DirectIntegrator>(
        DirectIntegrator{LightSample::UNIFORM_ALL, max_depth, m_camera, m_sampler});
    SInfo("Got DirectIntegrator (UNIFORM_ALL) with:\n\tmax depth " +
          string_format("%d", max_depth));
  } else if (itr_type == Val::WhittedIntegrator) {
    int max_depth = 0;
    max_depth = integrator_file[Key::MaxDepth].get<int>();
    m_integrator = std::make_shared<WhittedIntegrator>(
        WhittedIntegrator{max_depth, m_camera, m_sampler});
    SInfo("Got WhittedIntegrator with:\n\tmax depth " +
          string_format("%d", max_depth));
  } else {
    SWarn("Unknown Sampler type " + itr_type);
    return false;
  }
  SDebug("Integrator loaded");
  return true;
}
}  // namespace TRay
#undef VEC_OF_SHARED