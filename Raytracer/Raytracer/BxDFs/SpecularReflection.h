#ifndef SPECULAR_REFLECTION_H
#define SPECULAR_REFLECTION_H

#include <Raytracer/Core/BxDF.h>

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
    SpecularReflection(Spectrum_d i_reflectance, const Fresnel &i_fresnel);

    /**
    * Returns BxDF value for the specified incident and exitant directions.
    * The specular implementation always returns black spectrum.
    */
    virtual Spectrum_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Samples BxDF value for the specified incident direction.
    * The sampled exitant direction will be in the same hemisphere that the incident direction is in.
    * @param i_incident Incident direction. Should be normalized.
    * @param[out] o_exitant Exitant direction. The returned value should be normalized.
    * @param i_sample 2D sample. Should be in [0;1]^2 range.
    * @param[out] o_pdf PDF value for the sampled exitant direction. Always equal to 1.0.
    * @return Sampled BxDF value.
    */
    virtual Spectrum_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const;

    /**
    * Returns PDF value for the specified incident and exitant direction.
    * This implementation always returns value 0.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_exitant Exitant direction. Should be normalized.
    * @return PDF value. Greater or equal than zero.
    */
    virtual double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Returns total hemisphere scattering assuming a unit of light coming from the specified incident direction.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_samples 2D Samples sequence to be used for sampling the hemisphere. Should have at least one sample.
    * @return Total scattering value. Each spectrum component will be in [0;1] range.
    */
    virtual Spectrum_d TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const;

    /**
    * Returns total hemisphere scattering assuming a light coming uniformly from the entire hemisphere.
    * The implementation uses Monte Carlo integration to estimate the total scattering value.
    * @param i_samples 2D Samples sequence to be used for sampling the hemisphere. Should have at least one sample.
    * @return Total scattering value. Each spectrum component will be in [0;1] range.
    */
    virtual Spectrum_d TotalScattering(SamplesSequence2D i_samples) const;

  private:
    Spectrum_d m_reflectance;
    Fresnel m_fresnel;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Fresnel>
SpecularReflection<Fresnel>::SpecularReflection(Spectrum_d i_reflectance, const Fresnel &i_fresnel):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), m_reflectance(i_reflectance), m_fresnel(i_fresnel)
  {
  ASSERT(i_reflectance[0]>=0.0 && i_reflectance[0]<=1.0);
  ASSERT(i_reflectance[1]>=0.0 && i_reflectance[1]<=1.0);
  ASSERT(i_reflectance[2]>=0.0 && i_reflectance[2]<=1.0);
  }

template<typename Fresnel>
Spectrum_d SpecularReflection<Fresnel>::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  return Spectrum_d(0.0);
  }

template<typename Fresnel>
Spectrum_d SpecularReflection<Fresnel>::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
  {
  ASSERT(i_sample[0]>=0.0 && i_sample[0]<=1.0);
  ASSERT(i_sample[1]>=0.0 && i_sample[1]<=1.0);
  ASSERT(i_incident.IsNormalized());

  o_exitant=Vector3D_d(-i_incident[0],-i_incident[1],i_incident[2]);
  o_pdf=1.0;

  double fresnel = m_fresnel(i_incident[2]);
  ASSERT(fresnel>=0.0 && fresnel<=1.0);

  return m_reflectance * fresnel / fabs(i_incident[2]);
  }

template<typename Fresnel>
double SpecularReflection<Fresnel>::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  return 0.0;
  }

template<typename Fresnel>
Spectrum_d SpecularReflection<Fresnel>::TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const
  {
  ASSERT(i_incident.IsNormalized());

  double fresnel = m_fresnel(i_incident[2]);
  ASSERT(fresnel>=0.0 && fresnel<=1.0);

  return m_reflectance * fresnel;
  }

template<typename Fresnel>
Spectrum_d SpecularReflection<Fresnel>::TotalScattering(SamplesSequence2D i_samples) const
  {
  // Here we don't really need two samples for one integral's sample since specular reflection defines the reflected direction uniquely.
  size_t num_samples = std::distance(i_samples.m_begin, i_samples.m_end);
  ASSERT(num_samples > 0);

  double sum=0.0;
  SamplesSequence2D::IteratorType it=i_samples.m_begin;
  for(size_t i=0;i<num_samples;++i)
    {
    Point2D_d sample_incident = *(it++);

    Vector3D_d incident = SamplingRoutines::UniformHemisphereSampling(sample_incident);
    ASSERT(incident.IsNormalized() && incident[2]>=0.0);

    double fresnel = m_fresnel(incident[2]);
    ASSERT(fresnel>=0.0 && fresnel<=1.0);

    sum += fresnel*incident[2];
    }

  Spectrum_d ret=m_reflectance*(2.0*sum/num_samples);

  // Clamp spectrum values because a surface can not physically scatter more light than it received.
  ret[0]=MathRoutines::Clamp(ret[0],0.0,1.0);
  ret[1]=MathRoutines::Clamp(ret[1],0.0,1.0);
  ret[2]=MathRoutines::Clamp(ret[2],0.0,1.0);
  return ret;
  }

#endif // SPECULAR_REFLECTION_H