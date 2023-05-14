#include "shapes/TriangleMesh.h"

#include "core/geometry/Bound.h"
#include "core/geometry/Normal.h"
#include "core/geometry/Point.h"
#include "core/geometry/Transform.h"
#include "core/math/sampling.h"

namespace TRay {
TriangleMesh::TriangleMesh(const Transform &obj_to_world, int _n_triangles,
                           const int *vertex_indices, int _n_vertices,
                           const Point3f *vertices,
                           const Normal3f *vertex_normals,
                           const Point2f *vertex_uv)
    : n_triangles(_n_triangles),
      n_vertices(_n_vertices),
      vindex(vertex_indices, vertex_indices + 3 * _n_triangles) {
  // Point3f* vertices.
  vpos.reset(new Point3f[_n_vertices]);
  for (int i = 0; i < _n_vertices; i++) vpos[i] = obj_to_world(vertices[i]);
  // Optional Normal3f* vertex_normals.
  if (vertex_normals) {
    vnormal.reset(new Normal3f[_n_vertices]);
    for (int i = 0; i < _n_vertices; ++i)
      vnormal[i] = obj_to_world(vertex_normals[i]);
  }
  // Optional Point2f* vertex_uv.
  if (vertex_uv) {
    vuv.reset(new Point2f[_n_vertices]);
    memcpy(vuv.get(), vertex_uv, _n_vertices * sizeof(Point2f));
  }
}

Triangle::Triangle(const Transform &obj_world, const Transform &world_obj,
                   bool _flip_normal,
                   const std::shared_ptr<TriangleMesh> parent_mesh,
                   int triangle_index)
    : Shape(obj_world, world_obj, _flip_normal),
      m_parent_mesh(parent_mesh),
      vidx(&(parent_mesh->vindex[3 * triangle_index])) {}
Bound3f Triangle::object_bound() const {
  return (*world_to_obj)(world_bound());
}
Bound3f Triangle::world_bound() const {
  const Point3f &p0 = m_parent_mesh->vpos[vidx[0]];
  const Point3f &p1 = m_parent_mesh->vpos[vidx[1]];
  const Point3f &p2 = m_parent_mesh->vpos[vidx[2]];
  Bound3f bound = bound_insert(Bound3f(p0, p1), p2);
  // In case of axis-parallel triangle.
  bound.p_min -= Vector3f(0.01, 0.01, 0.01);
  bound.p_max += Vector3f(0.01, 0.01, 0.01);
  return bound;
}
bool Triangle::intersect(const Ray &ray, Float *thit, SurfaceInteraction *si,
                         bool) const {
  // Get triangle vertices.
  const Point3f &p0 = m_parent_mesh->vpos[vidx[0]];
  const Point3f &p1 = m_parent_mesh->vpos[vidx[1]];
  const Point3f &p2 = m_parent_mesh->vpos[vidx[2]];

  // Perform ray--triangle intersection test

  // Transform triangle vertices to ray coordinate space

  // Translate vertices based on ray origin
  Point3f p0t = p0 - Vector3f(ray.ori);
  Point3f p1t = p1 - Vector3f(ray.ori);
  Point3f p2t = p2 - Vector3f(ray.ori);

  // Permute components of triangle vertices and ray direction
  int kz = max_dim(abs(ray.dir));
  int kx = kz + 1;
  if (kx == 3) kx = 0;
  int ky = kx + 1;
  if (ky == 3) ky = 0;
  Vector3f d = permute(ray.dir, kx, ky, kz);
  p0t = permute(p0t, kx, ky, kz);
  p1t = permute(p1t, kx, ky, kz);
  p2t = permute(p2t, kx, ky, kz);

  // Apply shear transformation to translated vertex positions
  Float Sx = -d.x / d.z;
  Float Sy = -d.y / d.z;
  Float Sz = 1.f / d.z;
  p0t.x += Sx * p0t.z;
  p0t.y += Sy * p0t.z;
  p1t.x += Sx * p1t.z;
  p1t.y += Sy * p1t.z;
  p2t.x += Sx * p2t.z;
  p2t.y += Sy * p2t.z;

  // Compute edge function coefficients _e0_, _e1_, and _e2_
  Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
  Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
  Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

  // Fall back to double precision test at triangle edges
  if (sizeof(Float) == sizeof(float) &&
      (e0 == 0.0f || e1 == 0.0f || e2 == 0.0f)) {
    double p2txp1ty = (double)p2t.x * (double)p1t.y;
    double p2typ1tx = (double)p2t.y * (double)p1t.x;
    e0 = (float)(p2typ1tx - p2txp1ty);
    double p0txp2ty = (double)p0t.x * (double)p2t.y;
    double p0typ2tx = (double)p0t.y * (double)p2t.x;
    e1 = (float)(p0typ2tx - p0txp2ty);
    double p1txp0ty = (double)p1t.x * (double)p0t.y;
    double p1typ0tx = (double)p1t.y * (double)p0t.x;
    e2 = (float)(p1typ0tx - p1txp0ty);
  }

  // Perform triangle edge and determinant tests
  if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
    return false;
  Float det = e0 + e1 + e2;
  if (det == 0) return false;

  // Compute scaled hit distance to triangle and test against ray $t$ range
  p0t.z *= Sz;
  p1t.z *= Sz;
  p2t.z *= Sz;
  Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
  if (det < 0 && (tScaled >= 0 || tScaled < ray.t_max * det))
    return false;
  else if (det > 0 && (tScaled <= 0 || tScaled > ray.t_max * det))
    return false;

  // Compute barycentric coordinates and $t$ value for triangle intersection
  Float invDet = 1 / det;
  Float b0 = e0 * invDet;
  Float b1 = e1 * invDet;
  Float b2 = e2 * invDet;
  Float t = tScaled * invDet;

  // Ensure that computed triangle $t$ is conservatively greater than zero

  // Compute $\delta_z$ term for triangle $t$ error bounds
  Float maxZt = max_component(abs(Vector3f(p0t.z, p1t.z, p2t.z)));
  Float deltaZ = gamma(3) * maxZt;

  // Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
  Float maxXt = max_component(abs(Vector3f(p0t.x, p1t.x, p2t.x)));
  Float maxYt = max_component(abs(Vector3f(p0t.y, p1t.y, p2t.y)));
  Float deltaX = gamma(5) * (maxXt + maxZt);
  Float deltaY = gamma(5) * (maxYt + maxZt);

  // Compute $\delta_e$ term for triangle $t$ error bounds
  Float deltaE =
      2 * (gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

  // Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
  Float maxE = max_component(abs(Vector3f(e0, e1, e2)));
  Float deltaT = 3 *
                 (gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) *
                 std::abs(invDet);
  if (t <= deltaT) return false;

  // Compute triangle partial derivatives
  Vector3f dpdu, dpdv;
  Point2f uv[3];
  uv_values(uv);

  // Compute deltas for triangle partial derivatives
  Vector2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
  Vector3f dp02 = p0 - p2, dp12 = p1 - p2;
  Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
  bool degenerateUV = std::abs(determinant) < 1e-8;
  if (!degenerateUV) {
    Float invdet = 1 / determinant;
    dpdu = (duv12[1] * dp02 - duv02[1] * dp12) * invdet;
    dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * invdet;
  }
  if (degenerateUV || cross(dpdu, dpdv).length2() == 0) {
    // Handle zero determinant for triangle partial derivative matrix
    Vector3f ng = cross(p2 - p0, p1 - p0);
    if (ng.length2() == 0)
      // The triangle is actually degenerate.
      return false;
    make_coord_system(normalize(ng), &dpdu, &dpdv);
  }

  // Compute error bounds for triangle intersection
  Float xAbsSum =
      (std::abs(b0 * p0.x) + std::abs(b1 * p1.x) + std::abs(b2 * p2.x));
  Float yAbsSum =
      (std::abs(b0 * p0.y) + std::abs(b1 * p1.y) + std::abs(b2 * p2.y));
  Float zAbsSum =
      (std::abs(b0 * p0.z) + std::abs(b1 * p1.z) + std::abs(b2 * p2.z));
  Vector3f pError = gamma(7) * Vector3f(xAbsSum, yAbsSum, zAbsSum);

  // Interpolate $(u,v)$ parametric coordinates and hit point
  Point3f pHit = b0 * p0 + b1 * p1 + b2 * p2;
  Point2f uvHit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];

  // Fill in _SurfaceInteraction_ from triangle hit
  *si = SurfaceInteraction(pHit, pError, uvHit, -ray.dir, dpdu, dpdv, ray.time,
                           this);

  // Override surface normal in _isect_ for triangle
  si->n = si->shading.n = Normal3f(normalize(cross(dp02, dp12)));
  if (flip_normal ^ swap_handness) si->n = si->shading.n = -si->n;

  *thit = t;
  return true;
}
Interaction Triangle::sample_surface(const Point2f &u, Float *pdf_value) const {
  Point2f bary = triangle_uniform_sample(u);
  const Point3f &p0 = m_parent_mesh->vpos[vidx[0]];
  const Point3f &p1 = m_parent_mesh->vpos[vidx[1]];
  const Point3f &p2 = m_parent_mesh->vpos[vidx[2]];
  Interaction inter;
  inter.p = bary[0] * p0 + bary[1] * p1 + (1 - bary[0] - bary[1]) * p2;
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
  Point3f perr =
      abs(bary[0] * p0) + abs(bary[1] * p1) + abs((1 - bary[0] - bary[1]) * p2);
  inter.m_perr = gamma(6) * Vector3f(perr.x, perr.y, perr.z);
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
  SInfo(
      string_format("Creating triangle mesh with"
                    "\n\t%d triangles"
                    "\n\t%d vertices",
                    n_triangles, n_vertices));
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