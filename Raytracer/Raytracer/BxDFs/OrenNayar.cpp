#include "OrenNayar.h"

OrenNayar::OrenNayar(Spectrum_d i_reflectance, double i_sigma):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), m_reflectance(i_reflectance)
  {
  ASSERT(i_reflectance[0]>=0.0 && i_reflectance[0]<=1.0);
  ASSERT(i_reflectance[1]>=0.0 && i_reflectance[1]<=1.0);
  ASSERT(i_reflectance[2]>=0.0 && i_reflectance[2]<=1.0);
  ASSERT(i_sigma>=0.0 && i_sigma<=1.0);

  double sigma_sqr = i_sigma*i_sigma;
  m_A = 1.0 - (0.5 * sigma_sqr / (sigma_sqr + 0.33));
  m_B = 0.45 * sigma_sqr / (sigma_sqr + 0.09);
  }

Spectrum_d OrenNayar::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  bool same_hemisphere = i_incident[2]*i_exitant[2] > 0.0;

  if (same_hemisphere==false)
    return Spectrum_d(0.0);

  double sin_theta_incident, cos_theta_incident, sin_phi_incident, cos_phi_incident;
  VectorToAngles(i_incident, sin_theta_incident, cos_theta_incident, sin_phi_incident, cos_phi_incident);

  double sin_theta_exitant, cos_theta_exitant, sin_phi_exitant, cos_phi_exitant;
  VectorToAngles(i_exitant, sin_theta_exitant, cos_theta_exitant, sin_phi_exitant, cos_phi_exitant);

  // Compute cosine term of Oren-Nayar model.
  double dcos = cos_phi_incident * cos_phi_exitant + sin_phi_incident * sin_phi_exitant;
  double max_cos = std::max(0.0, dcos);

  // Compute sine and tangent terms of Oren-Nayar model.
  double sin_alpha, tan_beta;
  if (fabs(cos_theta_incident) > fabs(cos_theta_exitant))
    {
    sin_alpha = sin_theta_exitant;
    tan_beta = sin_theta_incident / fabs(cos_theta_incident);
    }
  else
    {
    sin_alpha = sin_theta_incident;
    tan_beta = sin_theta_exitant / fabs(cos_theta_exitant);
    }

  return m_reflectance * (INV_PI * (m_A + m_B * max_cos * sin_alpha * tan_beta));
  }