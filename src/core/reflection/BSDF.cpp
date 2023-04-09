#include "core/reflection/BSDF.h"
#include "core/reflection/BxDF.h"
#include "core/geometry/Normal.h"

namespace TRay {

BSDF::BSDF(const SurfaceInteraction &si, Float eta)
    : m_eta(eta),
      m_normal_s(si.shading.n),
      m_normal_g(si.n),
      m_s_tan(si.shading.dpdu),
      m_s_bitan(cross(m_normal_s, m_s_tan)) {}
void BSDF::add_BxDF(BxDF &b) { m_BxDFs.push_back(&b); }
int BSDF::count_BxDF(BxDFType flags = BSDF_ALL) const {
  int ret = 0;
  for (const BxDF *bxdf : m_BxDFs) {
    if (bxdf->match_types(flags)) ret++;
  }
  return ret;
}
Vector3f BSDF::world_to_local(const Vector3f v) const {
  return Vector3f(dot(m_s_tan, v), dot(m_s_bitan, v), dot(m_normal_s, v));
}
Vector3f BSDF::local_to_world(const Vector3f v) const {
  return Vector3f(m_s_tan.x * v.x + m_s_bitan.x * v.y + m_normal_s.x * v.z,
                  m_s_tan.y * v.x + m_s_bitan.y * v.y + m_normal_s.y * v.z,
                  m_s_tan.z * v.x + m_s_bitan.z * v.y + m_normal_s.z * v.z);
}
Spectrum BSDF::f(const Vector3f &wo_world, const Vector3f wi_world,
                 BxDFType flags) const {
  Vector3f wo = world_to_local(wo_world);
  Vector3f wi = world_to_local(wi_world);
  // Reflect only if both are positive.
  bool reflect = dot(wi_world, m_normal_g) * dot(wo_world, m_normal_g) > 0;
  Spectrum ret(0.0);
  for (const BxDF *bxdf : m_BxDFs) {
    if (bxdf->match_types(flags)) {
      // Correct type of BxDF, reflection or transmission.
      if ((reflect && (bxdf->m_type & BSDF_REFLECTION)) ||
          (!reflect && (bxdf->m_type & BSDF_TRANSMISSION))) {
        // Is reflection and contains reflection,
        // or is not reflection and contains transmission.
        ret += bxdf->f(wo, wi);
      }
    }
  }
  return ret;
}
Spectrum BSDF::rho(const Vector3f &wo_world, int nsamples,
                   const Point2f *samples, BxDFType flags) const {
  Vector3f wo = world_to_local(wo_world);
  Spectrum ret(0.0);
  for (const BxDF *bxdf : m_BxDFs) {
    if (bxdf->match_types(flags)) {
      // Correct type of BxDF.
      ret += bxdf->rho(wo, nsamples, samples);
    }
  }
  return ret;
}
Spectrum BSDF::rho(int n_samples, const Point2f *samples1,
                   const Point2f *samples2, BxDFType flags) const {
  Spectrum ret(0.0);
  for (const BxDF *bxdf : m_BxDFs) {
    if (bxdf->match_types(flags)) {
      // Correct type of BxDF.
      ret += bxdf->rho(n_samples, samples1, samples2);
    }
  }
  return ret;
}
Spectrum BSDF::sample_f(const Vector3f &wo_world, Vector3f *wi_world,
                        const Point2f &u, Float *pdf_value, BxDFType type,
                        BxDFType *sampled_type) const {
  // Choose matched BxDF to sample.
  int n_bxdf = num_BxDFs(type);
  if (n_bxdf == 0) {
    *pdf_value = 0;
    return Spectrum(0.0);
  }
  const BxDF *chosen_bxdf = nullptr;
  int idx = clamp(int(u[0] * n_bxdf), 0, n_bxdf - 1);
  int cnt = idx;
  for (const auto &bxdf : m_BxDFs) {
    if (bxdf->match_types(type) && cnt-- == 0) {
      chosen_bxdf = bxdf;
      break;
    }
  }
  // Recover a random variable since u[0] is used.
  // Remap u[0] to the interval of discrete indices.
  Point2f remap_u(u[0] * n_bxdf - idx, u[1]);
  // Sample chosen BxDF.
  Vector3f wi, wo = world_to_local(wo_world);
  *pdf_value = 0;
  if (sampled_type) *sampled_type = chosen_bxdf->m_type;
  Spectrum f = chosen_bxdf->sample_f(wo, &wi, remap_u, pdf_value, sampled_type);
  if (*pdf_value == 0) return Spectrum(0.0);
  *wi_world = local_to_world(wi);
  // Total pdf values of matched BxDFs.
  // Perfectly specular should be skipped since
  // delta distribution gives pdf value of one.
  if (!(chosen_bxdf->m_type & BSDF_SPECULAR) && n_bxdf > 1)
    for (const auto &bxdf : m_BxDFs)
      if (bxdf->match_types(type)) *pdf_value += bxdf->pdf(wo, wi);
  *pdf_value /= n_bxdf;
  // BSDF value of sampled direction.
  if (!(chosen_bxdf->m_type & BSDF_SPECULAR) && n_bxdf > 1) {
    bool reflect = dot(*wi_world, m_normal_g) * dot(wo_world, m_normal_g) > 0;
    f = 0.;
    for (const auto &bxdf : m_BxDFs) {
      if (bxdf->match_types(type) &&
          ((reflect && (bxdf->m_type & BSDF_REFLECTION)) ||
           (!reflect && (bxdf->m_type & BSDF_TRANSMISSION))))
        f += bxdf->f(wo, wi);
    }
  }
  SInfo("Total f = " + f.to_string() +
        ", pdf =" + format_one(" %f, ", *pdf_value) + "ratio =" +
        format_one(" %f \n",
                   ((*pdf_value > 0) ? (f / *pdf_value) : Spectrum(0.))));
  return f;
}
Float BSDF::pdf(const Vector3f &wo_world, const Vector3f &wi_world,
                BxDFType flags) const {
  if (m_BxDFs.empty()) return 0.f;
  Vector3f wo = world_to_local(wo_world), wi = world_to_local(wi_world);
  if (wo.z == 0) return 0.;
  Float pdf = 0.f;
  int n_matched = 0;
  for (const auto &bxdf : m_BxDFs)
    if (bxdf->match_types(flags)) {
      n_matched++;
      pdf += bxdf->pdf(wo, wi);
    }
  return n_matched ? pdf / n_matched : 0.0;
}
}  // namespace TRay