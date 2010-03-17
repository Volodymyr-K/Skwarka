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
  };

/**
* LTEIntegrator implementation that accounts for direct lighting only, i.e. for the light coming from all light sources
* that are not occluded by any object in the scene (except for volume regions).
* The class uses DirectLightingIntegrator class to compute the direct lighting and traces rays for specular reflection and specular refraction.
*/
class DirectLightingLTEIntegrator: public LTEIntegrator
  {
  public:
    /**
    * Creates DirectLightingLTEIntegrator instance.
    * @param ip_scene Scene instance. Should not be NULL.
    * @param ip_volume_integrator Volume integrator for computing volume radiance and transmittance. Can be NULL in which case it is assumed there is no participating media.
    * @param i_params Integrator parameters.
    */
    DirectLightingLTEIntegrator(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<VolumeIntegrator> ip_volume_integrator, DirectLightingLTEIntegratorParams i_params);

  private:
    /**
    * Requests 1D and 2D samples sequences needed for the surface part of the LTE integration.
    * This implementation just redirects the call to the DirectLightingIntegrator::RequestSamples() method.
    */
    virtual void _RequestSamples(intrusive_ptr<Sampler> ip_sampler);

    /**
    * Computes surface radiance along the specified ray.
    * The method will only be called when the ray does intersect some primitive in the scene.
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param i_intersection Intersection of the specified ray with the nearest primitive in the scene.
    * @param ip_sample Sample instance containing requested samples sequences. Can be null.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @return Resulting radiance value.
    */
    virtual Spectrum_d _SurfaceRadiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const;

  private:
    intrusive_ptr<DirectLightingIntegrator> mp_direct_lighting_integrator;

    DirectLightingLTEIntegratorParams m_params;
  };

#endif // DIRECT_LIGHTING_LTE_INTEGRATOR_H