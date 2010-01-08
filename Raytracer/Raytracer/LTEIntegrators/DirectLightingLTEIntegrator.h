#ifndef DIRECT_LIGHTING_LTE_INTEGRATOR_H
#define DIRECT_LIGHTING_LTE_INTEGRATOR_H

#include <Common/Common.h>
#include <Raytracer/Core/LTEIntegrator.h>
#include <Raytracer/Core/DirectLightingIntegrator.h>

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
    * @param ip_direct_lighting_integrator DirectLightingIntegrator for computing direct illumination. Should not be NULL.
    * @param i_max_specular_depth Maximum specular depth.
    */
    DirectLightingLTEIntegrator(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<VolumeIntegrator> ip_volume_integrator,
      intrusive_ptr<DirectLightingIntegrator> ip_direct_lighting_integrator, size_t i_max_specular_depth = 6);

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

    size_t m_max_specular_depth;
  };

#endif // DIRECT_LIGHTING_LTE_INTEGRATOR_H