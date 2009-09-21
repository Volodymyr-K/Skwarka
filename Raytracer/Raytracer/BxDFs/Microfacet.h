#ifndef MICROFACET_H
#define MICROFACET_H

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/Core/Fresnel.h>

template<typename Fresnel, typename MicrofacetDistribution>
class Microfacet: public BxDF
  {
  public:
    Microfacet(Spectrum_d i_reflectance, const Fresnel &i_fresnel, const MicrofacetDistribution &i_distribution);

    virtual Spectrum_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    virtual Spectrum_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const;

    virtual double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

  private:
    double G(const Vector3D_d &i_incident, const Vector3D_d &i_exitant, const Vector3D_d &i_half_angle) const;

  private:
    Spectrum_d m_reflectance;
    Fresnel m_fresnel;
    MicrofacetDistribution m_distribution;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Fresnel, typename MicrofacetDistribution>
Microfacet<Fresnel,MicrofacetDistribution>::Microfacet(Spectrum_d i_reflectance, const Fresnel &i_fresnel,
                                               const MicrofacetDistribution &i_distribution):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)), m_reflectance(i_reflectance), m_fresnel(i_fresnel), m_distribution(i_distribution)
  {
  }

template<typename Fresnel, typename MicrofacetDistribution>
Spectrum_d Microfacet<Fresnel,MicrofacetDistribution>::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  double cos_theta_incident = fabs(i_incident[2]);
  double cos_theta_exitant = fabs(i_exitant[2]);

  Vector3D_d half_angle(i_incident + i_exitant);
  half_angle.Normalize();

  double cosine_half_angle = i_exitant*half_angle;
  double fresnel = m_fresnel(i_exitant*half_angle);

  return m_reflectance * m_distribution.Evaluate(half_angle) * G(i_incident, i_exitant, half_angle) * fresnel /
    (4.0 * cos_theta_exitant * cos_theta_exitant);
  }

template<typename Fresnel, typename MicrofacetDistribution>
Spectrum_d Microfacet<Fresnel,MicrofacetDistribution>::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant,
                                                      const Point2D_d &i_sample, double &o_pdf) const
  {
  m_distribution.Sample(i_incident, o_exitant, i_sample, o_pdf);
  if (i_incident[2]*o_exitant[2]<0.0)
    return Spectrum_d(0.0);
  return this->Evaluate(i_incident, o_exitant);
  }

template<typename Fresnel, typename MicrofacetDistribution>
double Microfacet<Fresnel,MicrofacetDistribution>::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  if (i_incident[2]*i_exitant[2]<0.0)
    return 0.0;
  return m_distribution.PDF(i_incident,i_exitant);
  }

template<typename Fresnel, typename MicrofacetDistribution>
double Microfacet<Fresnel,MicrofacetDistribution>::G(const Vector3D_d &i_incident,const Vector3D_d &i_exitant,
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