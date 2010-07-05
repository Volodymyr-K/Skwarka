#ifndef FRESNEL_BLEND_H
#define FRESNEL_BLEND_H

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/SamplingRoutines.h>

/**
* Glossy reflective BxDF implementation that models a diffuse underlying surface with a glossy specular surface above it.
* The effect of reflection from the diffuse surface is modulated by the amount of energy left after Fresnel effects have been considered.
* The method was initially developed by Ashikhmin and Shirley (2000,2002) and the model obeys reciprocity and conserves energy.
*
* The template parameter is the microfacet distribution implementation that defines the distribution of the facets normals for the glossy layer.
*/
template<typename MicrofacetDistribution>
class FresnelBlend: public BxDF
  {
  public:
    /**
    * Creates instance of FresnelBlend with the specified reflectances and microfacet distribution.
    * @param i_diffuse_reflectance The reflectance(color) of the diffuse surface. Each spectrum component should be in [0;1] range.
    * @param i_specular_reflectance The reflectance of the glossy layer at normal incidence. Each spectrum component should be in [0;1] range.
    * @param i_distribution MicrofacetDistribution implementation.
    */
    FresnelBlend(Spectrum_d i_diffuse_reflectance, Spectrum_d i_specular_reflectance, const MicrofacetDistribution &i_distribution);

    /**
    * Returns BxDF value for the specified incident and exitant directions.
    */
    virtual Spectrum_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Samples BxDF value for the specified incident direction.
    * The sampled exitant direction may be in any hemisphere, regardless of the incident direction.
    * The MicrofacetDistribution is used to sample the exitant direction for the glossy layer.
    * @param i_incident Incident direction. Should be normalized.
    * @param[out] o_exitant Exitant direction. The returned value should be normalized.
    * @param i_sample 2D sample. Should be in [0;1]^2 range.
    * @param[out] o_pdf PDF value for the sampled exitant direction. The returned value should be greater or equal than zero.
    * @return Sampled BxDF value.
    */
    virtual Spectrum_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const;

    /**
    * Returns PDF value for the specified incident and exitant direction.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_exitant Exitant direction. Should be normalized.
    * @return PDF value. Greater or equal than zero.
    */
    virtual double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

  private:
    /**
    * Returns approximation of the fresnel term.
    */
    Spectrum_d _SchlickFresnel(double i_cos_theta) const;

  private:
    Spectrum_d m_specular_reflectance, m_diffuse_reflectance;
    MicrofacetDistribution m_distribution;

    // Probabilities of sampling the diffuse and the glossy component.
    double m_diffuse_probability, m_specular_probability;
    double m_inv_diffuse_probability, m_inv_specular_probability;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Declare the lookup table with probabilities for sampling the diffuse part of the FresnelBlend.
// The array indices correspond to the diffuse and specular reflectance.
const size_t FresnelBlend_SamplesNum = 20;
extern const double FresnelBlend_DiffuseProbs[FresnelBlend_SamplesNum][FresnelBlend_SamplesNum];

template<typename MicrofacetDistribution>
FresnelBlend<MicrofacetDistribution>::FresnelBlend(Spectrum_d i_diffuse_reflectance, Spectrum_d i_specular_reflectance, const MicrofacetDistribution &i_distribution):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)), m_diffuse_reflectance(i_diffuse_reflectance), m_specular_reflectance(i_specular_reflectance), m_distribution(i_distribution)
  {
  ASSERT(InRange(i_diffuse_reflectance, 0.0, 1.0));
  ASSERT(InRange(i_specular_reflectance, 0.0, 1.0));

  // Lookup for the probabilities to sample diffuse and glossy parts of the FresnelBlend.
  size_t diffuse_index = std::min(size_t(i_diffuse_reflectance.Luminance()*FresnelBlend_SamplesNum), FresnelBlend_SamplesNum-1);
  size_t specular_index = std::min(size_t(i_specular_reflectance.Luminance()*FresnelBlend_SamplesNum), FresnelBlend_SamplesNum-1);

  m_diffuse_probability = FresnelBlend_DiffuseProbs[diffuse_index][specular_index];
  m_specular_probability = 1.0-m_diffuse_probability;
  ASSERT(m_diffuse_probability>=0.0 && m_diffuse_probability<=1.0);

  m_inv_diffuse_probability = m_diffuse_probability>0.0 ? 1.0/m_diffuse_probability : 0.0;
  m_inv_specular_probability = m_specular_probability>0.0 ? 1.0/m_specular_probability : 0.0;
  }

template<typename MicrofacetDistribution>
Spectrum_d FresnelBlend<MicrofacetDistribution>::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  double cos_theta_incident = fabs(i_incident[2]);
  double cos_theta_exitant = fabs(i_exitant[2]);

  Vector3D_d half_angle(i_incident + i_exitant);
  half_angle.Normalize();
  double cosine_half_angle = i_exitant*half_angle;

  Spectrum_d diffuse = (28.0/(23.0*M_PI)) * m_diffuse_reflectance * (Spectrum_d(1.0) - m_specular_reflectance) *
    (1.0 - pow(1.0 - 0.5*cos_theta_exitant, 5.0)) * (1.0 - pow(1.0 - 0.5*cos_theta_incident, 5.0));

  Spectrum_d specular = m_distribution.Evaluate(half_angle) / (4.0 * fabs(cosine_half_angle) * std::max(cos_theta_exitant, cos_theta_incident)) * _SchlickFresnel(cosine_half_angle);
  return diffuse + specular;
  }

template<typename MicrofacetDistribution>
Spectrum_d FresnelBlend<MicrofacetDistribution>::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant,
                                                              const Point2D_d &i_sample, double &o_pdf) const
  {
  ASSERT(i_sample[0]>=0.0 && i_sample[0]<=1.0);
  ASSERT(i_sample[1]>=0.0 && i_sample[1]<=1.0);
  ASSERT(i_incident.IsNormalized());

  Point2D_d sample = i_sample;

  // Choose which layer to sample.
  if (sample[0] < m_diffuse_probability)
    {
    // Rescale the sample.
    sample[0] = sample[0] * m_inv_diffuse_probability;

    // Cosine-sample the hemisphere, flipping the direction if necessary.
    o_exitant = SamplingRoutines::CosineHemisphereSampling(sample);
    if (i_incident[2] < 0.0) o_exitant[2] *= -1.0;

    o_pdf = m_diffuse_probability*fabs(o_exitant[2])*INV_PI + m_specular_probability*m_distribution.PDF(i_incident, o_exitant);
    }
  else
    {
    // Rescale the sample.
    sample[0] = (sample[0] - m_diffuse_probability) * m_inv_specular_probability;
    ASSERT(sample[0]>=0.0 && sample[0]<1.0);

    double microfacted_pdf;
    m_distribution.Sample(i_incident, o_exitant, sample, microfacted_pdf);
    o_pdf = m_diffuse_probability*fabs(o_exitant[2])*INV_PI + m_specular_probability*microfacted_pdf;

    // Microfacet distribution implementation can sample exitant vectors in a wrong hemisphere, so we just ignore those results.
    if (i_incident[2]*o_exitant[2]<0.0)
      {
      o_pdf = 0.0;
      return Spectrum_d(0.0);
      }
    }

  return this->Evaluate(i_incident, o_exitant);
  }

template<typename MicrofacetDistribution>
double FresnelBlend<MicrofacetDistribution>::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  if (i_incident[2]*i_exitant[2]<0.0)
    return 0.0;

  return m_diffuse_probability*fabs(i_exitant[2])*INV_PI + m_specular_probability*m_distribution.PDF(i_incident, i_exitant);
  }

template<typename MicrofacetDistribution>
Spectrum_d FresnelBlend<MicrofacetDistribution>::_SchlickFresnel(double i_cos_theta) const
  {
  ASSERT(i_cos_theta>=0.0 && i_cos_theta<=1.0);
  return m_specular_reflectance + pow(1.0-i_cos_theta, 5.0) * (Spectrum_d(1.0) - m_specular_reflectance);
  }

#endif // FRESNEL_BLEND_H