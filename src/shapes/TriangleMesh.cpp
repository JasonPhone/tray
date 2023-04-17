#include "shapes/TriangleMesh.h"
#include "core/geometry/Point.h"
#include "core/geometry/Normal.h"
#include "core/geometry/Transform.h"
#include "core/geometry/Bound.h"
#include "core/math/sampling.h"

namespace TRay {
TriangleMesh::TriangleMesh(const Transform &obj_to_world, int n_triangles,
                           const int *vertex_indices, int n_vertices,
                           const Point3f *vertices,
                           const Normal3f *vertex_normals,
                           const Point2f *vertex_uv)
    : n_triangles(n_triangles),
      n_vertices(n_vertices),
      vindex(vertex_indices, vertex_indices + 3 * n_triangles) {
  // Point3f* vertices.
  vpos.reset(new Point3f[n_vertices]);
  for (int i = 0; i < n_vertices; i++) vpos[i] = obj_to_world(vertices[i]);
  // Optional Normal3f* vertex_normals.
  if (vertex_normals) {
    vnormal.reset(new Normal3f[n_vertices]);
    for (int i = 0; i < n_vertices; ++i)
      vnormal[i] = obj_to_world(vertex_normals[i]);
  }
  // Optional Point2f* vertex_uv.
  if (vertex_uv) {
    vuv.reset(new Point2f[n_vertices]);
    memcpy(vuv.get(), vertex_uv, n_vertices * sizeof(Point2f));
  }
}

Triangle::Triangle(const Transform &obj_world, const Transform &world_obj,
                   bool flip_normal,
                   const std::shared_ptr<TriangleMesh> parent_mesh,
                   int triangle_index)
    : Shape(obj_world, world_obj, flip_normal),
      m_parent_mesh(parent_mesh),
      vidx(&(parent_mesh->vindex[3 * triangle_index])) {}
Bound3f Triangle::object_bound() const {
  return (*world_to_obj)(world_bound());
}
Bound3f Triangle::world_bound() const {
  const Point3f &p0 = m_parent_mesh->vpos[vidx[0]];
  const Point3f &p1 = m_parent_mesh->vpos[vidx[1]];
  const Point3f &p2 = m_parent_mesh->vpos[vidx[2]];
  return bound_insert(Bound3f(p0, p1), p2);
}
bool Triangle::intersect(const Ray &ray, Float *thit, SurfaceInteraction *si,
                         bool test_alpha_texture) const {
  // Prepare.
  // --------
  const Point3f &p0 = m_parent_mesh->vpos[vidx[0]];
  const Point3f &p1 = m_parent_mesh->vpos[vidx[1]];
  const Point3f &p2 = m_parent_mesh->vpos[vidx[2]];
  // Ray-triangle intersect test.
  // ----------------------------
  // Transform to ray space, where ray starts from (0, 0, 0) and goes along +z.
  // TODO some coefficients can be precomputed when a ray is constructed.
  // Translate to ray origin.
  Point3f p0t = p0 - Vector3f(ray.ori);
  Point3f p1t = p1 - Vector3f(ray.ori);
  Point3f p2t = p2 - Vector3f(ray.ori);
  // Reorder the basis to make ray direction largest along +z.
  int dimz = max_dim(abs(ray.dir));
  int dimx = (dimz + 1) % 3;
  int dimy = (dimx + 1) % 3;
  Vector3f dirt = permute(ray.dir, dimx, dimy, dimz);
  p0t = permute(p0t, dimx, dimy, dimz);
  p1t = permute(p1t, dimx, dimy, dimz);
  p2t = permute(p2t, dimx, dimy, dimz);
  // Shear to make dir (0, 0, 1, 0).
  // z values are postponed until a real hit.
  Float sx = -dirt.x / dirt.z;
  Float sy = -dirt.y / dirt.z;
  Float sz = 1.f / dirt.z;
  p0t.x += sx * p0t.z;
  p0t.y += sy * p0t.z;
  p1t.x += sx * p1t.z;
  p1t.y += sy * p1t.z;
  p2t.x += sx * p2t.z;
  p2t.y += sy * p2t.z;
  // Edge function coefficients.
  // e_i is for p(0, 0), p_{i + 1} and p_{i + 2}
  // Use double for higher precision.
  double e0 = 1.0 * p1t.x * p2t.y - p1t.y * p2t.x;
  double e1 = 1.0 * p2t.x * p0t.y - p2t.y * p0t.x;
  double e2 = 1.0 * p0t.x * p1t.y - p0t.y * p1t.x;
  // Test edge and sum.
  if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
    return false;
  double d = e0 + e1 + e2;
  if (d == 0) return false;
  // Scaled hit distance and time range test.
  p0t.z *= sz;
  p1t.z *= sz;
  p2t.z *= sz;
  // Avoid dividing.
  Float t_scaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
  if (d < 0 && (t_scaled >= 0 || t_scaled < ray.t_max * d))
    return false;
  else if (d > 0 && (t_scaled <= 0 || t_scaled > ray.t_max * d))
    return false;
  // Barycentric coords and hit time.
  Float d_inv = 1.0 / d;
  Float b0 = e0 * d_inv;
  Float b1 = e1 * d_inv;
  Float b2 = e2 * d_inv;
  Float t = t_scaled * d_inv;
  // Partial derivatives.
  // --------------------
  Vector3f dpdu, dpdv;
  Point2f uv[3];
  uv_values(uv);
  // Deltas for triangle partial derivatives.
  Vector2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
  Vector3f dp02 = p0 - p2, dp12 = p1 - p2;
  Float det = duv02[0] * duv12[1] - duv02[1] * duv12[0];
  bool degenerateUV = std::abs(det) < 1e-8;
  if (!degenerateUV) {
    Float det_inv = 1 / det;
    dpdu = (duv12[1] * dp02 - duv02[1] * dp12) * det_inv;
    dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * det_inv;
  }
  if (degenerateUV || cross(dpdu, dpdv).length2() == 0) {
    // Zero determinant for triangle partial derivative matrix.
    Vector3f ng = cross(p2 - p0, p1 - p0);
    if (ng.length2() == 0)
      // The triangle is degenerate. Fake hit.
      return false;
    make_coord_system(normalize(ng), &dpdu, &dpdv);
  }
  // Interpolate uv and hit point.
  // -----------------------------
  Point3f p_hit = b0 * p0 + b1 * p1 + b2 * p2;
  Point2f uv_hit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];
  // Fill SurfaceInteraction.
  // ------------------------
  *si = SurfaceInteraction(p_hit, uv_hit, -ray.dir, dpdu, dpdv, ray.time, this);
  // Repair potential bad modeling.
  si->n = si->shading.n = Normal3f(normalize(cross(dp02, dp12)));
  if (flip_normal ^ swap_handness) si->n = si->shading.n = -(si->n);
  // Shadings.
  if (m_parent_mesh->vnormal) {
    // Initialize shading geometry
    // Compute shading normal ns
    Normal3f ns;
    if (m_parent_mesh->vnormal) {
      ns = (b0 * m_parent_mesh->vnormal[vidx[0]] +
            b1 * m_parent_mesh->vnormal[vidx[1]] +
            b2 * m_parent_mesh->vnormal[vidx[2]]);
    }
    if (ns.length2() > 0)
      ns = normalize(ns);
    else
      ns = si->n;
    // Shading tangent.
    Vector3f ns_tan;
    if (ns_tan.length2() > 0)
      ns_tan = normalize(ns_tan);
    else
      ns_tan = normalize(si->dpdu);

    // Shading bitangent.
    Vector3f ns_bitan = cross(ns_tan, ns);
    if (ns_bitan.length2() > 0.f) {
      ns_bitan = normalize(ns_bitan);
      ns_tan = cross(ns_bitan, ns);
    } else {
      make_coord_system((Vector3f)ns, &ns_tan, &ns_bitan);
    }
    if (flip_normal) ns_bitan = -ns_bitan;
    si->set_shading_geometry(ns_tan, ns_bitan, true);
  }
  // Return.
  // -------
  *thit = t;
  return true;
}
Interaction Triangle::sample_surface(const Point2f &u, Float *pdf_value) const {
  Point2f bary = triangle_uniform_sample(u);
  const Point3f &p0 = m_parent_mesh->vpos[vidx[0]];
  const Point3f &p1 = m_parent_mesh->vpos[vidx[1]];
  const Point3f &p2 = m_parent_mesh->vpos[vidx[2]];
  Interaction inter;
  inter.p = bary[0] * p0 + bary[0] * p1 + (1 - bary[0] - bary[1]) * p2;
  inter.n = Normal3f(normalize(cross(p1 - p0, p2 - p0)));
  if (m_parent_mesh->vnormal) {
    // Geometric normal should stay with geometric representation.
    Normal3f shading_n =
        normalize(bary[0] * m_parent_mesh->vnormal[vidx[0]] +
                  bary[1] * m_parent_mesh->vnormal[vidx[1]] +
                  (1 - bary[0] - bary[1]) * m_parent_mesh->vnormal[vidx[2]]);
    inter.n = align_with(inter.n, shading_n);
  } else if (flip_normal ^ swap_handness) {
    inter.n *= -1;
  }
  if (pdf_value) *pdf_value = 1.0 / area();
  return inter;
}
Float Triangle::area() const {
  const Point3f &p0 = m_parent_mesh->vpos[vidx[0]];
  const Point3f &p1 = m_parent_mesh->vpos[vidx[1]];
  const Point3f &p2 = m_parent_mesh->vpos[vidx[2]];
  return 0.5 * cross(p1 - p0, p2 - p0).length();
}

/// @brief Create a TriangleMesh and return Triangle vector.
///        Pointer to mesh is stored in each Triangle.
/// @param obj_world Transform the very triangle.
/// @param world_obj
/// @param flip_normal true to flip every shape normal.
/// @param n_triangles Number of triangles.
/// @param vertex_indices Array of vertex indices, three for each triangle.
///                       Layout: [t0_0, t0_1, t0_2, t1_0, ...].
/// @param vertices Array of vertices, access from index array only.
/// @param tan_vectors Array of tangent vectors, one for each vertex.
///                    Optional. Used for computing shading space.
/// @param vertex_normals Array of vertex normals. Optional.
/// @param vertex_uv Array of vertex uv. Optional.
/// @return Vector of Triangles.
std::vector<std::shared_ptr<Shape>> create_triangle_mesh(
    const Transform &obj_world, const Transform &world_obj, bool flip_normal,
    int n_triangles, const int *vertex_indices, int n_vertices,
    const Point3f *vertices, const Normal3f *vertex_normals,
    const Point2f *vertex_uv) {
  std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>(
      obj_world, n_triangles, vertex_indices, n_vertices, vertices,
      vertex_normals, vertex_uv);
  std::vector<std::shared_ptr<Shape>> triangle_shapes;
  // Avoid memory extending.
  triangle_shapes.reserve(n_triangles);
  for (int i = 0; i < n_triangles; ++i)
    triangle_shapes.push_back(
        std::make_shared<Triangle>(obj_world, world_obj, flip_normal, mesh, i));
  return triangle_shapes;
}
}  // namespace TRay