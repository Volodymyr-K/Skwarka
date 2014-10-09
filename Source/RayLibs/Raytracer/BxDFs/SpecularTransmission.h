#ifndef SPECULAR_TRANSMISSION_H
#define SPECULAR_TRANSMISSION_H

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/Core/Fresnel.h>

/**
* Specular transparent BxDF implementation.
* The BxDF transmits light along a single direction and therefore is defined by a singular delta function.
* the transmitted direction is defined by the Snell's law.
* @sa SpecularReflection
*/
class SpecularTransmission: public BxDF
  {
  public:
    /**
    * Creates SpecularTransmission instance with the specified reflectance and Fresnel instance.
    * @param i_transmittance The transmittance(color) of the surface. Each spectrum component should be in [0;1] range.
    * @param i_refractive_index_inner Refractive index of the object. Should be positive.
    * @param i_refractive_index_outer Refractive index of the medium. Should be positive.
    */
    SpecularTransmission(SpectrumCoef_d i_transmittance, double i_refractive_index_inner, double i_refractive_index_outer);

    /**
    * Returns BxDF value for the specified incident and exitant directions.
    * The specular implementation always returns black spectrum.
    */
    virtual SpectrumCoef_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Samples BxDF value for the specified incident direction.
    * The sampled exitant direction will be in the opposite hemisphere that the incident direction is in.
    * @param i_incident Incident direction. Should be normalized.
    * @param[out] o_exitant Exitant direction. The returned value should be normalized.
    * @param i_sample 2D sample. Should be in [0;1]x[0;1] range.
    * @param[out] o_pdf PDF value for the sampled exitant direction. Always equal to 1.0.
    * @return Sampled BxDF value. The value does not account for the radiance change due to the refractive index. The calling code must take care of that.
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
    SpectrumCoef_d m_transmittance;

    double m_refractive_index_inner, m_refractive_index_outer;
    FresnelDielectric m_fresnel;
  };

#endif // SPECULAR_TRANSMISSION_H