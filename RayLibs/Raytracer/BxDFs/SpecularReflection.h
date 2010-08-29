#ifndef SPECULAR_REFLECTION_H
#define SPECULAR_REFLECTION_H

#include <Raytracer/Core/BxDF.h>
#include <Math/SamplingRoutines.h>

/**
* Specular reflective BxDF implementation.
* The BxDF reflects light along a single direction and therefore is defined by a singular delta function.
* The template parameter is a Fresnel class that is used to get the surface reflectance value. It should define "double operator(double) const" method.
* @sa SpecularTransmission
*/
template<typename Fresnel>
class SpecularReflection: public BxDF
  {
  public:
    /**
    * Creates SpecularReflection instance with the specified reflectance and Fresnel instance.
    * @param i_reflectance The reflectance(color) of the surface. Each spectrum component should be in [0;1] range.
    * @param i_fresnel Fresnel implementation.
    */
    SpecularReflection(SpectrumCoef_d i_reflectance, const Fresnel &i_fresnel);

    /**
    * Returns BxDF value for the specified incident and exitant directions.
    * The specular implementation always returns black spectrum.
    */
    virtual SpectrumCoef_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Samples BxDF value for the specified incident direction.
    * The sampled exitant direction will be in the same hemisphere that the incident direction is in.
    * @param i_incident Incident direction. Should be normalized.
    * @param[out] o_exitant Exitant direction. The returned value should be normalized.
    * @param i_sample 2D sample. Should be in [0;1]x[0;1] range.
    * @param[out] o_pdf PDF value for the sampled exitant direction. Always equal to 1.0.
    * @return Sampled BxDF value.
    */
    virtual SpectrumCoef_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const;

    /**
    * Returns PDF value for the specified incident and exitant direction.
    * This implementation always returns value 0.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_exitant Exitant direction. Should be normalized.
    * @return PDF value. Greater or equal than zero.
    */
    virtual double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Returns total scattering (i.e. fraction of scattered light) assuming a unit of light coming from the specified incident direction.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_samples 2D Samples sequence to be used for sampling the hemisphere. Should have at least one sample.
    * @return Total scattering value. Each spectrum component will be in [0;1] range.
    */
    virtual SpectrumCoef_d TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const;

    /**
    * Returns total scattering (i.e. fraction of scattered light) assuming a light coming uniformly from the specified hemisphere.
    * The method takes two 2D samples sequences that must have the same number of elements.
    * The implementation uses Monte Carlo integration to estimate the total scattering value.
    * @param i_hemisphere Defines the hemisphere of the incoming light.
    * Value true corresponds to the hemisphere above XY plane (i.e. with positive Z coordinate) and
    * value false corresponds to the hemisphere below XY plane (i.e. with negative Z coordinate).
    * @param i_samples1 First samples sequence. Should have the same number of elements that i_samples2 has.
    * @param i_samples2 Second samples sequence. Should have the same number of elements that i_samples1 has.
    * @return Total scattering value. Each spectrum component will be in [0;1] range.
    */
    virtual SpectrumCoef_d TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples1, SamplesSequence2D i_samples2) const;

  private:
    SpectrumCoef_d m_reflectance;
    Fresnel m_fresnel;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Fresnel>
SpecularReflection<Fresnel>::SpecularReflection(SpectrumCoef_d i_reflectance, const Fresnel &i_fresnel):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), m_reflectance(i_reflectance), m_fresnel(i_fresnel)
  {
  ASSERT(InRange(i_reflectance,0.0,1.0));
  }

template<typename Fresnel>
SpectrumCoef_d SpecularReflection<Fresnel>::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  return SpectrumCoef_d(0.0);
  }

template<typename Fresnel>
SpectrumCoef_d SpecularReflection<Fresnel>::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
  {
  ASSERT(i_sample[0]>=0.0 && i_sample[0]<=1.0);
  ASSERT(i_sample[1]>=0.0 && i_sample[1]<=1.0);
  ASSERT(i_incident.IsNormalized());

  o_exitant=Vector3D_d(-i_incident[0],-i_incident[1],i_incident[2]);
  o_pdf=1.0;

  SpectrumCoef_d fresnel = m_fresnel(i_incident[2]);
  ASSERT(InRange(fresnel,0.0,1.0));

  return m_reflectance * fresnel;
  }

template<typename Fresnel>
double SpecularReflection<Fresnel>::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  return 0.0;
  }

template<typename Fresnel>
SpectrumCoef_d SpecularReflection<Fresnel>::TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const
  {
  ASSERT(i_incident.IsNormalized());

  SpectrumCoef_d fresnel = m_fresnel(i_incident[2]);
  ASSERT(InRange(fresnel,0.0,1.0));

  return m_reflectance * fresnel;
  }

template<typename Fresnel>
SpectrumCoef_d SpecularReflection<Fresnel>::TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples1, SamplesSequence2D i_samples2) const
  {
  // Here we don't really need two samples for one integral's sample since specular reflection defines the reflected direction uniquely.
  size_t num_samples = std::distance(i_samples1.m_begin, i_samples1.m_end);
  ASSERT(num_samples > 0);

  double Z_sign = i_hemisphere ? 1.0 : -1.0;

  SpectrumCoef_d ret;
  SamplesSequence2D::Iterator it=i_samples1.m_begin;
  for(size_t i=0;i<num_samples;++i)
    {
    Point2D_d sample_incident = *(it++);

    Vector3D_d incident = SamplingRoutines::UniformHemisphereSampling(sample_incident);
    ASSERT(incident.IsNormalized() && incident[2]>=0.0);

    SpectrumCoef_d fresnel = m_fresnel(incident[2]*Z_sign);
    ASSERT(InRange(fresnel,0.0,1.0));

    ret.AddWeighted(fresnel, incident[2]);
    }

  ret *= m_reflectance*(2.0/num_samples);

  // Clamp spectrum values because a surface can not physically scatter more light than it received.
  ret.Clamp(0.0,1.0);
  return ret;
  }

#endif // SPECULAR_REFLECTION_H