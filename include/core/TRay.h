/**
 * @file tray.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Global includes, defines, consts, inlines and types.
 * @version 0.1
 * @date 2023-03-11
 */
#pragma once
#include <iostream>
#include <cmath>
#include <utility>
#include <assert.h>
#include <memory>

#ifdef TRAY_FLOAT_AS_DOUBLE
using Float = double;
#else
using Float = float;
#endif

// Logging and assert warp
#define ASSERT(x) assert((x))
#define PEEK(x) std::cerr << #x << ": " << x << "\n";
// For compiling speed.
#ifndef NDEBUG
#include "spdlog/spdlog.h"
#define SInfo(msg) spdlog::info(msg)
#define FInfo(fmt, msg...) spdlog::info(fmt, ##msg)
#define SWarn(msg) spdlog::WARN(msg)
#define FWarn(fmt, msg...) spdlog::WARN(fmt, ##msg)
#define SError(msg) spdlog::error(msg)
#define FError(fmt, msg...) spdlog::error(fmt, ##msg)
#define SCritical(msg) spdlog::critical(msg)
#define FCritical(fmt, msg...) spdlog::critical(fmt, ##msg)
#else
#define SInfo(msg) std::cerr << msg << "\n";
#define FInfo(fmt, msg...)
#define SWarn(msg) std::cerr << msg << "\n";
#define FWarn(fmt, msg...)
#define SError(msg) std::cerr << msg << "\n";
#define FError(fmt, msg...)
#define SCritical(msg) std::cerr << msg << "\n";
#define FCritical(fmt, msg...)
#endif

namespace TRay {
// core/geometry/Vector.h
// ----------------------
template <typename T>
class Vector3;
using Vector3i = Vector3<int>;
using Vector3f = Vector3<Float>;
template <typename T>
class Vector2;
using Vector2i = Vector2<int>;
using Vector2f = Vector2<Float>;
// core/geometry/Point.h
// ---------------------
template <typename T>
class Point3;
using Point3i = Point3<int>;
using Point3f = Point3<Float>;
template <typename T>
class Point2;
using Point2i = Point2<int>;
using Point2f = Point2<Float>;
template <typename T>
// core/geometry/Normal.h
// ------------------------
class Normal3;
using Normal3f = Normal3<Float>;
// core/geometry/Ray.h
// -------------------
class Ray;
template <typename T>
// core/geometry/Bound.h
// ---------------------
class Bound3;
using Bound3i = Bound3<int>;
using Bound3f = Bound3<Float>;
template <typename T>
class Bound2;
using Bound2i = Bound2<int>;
using Bound2f = Bound2<Float>;
// core/geometry/Transform.h
// -------------------------
class Transform;
// core/geometry/AnimateTransform.h
// -------------------------
class AnimateTransform;
// core/geometry/Quaternion.h
// --------------------------
class Quaternion;
// core/geometry/Interaction.h
// ---------------------------
class Interaction;
class SurfaceInteraction;
// core/geometry/Shape.h
// ------------------
class Shape;
// shapes/Sphere.h
// ------------------
class Sphere;
// core/math/Matrix.h
struct Mat4x4;
// core/primitives/Primitive.h
class Primitive;
// core/primitives/GeometricPrimitive.h
class GeometricPrimitive;
// core/primitives/TransformedPrimitive.h
class TransformedPrimitive;
// core/lights/Ligtht.h
class Light;
class AreaLight;
// core/materials/Material.h
class Material;
enum class TransportMode;
// core/reflection?
class BSDF;

// Math constants.
// ---------------
static constexpr Float FLOAT_MAX = std::numeric_limits<Float>::max();
static constexpr Float FLOAT_INF = std::numeric_limits<Float>::infinity();
static constexpr Float PI = 3.14159265358979323846;
static constexpr Float FLOAT_EPS = std::numeric_limits<Float>::epsilon() * 0.5;

// Math functions.
// ---------------
inline Float lerp(Float a, Float b, Float t) { return (1 - t) * a + t * b; }
template <typename T, typename U, typename V>
inline T clamp(T val, U low, V high) {
  return (val < low) ? low : ((val > high) ? high : val);
}
inline Float deg_to_rad(Float deg) { return deg / 180.0 * PI; }
inline Float gamma(int n) { return (n * FLOAT_EPS) / (1 - n * FLOAT_EPS); }
/**
 * @brief Solving quadratic formulation ax^2 + bx + c = 0.
 * 
 * @param t0 Store the solutions.
 * @param t1 t0 < t1 is guaranteed.
 */
inline bool solve_quadratic(Float a, Float b, Float c, Float *t0, Float *t1) {
  double discrim = (double)b * (double)b - 4 * (double)a * (double)c;
  if (discrim < 0) return false;
  double discrim_rt = std::sqrt(discrim);

  double q;
  if (b < 0)
    q = -.5 * (b - discrim_rt);
  else
    q = -.5 * (b + discrim_rt);
  *t0 = q / a;
  *t1 = c / q;
  if (*t0 > *t1) std::swap(*t0, *t1);
  return true;
}
}  // namespace TRay