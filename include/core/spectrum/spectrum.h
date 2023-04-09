#pragma once
#include "core/spectrum/RGBSpectrum.h"
// #include "core/spectrum/CoefficientSpectrum.h"

/**
 * Spectrum is switched by `using Spectrum = XXXSpectrum;`.
 * CoefficientSpectrum is the base class, which stores a fixed number of samples
 * of SPD.
 */
namespace TRay {
/// @brief Blackbody radiance based on Planck's law.
/// @param lambdas Sample points in wavelength(nm).
/// @param n_lambdas Number of wavelength samples.
/// @param T Temperature(Kelvin).
/// @param Le Result emitted radiance.
inline void blackbody(const Float lambdas[], int n_lambdas, Float T, Float *Le) {
  // Constants.
  const Float c = 299792458;       // Light speed(m/s) in vacuum.
  const Float h = 6.62606957e-34;  // Planck's constant(Js).
  const Float kb = 1.3806488e-23;  // Boltzmann constant(J/K).
  // By each wavelength sample.
  for (int i = 0; i < n_lambdas; i++) {
    Float lambda = lambdas[i] * 1e-9;  // nm to m.
    Float lambda5 = (lambda * lambda) * (lambda * lambda) * lambda;
    Le[i] = (2 * h * c * c) /
            (lambda5 * (std::exp((h * c) / (lambda * kb * T)) - 1));
  }
}
/// @brief Blackbody radiance based on Planck's law, normalized.
/// @param lambdas Sample points in wavelength(nm).
/// @param n_lambdas Number of wavelength samples.
/// @param T Temperature(Kelvin).
/// @param Le Result emitted radiance.
inline void blackbody_normalized(const Float lambdas[], int n_lambdas, Float T,
                          Float *Le) {
  blackbody(lambdas, n_lambdas, T, Le);
  Float lambda_max = 2.8977721e-3 / T * 1e9;  // Wien's displacement law.
  Float L_max;
  blackbody(&lambda_max, 1, T, &L_max);
  for (int i = 0; i < n_lambdas; i++) {
    Le[i] /= lambda_max;
  }
}
}  // namespace TRay