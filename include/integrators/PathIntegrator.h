#pragma once
#include "core/Integrator.h"
/**
 * The original idea for path tracing comes from the "area form" of light
 * transport equation, where instead of integrate over a sphere of directions,
 * we compute the integrand over surface of objects in the scene. The Jacobian
 * is needed to transform from solid angle to area.
 *
 * Once we have domain being area, it's natural to sample the points in the
 * scene (on the surface, if no participating media), construct a path
 * connecting camera and some light source, test the visibility and compute how
 * mush light accumulates through this very path.
 *
 * But simply sampling points will suffer from upcoming visibility test. So at
 * each point we still sample a ditrction and trace a ray along it, constructing
 * path incrementally, with a correlation of each new path being just one more
 * point attached to previous old path.
 *
 */

namespace TRay {
class PathIntegrator : public SamplerIntegrator {
 public:
  PathIntegrator(int max_depth, std::shared_ptr<const Camera> &camera,
                 std::shared_ptr<Sampler> &sampler)
      : SamplerIntegrator(camera, sampler), m_max_depth(max_depth) {}
  Spectrum Li(const Ray &ray, const Scene &scene, Sampler &sampler,
              int depth = 0) const override;

 private:
  const int m_max_depth;
};
}  // namespace TRay