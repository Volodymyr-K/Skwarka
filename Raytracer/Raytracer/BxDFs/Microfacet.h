#ifndef MICROFACET_H
#define MICROFACET_H

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/Core/Fresnel.h>

template<typename MicrofacetDistribution>
class Microfacet: public BxDF
  {
  public:
    Microfacet(Spectrum_d i_reflectance, const MicrofacetDistribution &i_distribution, double i_refractive_index, double i_absorption);

    virtual Spectrum_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    virtual Spectrum_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const;

    virtual double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

  private:
    double G(const Vector3D_d &i_incident, const Vector3D_d &i_exitant, const Vector3D_d &i_half_angle) const;

  private:
    Spectrum_d m_reflectance;
    MicrofacetDistribution m_distribution;
    double m_refractive_index, m_absorption;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename MicrofacetDistribution>
Microfacet<MicrofacetDistribution>::Microfacet(Spectrum_d i_reflectance, const MicrofacetDistribution &i_distribution,
                                               double i_refractive_index, double i_absorption):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)), m_reflectance(i_reflectance), m_distribution(i_distribution),
m_refractive_index(i_refractive_index), m_absorption(i_absorption)
  {
  }

template<typename MicrofacetDistribution>
Spectrum_d Microfacet<MicrofacetDistribution>::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  double cos_theta_incident = fabs(i_incident[2]);
  double cos_theta_exitant = fabs(i_exitant[2]);

  Vector3D_d half_angle(i_incident + i_exitant);
  half_angle.Normalize();

  double cosine_half_angle = i_exitant*half_angle;
  double fresnel = FresnelConductorReflectance(m_refractive_index, m_absorption, i_exitant*half_angle);

  return m_reflectance * m_distribution.Evaluate(half_angle) * G(i_incident, i_exitant, half_angle) * fresnel /
    (4.0 * cos_theta_exitant * cos_theta_exitant);
  }

template<typename MicrofacetDistribution>
Spectrum_d Microfacet<MicrofacetDistribution>::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant,
                                                      const Point2D_d &i_sample, double &o_pdf) const
  {
  m_distribution.Sample(i_incident, o_exitant, i_sample, o_pdf);
  if (i_incident[2]*o_exitant[2]<0.0)
    return Spectrum_d(0.0);
  return this->Evaluate(i_incident, o_exitant);
  }

template<typename MicrofacetDistribution>
double Microfacet<MicrofacetDistribution>::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  if (i_incident[2]*i_exitant[2]<0.0)
    return 0.0;
  return m_distribution.PDF(i_incident,i_exitant);
  }

template<typename MicrofacetDistribution>
double Microfacet<MicrofacetDistribution>::G(const Vector3D_d &i_incident,const Vector3D_d &i_exitant,
                                             const Vector3D_d &i_half_angle) const
  {
  double cos_theta_half_angle = fabs(i_half_angle[2]);
  double cos_theta_incident = fabs(i_incident[2]);
  double cos_theta_exitant = fabs(i_exitant[2]);
  double incident_dot_half_angle = i_incident*i_half_angle;
  return std::min(1.0,std::min(
    (2.0 * cos_theta_half_angle * cos_theta_incident / incident_dot_half_angle),
    (2.0 * cos_theta_half_angle * cos_theta_exitant / incident_dot_half_angle))
    );
  }

#endif // MICROFACET_H