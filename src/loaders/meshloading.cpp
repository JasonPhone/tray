#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "core/geometry/Point.h"
#include "loaders/meshloading.h"
#include "shapes/TriangleMesh.h"

namespace TRay {
bool load_triangle_mesh(const Transform& obj_to_world, bool flip_normal,
                        std::vector<std::shared_ptr<Shape>>* triangles,
                        const char* filename, const char* basepath) {
  SInfo("Loading triangle mesh from file " + std::string(filename) +
        ", base dir " + std::string(basepath));
  // Load from file.
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                              filename, basepath, true);
  if (!warn.empty()) SWarn("Warn loading triangle mesh: " + warn);
  if (!err.empty()) SError("Error loading triangle mesh: " + err);
  if (!ret) {
    printf("Failed to load/parse .obj.\n");
    SError("Failed loading triangle mesh from " + std::string(filename));
    return false;
  }
  // Dump into TRay format.
  // NOTE Maybe redundant for multiple shapes,
  // since TriangleMesh keeps a copy of vertices.
  std::vector<Point3f> total_vertices;
  SInfo(
      string_format("%d vertices in total.", int(attrib.vertices.size() / 3)));
  for (size_t vi = 0; vi < attrib.vertices.size() / 3; vi++) {
    total_vertices.push_back({attrib.vertices[3 * vi + 0],
                              attrib.vertices[3 * vi + 1],
                              attrib.vertices[3 * vi + 2]});
  }
  for (size_t si = 0; si < shapes.size(); si++) {
    SInfo(string_format("Processing shape %d (name \"%s\")...", int(si),
                        shapes[si].name.c_str()));
    int n_triangles = 0, n_vertices = 0;
    std::vector<int> vertex_indices;
    n_triangles = shapes[si].mesh.num_face_vertices.size();
    n_vertices = total_vertices.size();

    size_t idx_offset = 0;
    for (size_t fi = 0; fi < shapes[si].mesh.num_face_vertices.size(); fi++) {
      size_t vnum = shapes[si].mesh.num_face_vertices[fi];
      // SInfo(string_format("face %d, num vertices %d", int(fi), int(vnum)));
      for (size_t vi = 0; vi < vnum; vi++) {
        tinyobj::index_t idx = shapes[si].mesh.indices[idx_offset + vi];
        vertex_indices.push_back(idx.vertex_index);
      }
      // SInfo("------------");
      idx_offset += vnum;
    }
    /**
     * input
     *   const Transform &obj_world, const Transform &world_obj,
     *   bool flip_normal,
     *   int n_triangles, const int *vertex_indices,
     *   int n_vertices, const Point3f *vertices,
     *
     * output
     *   std::vector<std::shared_ptr<Shape>>
     */
    auto triangle_vec = create_triangle_mesh(
        obj_to_world, obj_to_world.inverse(), flip_normal, n_triangles,
        &vertex_indices[0], n_vertices, &total_vertices[0]);
    triangles->insert(triangles->end(), triangle_vec.begin(),
                      triangle_vec.end());
  }

  SInfo("Loaded triangle mesh from file " + std::string(filename) +
        ", base dir " + std::string(basepath));
  return true;
}
}  // namespace TRay