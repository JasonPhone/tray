/**
 * @file TRay.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Global includes, defines, consts, inlines and types.
 * @version 0.1
 * @date 2023-03-11
 */
#pragma once
#include <assert.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#ifdef TRAY_FLOAT_AS_DOUBLE
using Float = double;
#else
using Float = float;
#endif

// Logging and assert warp
#define ASSERT(x) assert((x))
#define PEEK(x) std::cerr << #x << ": " << x << "\n";
#define SInfo(msg) std::cerr << "[I]"#msg << "\n"
#define SWarn(msg) std::cerr << "[W]"#msg << "\n"
#define SError(msg) std::cerr << "[E]"#msg << "\n"
#define SCritical(msg) std::cerr << "[C]"#msg << "\n"

#ifndef NDEBUG
#define SDebug(msg) std::cerr << "[D]"#msg << "\n"
#else
#define SDebug(msg) std::cerr << msg << "\n"
#endif

// Platform.
#if defined(_WIN32) || defined(_WIN64)
#define TRAY_ON_WINDOWS
#endif
#if defined(_MSC_VER)
#define TRAY_ON_MSVC
// #if _MSC_VER == 1800
// #define snprintf _snprintf
// #endif
#endif

// Memory management. Stack in-place memory.
#define ALLOCA(TYPE, COUNT) (TYPE *)alloca((COUNT) * sizeof(TYPE))
#ifndef TRAY_L1_CACHELINE_SIZE
#define TRAY_L1_CACHELINE_SIZE 64
#endif

namespace TRay {
// Forward decl.
// TODO reorder to show the structure
// core/geometry/Vector.h
template <typename T> class Vector3;
using Vector3i = Vector3<int>;
using Vector3f = Vector3<Float>;
template <typename T> class Vector2;
using Vector2i = Vector2<int>;
using Vector2f = Vector2<Float>;
// core/geometry/Point.h
template <typename T> class Point3;
using Point3i = Point3<int>;
using Point3f = Point3<Float>;
template <typename T> class Point2;
using Point2i = Point2<int>;
using Point2f = Point2<Float>;
template <typename T>
// core/geometry/Normal.h
class Normal3;
using Normal3f = Normal3<Float>;
// core/geometry/Ray.h
class Ray;
// core/geometry/Bound.h
template <typename T> class Bound3;
using Bound3i = Bound3<int>;
using Bound3f = Bound3<Float>;
template <typename T> class Bound2;
using Bound2i = Bound2<int>;
using Bound2f = Bound2<Float>;
// core/geometry/Transform.h
class Transform;
// core/geometry/Quaternion.h
class Quaternion;
// core/geometry/Interaction.h
class Interaction;
class SurfaceInteraction;
// core/geometry/Shape.h
class Shape;
// shapes/Sphere.h
class Sphere;
// shapes/TriangleMesh.h
class Triangle;
struct TriangleMesh;
// core/math/Matrix.h
struct Mat4x4;
// core/math/RNG.h
class RNG;
// core/math/sampling.h
struct Distribution1D;
// core/primitives/Primitive.h
class Primitive;
// core/primitives/GeometricPrimitive.h
class GeometricPrimitive;
// core/primitives/Aggregate.h
class Aggregate;
// accelerators/LinearAccel.h
class LinearAccel;
// core/spectrum/CoefficientSpectrum.h
template <int n_samples_t> class CoefficientSpectrum;
// core/spectrum/RGBSpectrum.h
class RGBSpectrum;
using Spectrum = RGBSpectrum;
// Camera.h
class Camera;
class ProjectiveCamera;
// cameras/PerspectiveCamera.h
class PerspectiveCamera;
// core/Film.h
class Film;
class FilmTile;
// core/Filter.h
class Filter;
// core/Sampler.h
class Sampler;
class PixelSampler;
class GlobalSampler;
// core/reflection/BxDF.h
class BxDF;
class ScaledBxDF;
// core/reflection/Fresnel.h
class Fresnel;
class FresnelDielectric;
class FresnelConductor;
class FresnelConst;
// core/reflection/FresnelSpecular.h
class SpecularReflection;
class SpecularTransmission;
class FresnelSpcular;
// core/reflection/Lambertian.h
class LambertianReflection;
class LambertianTransmission;
// core/reflection/BSDF.h
class BSDF;
// core/Material.h
class Material;
enum class TransportMode;
// materials/MatteMaterial.h
class MatteMaterial;
// materials/MirrorMaterial.h
class MirrorMaterial;
// core/Texture.h
class TextureMapping2D;
class UVMapping2D;
class SphericalMapping2D;
class PlanarMapping2D;
class TextureMapping3D;
class TransformMapping3D;
template <typename T> class Texture;
// textures/ConstantTexture.h
template <typename T> class ConstantTexture;
// textures/ScaleTexture.h
template <typename T1, typename T2> class ScaleTexture;
// textures/LerpTexture.h
template <typename T> class LerpTexture;
// textures/BiLerpTexture.h
template <typename T> class BiLerpTexture;
// core/Ligtht.h
class Light;
class VisibilityTester;
// core/Scene.h
class Scene;
// core/Light.h
class AreaLight;
// lights/DiffuseAreaLight.h
class DiffuseAreaLight;
// lights/DistantLight.h
class DistantLight;
// core/Integrator.h
class Integrator;
class SamplerIntegrator;
// integrators/WhittedIntegrator.h
class WhittedIntegrator;
// integrators/DirectIntegrator.h
class DirectIntegrator;
// core/Memory.h
class MemoryPool;

// ---------------------

// Math constants.
// ---------------
static constexpr Float TRAY_MAX = std::numeric_limits<Float>::max();
static constexpr Float TRAY_INF = std::numeric_limits<Float>::infinity();
static constexpr Float PI = 3.14159265358979323846;
static constexpr Float PI_INV = 0.31830988618379067154;
// \frac{1}{2 \pi}
static constexpr Float PI_INV2 = 0.15915494309189533577;
// \frac{1}{4 \pi}
static constexpr Float PI_INV4 = 0.07957747154594766788;
static constexpr Float PI_DIV_2 = 1.57079632679489661923;
static constexpr Float PI_DIV_4 = 0.78539816339744830961;
static constexpr Float TRAY_EPS = std::numeric_limits<Float>::epsilon() * 0.5;
static constexpr Float SQRT2 = 1.41421356237309504880;
// static constexpr Float DoubleOneMinusEpsilon = 0x1.fffffffffffffp-1;
// static constexpr Float FloatOneMinusEpsilon = 0x1.fffffep-1;

// Math functions.
// ---------------
template <typename T> inline T mod(T a, T b) {
  T result = a - (a / b) * b;
  return T((result < 0) ? result + b : result);
}
inline Float lerp(Float a, Float b, Float t) { return (1 - t) * a + t * b; }
template <typename T, typename U, typename V>
inline T clamp(T val, U low, V high) {
  return (val < low) ? low : ((val > high) ? high : val);
}
inline Float deg_to_rad(Float deg) { return deg / 180.0 * PI; }
inline Float gamma(int n) { return (n * TRAY_EPS) / (1 - n * TRAY_EPS); }
/**
 * @brief Solving quadratic formulation ax^2 + bx + c = 0.
 *
 * @param t0 Store the solutions.
 * @param t1 t0 < t1 is guaranteed.
 */
inline bool solve_quadratic(Float a, Float b, Float c, Float *t0, Float *t1) {
  double discrim = (double)b * (double)b - 4 * (double)a * (double)c;
  if (discrim < 0)
    return false;
  double discrim_rt = std::sqrt(discrim);

  double q;
  if (b < 0)
    q = -.5 * (b - discrim_rt);
  else
    q = -.5 * (b + discrim_rt);
  *t0 = q / a;
  *t1 = c / q;
  if (*t0 > *t1)
    std::swap(*t0, *t1);
  return true;
}
/// @brief Get the smallest power of 2 non-less than @param v.
inline int32_t pow2_ceil(int32_t v) {
  v--;
  // Make all ones.
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  return v + 1;
}
inline int64_t pow2_ceil(int64_t v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v |= v >> 32;
  return v + 1;
}
template <typename T> inline constexpr bool is_pow2(T v) {
  return v && !(v & (v - 1));
}
/// @brief Integer floor of log2( @param v )
inline int log2_int(uint32_t v) { return 31 - __builtin_clz(v); }
inline uint32_t float_to_bits(float f) {
  uint32_t ui;
  memcpy(&ui, &f, sizeof(float));
  return ui;
}
inline float bits_to_float(uint32_t ui) {
  float f;
  memcpy(&f, &ui, sizeof(uint32_t));
  return f;
}
inline uint64_t float_to_bits(double f) {
  uint64_t ui;
  memcpy(&ui, &f, sizeof(double));
  return ui;
}
inline double bits_to_float(uint64_t ui) {
  double f;
  memcpy(&f, &ui, sizeof(uint64_t));
  return f;
}
inline float next_float_up(float v) {
  // Handle infinity and negative zero for _NextFloatUp()_
  if (std::isinf(v) && v > 0.)
    return v;
  if (v == -0.f)
    v = 0.f;
  // Advance _v_ to next higher float
  uint32_t ui = float_to_bits(v);
  if (v >= 0)
    ++ui;
  else
    --ui;
  return bits_to_float(ui);
}
inline float next_float_down(float v) {
  // Handle infinity and positive zero for _NextFloatDown()_
  if (std::isinf(v) && v < 0.)
    return v;
  if (v == 0.f)
    v = -0.f;
  uint32_t ui = float_to_bits(v);
  if (v > 0)
    --ui;
  else
    ++ui;
  return bits_to_float(ui);
}
inline double next_float_up(double v, int delta = 1) {
  if (std::isinf(v) && v > 0.)
    return v;
  if (v == -0.f)
    v = 0.f;
  uint64_t ui = float_to_bits(v);
  if (v >= 0.)
    ui += delta;
  else
    ui -= delta;
  return bits_to_float(ui);
}
inline double next_float_down(double v, int delta = 1) {
  if (std::isinf(v) && v < 0.)
    return v;
  if (v == 0.f)
    v = -0.f;
  uint64_t ui = float_to_bits(v);
  if (v > 0.)
    ui -= delta;
  else
    ui += delta;
  return bits_to_float(ui);
}

// Other inlines.
// --------------
/// @brief Count trailing zeros.
inline int ctz(uint32_t v) { return __builtin_ctz(v); }
/// @brief gamma correction for color value.
inline Float gamma_correct(Float value) {
  if (value <= 0.0031308f)
    return 12.92f * value;
  return 1.055f * std::pow(value, (Float)(1.f / 2.4f)) - 0.055f;
}
/// @tparam Test Lambda function.
/// @param size Size of sequence.
/// @param test Test function, accepts an index and returns boolean.
///             array[index] < target to get lower bound and <= to get upper.
template <typename Test> int binary_search(int size, const Test &test) {
  int first = 0, len = size;
  while (len > 0) {
    int half = len >> 1, mid = first + half;
    if (test(mid)) {
      first = mid + 1;
      len -= half + 1;
    } else {
      len = half;
    }
  }
  return first;
}
} // namespace TRay