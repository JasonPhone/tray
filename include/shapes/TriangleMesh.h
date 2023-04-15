#pragma once
#include "core/TRay.h"
#include "core/geometry/Shape.h"

namespace TRay {
/// @brief Just what you think it is.
struct TriangleMesh {
  /// @param obj_to_world Specify the world position of this triangle mesh.
  ///                     Vertices in TriangleMesh are all transformed into
  ///                     world space.
  /// @param n_triangles Number of triangles.
  /// @param vertex_indices Array of vertex indices, three for each triangle.
  ///                       Layout: [t0_0, t0_1, t0_2, t1_0, ...].
  /// @param vertices Array of vertices, access from index array only.
  /// @param tan_vectors Array of tangent vectors, one for each vertex.
  ///                    Optional. Used for computing shading space.
  /// @param vertex_normals Array of vertex normals. Optional.
  /// @param vertex_uv Array of vertex uv. Optional.
  TriangleMesh(const Transform& obj_to_world, int n_triangles,
               const int* vertex_indices, int n_vertices,
               const Point3f* vertices, const Vector3f* tan_vectors = nullptr,
               const Normal3f* vertex_normals = nullptr,
               const Point2f* vertex_uv = nullptr);
  const int n_triangles, n_vertices;
  /// @brief Vertex indices for triangles, three in a row.
  std::vector<int> vindex;
  std::unique_ptr<Point3f[]> vpos;
  std::unique_ptr<Vector3f[]> vtangent;
  std::unique_ptr<Normal3f[]> vnormal;
  std::unique_ptr<Point2f[]> vuv;
};

class Triangle : public Shape {
 public:
  Triangle(const Transform& obj_world, const Transform& world_obj,
           bool flip_normal, const std::shared_ptr<TriangleMesh> parent_mesh,
           int triangle_index);
  Bound3f object_bound() const override;
  Bound3f world_bound() const override;
  bool intersect(const Ray& ray, Float* thit, SurfaceInteraction* si,
                 bool test_alpha_texture = true) const override;
  Interaction sample_surface(const Point2f& u, Float* pdf_value) const override;
  Float area() const override;

  void uv_values(Point2f uv[3]) const {
    if (m_parent_mesh->vuv) {
      uv[0] = m_parent_mesh->vuv[vidx[0]];
      uv[1] = m_parent_mesh->vuv[vidx[1]];
      uv[2] = m_parent_mesh->vuv[vidx[2]];
    } else {
      uv[0] = Point2f(0, 0);
      uv[1] = Point2f(1, 0);
      uv[2] = Point2f(1, 1);
    }
  }

 private:
  std::shared_ptr<TriangleMesh> m_parent_mesh;
  const int* vidx;
};

std::vector<std::shared_ptr<Shape>> create_triangle_mesh(
    const Transform& obj_world, const Transform& world_obj, bool flip_normal,
    int n_triangles, const int* vertex_indices, int n_vertices,
    const Point3f* vertices, const Vector3f* tan_vectors = nullptr,
    const Normal3f* vertex_normals = nullptr,
    const Point2f* vertex_uv = nullptr);
}  // namespace TRay