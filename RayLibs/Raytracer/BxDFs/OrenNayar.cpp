#include "OrenNayar.h"
#include <Math/MathRoutines.h>

OrenNayar::OrenNayar(Spectrum_d i_reflectance, double i_sigma):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), m_reflectance(i_reflectance)
  {
  ASSERT(InRange(i_reflectance,0.0,1.0));
  ASSERT(i_sigma>=0.0 && i_sigma<=1.0);

  double sigma_sqr = i_sigma*i_sigma;
  m_A = 1.0 - (0.5 * sigma_sqr / (sigma_sqr + 0.33));
  m_B = 0.45 * sigma_sqr / (sigma_sqr + 0.09);
  }

// The method does not check if the vectors are in the same hemisphere.
Spectrum_d OrenNayar::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  double cos_theta_incident = i_incident[2], cos_theta_exitant = i_exitant[2];

  double dcos = i_incident[0] * i_exitant[0] + i_incident[1] * i_exitant[1];
  double max_cos = std::max(0.0, dcos);

  if (fabs(cos_theta_incident) > fabs(cos_theta_exitant))
    return m_reflectance * (INV_PI * (m_A + m_B * max_cos / fabs(cos_theta_incident)));
  else
    return m_reflectance * (INV_PI * (m_A + m_B * max_cos / fabs(cos_theta_exitant)));
  }