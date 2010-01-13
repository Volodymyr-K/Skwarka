#include "Lambertian.h"
#include <Math/Constants.h>

Lambertian::Lambertian(Spectrum_d i_reflectance):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), m_reflectance(i_reflectance), m_reflectance_inv_pi(i_reflectance*INV_PI)
  {
  ASSERT(InRange(i_reflectance,0.0,1.0));
  }

// The method does not check if the vectors are in the same hemisphere and returns constant value everywhere.
Spectrum_d Lambertian::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());
  return m_reflectance_inv_pi;
  }

Spectrum_d Lambertian::TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const
  {
  ASSERT(i_incident.IsNormalized());
  return m_reflectance;
  }

Spectrum_d Lambertian::TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples) const
  {
  return m_reflectance;
  }