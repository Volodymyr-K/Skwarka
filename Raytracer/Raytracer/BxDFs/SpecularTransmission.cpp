#include "SpecularTransmission.h"
#include <Math/SamplingRoutines.h>
#include <cmath>

SpecularTransmission::SpecularTransmission(Spectrum_d i_transmittance, double i_refractive_index_inner, double i_refractive_index_outer):
BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)), m_transmittance(i_transmittance),
m_refractive_index_inner(i_refractive_index_inner), m_refractive_index_outer(i_refractive_index_outer),
m_fresnel(i_refractive_index_inner, i_refractive_index_outer)
  {
  ASSERT(i_transmittance[0]>=0.0 && i_transmittance[0]<=1.0);
  ASSERT(i_transmittance[1]>=0.0 && i_transmittance[1]<=1.0);
  ASSERT(i_transmittance[2]>=0.0 && i_transmittance[2]<=1.0);

  ASSERT(i_refractive_index_inner>0.0 && i_refractive_index_outer>0.0);
  }

Spectrum_d SpecularTransmission::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  return Spectrum_d(0.0);
  }

Spectrum_d SpecularTransmission::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
  {
  ASSERT(i_sample[0]>=0.0 && i_sample[0]<=1.0);
  ASSERT(i_sample[1]>=0.0 && i_sample[1]<=1.0);
  ASSERT(i_incident.IsNormalized());

  o_pdf = 1.0;

  double Z_sign = -1.0;
  double refractive_index_inner(m_refractive_index_inner), refractive_index_outer(m_refractive_index_outer);
  if (i_incident[2]<0.0)
    {
    Z_sign=1.0;
    std::swap(refractive_index_inner, refractive_index_outer);
    }

  double eta = refractive_index_outer/refractive_index_inner;
  double sin_theta_incident_sqr = 1.0-i_incident[2]*i_incident[2];
  double sin_theta_exitant_sqr = eta*eta*sin_theta_incident_sqr;
  ASSERT(sin_theta_exitant_sqr>=0.0);

  // Handle total internal reflection for transmission.
  if (sin_theta_exitant_sqr >= 1.0)
    {
    o_exitant=Vector3D_d(-i_incident[0],-i_incident[1],i_incident[2]);
    return Spectrum_d(0.0);
    }

  double cos_theta_exitant = sqrt(1.0-sin_theta_exitant_sqr);

  o_exitant=Vector3D_d(-i_incident[0]*eta, -i_incident[1]*eta, cos_theta_exitant*Z_sign);

  double fresnel = m_fresnel(i_incident[2]);
  ASSERT(fresnel>=0.0 && fresnel<=1.0);

  return (refractive_index_inner*refractive_index_inner)/(refractive_index_outer*refractive_index_outer) * (1.0-fresnel) * m_transmittance / cos_theta_exitant;
  }

double SpecularTransmission::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  return 0.0;
  }

Spectrum_d SpecularTransmission::TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const
  {
  ASSERT(i_incident.IsNormalized());

  double eta = i_incident[2]>=0.0 ? m_refractive_index_outer/m_refractive_index_inner : m_refractive_index_inner/m_refractive_index_outer;

  double sin_theta_incident_sqr = 1.0-i_incident[2]*i_incident[2];
  double sin_theta_exitant_sqr = eta*eta*sin_theta_incident_sqr;
  ASSERT(sin_theta_exitant_sqr>=0.0);

  // Handle total internal reflection for transmission.
  if (sin_theta_exitant_sqr >= 1.0)
    return Spectrum_d(0.0);

  double fresnel = m_fresnel(i_incident[2]);
  ASSERT(fresnel>=0.0 && fresnel<=1.0);

  return (1.0-fresnel) * m_transmittance;
  }

Spectrum_d SpecularTransmission::TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples) const
  {
  // Here we don't really need two samples for one integral's sample since specular transmission defines the reflected direction uniquely.
  size_t num_samples = std::distance(i_samples.m_begin, i_samples.m_end);
  ASSERT(num_samples > 0);

  double eta, Z_sign;
  if (i_hemisphere)
    {
    eta=m_refractive_index_outer/m_refractive_index_inner;
    Z_sign=1.0;
    }
  else
    {
    eta=m_refractive_index_inner/m_refractive_index_outer;
    Z_sign=-1.0;
    }

  double eta_sqr = eta*eta;

  double sum=0.0;
  SamplesSequence2D::IteratorType it=i_samples.m_begin;
  for(size_t i=0;i<num_samples;++i)
    {
    Point2D_d sample_incident = *(it++);

    Vector3D_d incident = SamplingRoutines::UniformHemisphereSampling(sample_incident);
    ASSERT(incident.IsNormalized());

    double sin_theta_incident_sqr = 1.0-incident[2]*incident[2];
    double sin_theta_exitant_sqr = eta_sqr*sin_theta_incident_sqr;
    ASSERT(sin_theta_exitant_sqr>=0.0);

    // Handle total internal reflection for transmission.
    if (sin_theta_exitant_sqr >= 1.0)
      continue;

    double fresnel = m_fresnel(incident[2]*Z_sign);
    ASSERT(fresnel>=0.0 && fresnel<=1.0);

    sum += (1.0-fresnel)*incident[2];
    }

  Spectrum_d ret = m_transmittance*(2.0*sum/num_samples);

  // Clamp spectrum values because a surface can not physically scatter more light than it received.
  ret[0]=MathRoutines::Clamp(ret[0],0.0,1.0);
  ret[1]=MathRoutines::Clamp(ret[1],0.0,1.0);
  ret[2]=MathRoutines::Clamp(ret[2],0.0,1.0);
  return ret;
  }