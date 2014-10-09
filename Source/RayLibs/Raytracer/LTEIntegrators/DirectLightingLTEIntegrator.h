#ifndef DIRECT_LIGHTING_LTE_INTEGRATOR_H
#define DIRECT_LIGHTING_LTE_INTEGRATOR_H

#include <Common/Common.h>
#include <Raytracer/Core/LTEIntegrator.h>
#include <Raytracer/Core/DirectLightingIntegrator.h>

/**
* DTO for parameters for DirectLightingLTEIntegrator.
* @sa DirectLightingLTEIntegrator
*/
struct DirectLightingLTEIntegratorParams
  {
  /**
  * Number of samples for direct lighting estimation.
  * The actual number of shadow rays traced will be twice the number because we sample both BSDF and light sources.
  */
  size_t m_direct_light_samples_num;

  /**
  * Maximum specular depth for specular reflections and refractions.
  */
  size_t m_max_specular_depth;

  /**
  * Step size to be used for participating media integration. Should be greater than 0.0
  */
  double m_media_step_size;
  };

/**
* LTEIntegrator implementation that accounts for direct lighting only, i.e. for the light coming from all light sources
* that are not occluded by any object in the scene (except for volume regions for which the transmittance is computed).
* For participating media only a single scattering is computed (which is similar to direct lighting for surfaces).
* The class uses DirectLightingIntegrator class to compute the direct lighting and traces rays for specular reflection and specular refraction.
*/
class DirectLightingLTEIntegrator: public LTEIntegrator
  {
  public:
    /**
    * Creates DirectLightingLTEIntegrator instance.
    * @param ip_scene Scene instance. Should not be NULL.
    * @param i_params Integrator parameters.
    */
    DirectLightingLTEIntegrator(intrusive_ptr<const Scene> ip_scene, DirectLightingLTEIntegratorParams i_params);

  private:
    /**
    * Requests 1D and 2D samples sequences needed for the surface part of the LTE integration.
    * This implementation calls the DirectLightingIntegrator::RequestSamples() method and requests samples for media integration.
    */
    virtual void _RequestSamples(intrusive_ptr<Sampler> ip_sampler);

    /**
    * Computes surface radiance along the specified ray.
    * The method will only be called when the ray does intersect some primitive in the scene.
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param i_intersection Intersection of the specified ray with the nearest primitive in the scene.
    * @param ip_sample Sample instance containing requested samples sequences. Can be null.
    * @param i_ts Thread specifics (memory pool, random number generator etc.).
    * @return Resulting radiance value.
    */
    virtual Spectrum_d _SurfaceRadiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, ThreadSpecifics i_ts) const;

    /**
    * Computes media radiance and transmittance for the specified ray. Only single scattering is computed.
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param ip_sample Sample instance containing requested samples sequences.
    * @param o_transmittance Resulting transmittance value. All spectrum components will be in [0;1] range.
    * @param i_ts Thread specifics (memory pool, random number generator etc.).
    * @return Resulting radiance value.
    */
    virtual Spectrum_d _MediaRadianceAndTranmsittance(const RayDifferential &i_ray, const Sample *ip_sample, SpectrumCoef_d &o_transmittance, ThreadSpecifics i_ts) const;

    /**
    * Helper private method that computes media transmittance for the specified ray.
    */
    SpectrumCoef_d _MediaTransmittance(const Ray &i_ray, ThreadSpecifics i_ts) const;

  private:
    intrusive_ptr<const Scene> mp_scene;

    intrusive_ptr<DirectLightingIntegrator> mp_direct_lighting_integrator;

    DirectLightingLTEIntegratorParams m_params;

    // IDs of samples sequences used for media integration.
    size_t m_media_offset1_id, m_media_offset2_id;
  };

#endif // DIRECT_LIGHTING_LTE_INTEGRATOR_H