#include "Lambertian.h"
#include <Math/Constants.h>

Lambertian::Lambertian(Spectrum_d i_reflectance):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), m_reflectance(i_reflectance), m_reflectance_inv_pi(i_reflectance*INV_PI)
  {
  ASSERT(i_reflectance[0]>=0.0 && i_reflectance[0]<=1.0);
  ASSERT(i_reflectance[1]>=0.0 && i_reflectance[1]<=1.0);
  ASSERT(i_reflectance[2]>=0.0 && i_reflectance[2]<=1.0);
  }

Spectrum_d Lambertian::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  bool same_hemisphere = i_incident[2]*i_exitant[2] > 0.0;

  if (same_hemisphere)
    return m_reflectance_inv_pi;
  else
    return Spectrum_d(0.0);
  }

Spectrum_d Lambertian::TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const
  {
  return m_reflectance;
  }

Spectrum_d Lambertian::TotalScattering(SamplesSequence2D i_samples) const
  {
  return m_reflectance;
  }