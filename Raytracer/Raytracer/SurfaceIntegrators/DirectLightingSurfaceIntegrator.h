#ifndef DIRECT_LIGHTING_SURFACE_INTEGRATOR_H
#define DIRECT_LIGHTING_SURFACE_INTEGRATOR_H

#include <Common/Common.h>
#include <Raytracer/Core/SurfaceIntegrator.h>
#include <Raytracer/Core/Renderer.h>
#include <Raytracer/Core/DirectLightingIntegrator.h>

/**
* SurfaceIntegrator implementation that accounts for direct light only, i.e. for the light coming from all light sources
* that are not occluded by any object in the scene (except for volume regions).
* The class uses DirectLightingIntegrator class to compute the direct lighting and traces rays for specular reflection and specular refraction.
* The class uses Russian Roulette to stop tracing specular rays once the minimum specular depth is achieved.
*/
class DirectLightingSurfaceIntegrator: public SurfaceIntegrator
  {
  public:
    /**
    * Creates DirectLightingSurfaceIntegrator instance.
    * @param ip_renderer Renderer to be used for computing radiance from the specularly reflected and transmitted rays. Should not be null.
    * @param ip_direct_lighting_integrator DirectLightingIntegrator for computing direct illumination. Should not be null.
    * @param i_min_specular_depth Minimum number of specular reflections and specular refractions computed before the Russian Roulette is used.
    * @param i_stop_probability Stopping probability for the Russian Roulette. Should be in (0;1] range.
    */
    DirectLightingSurfaceIntegrator(intrusive_ptr<const Renderer> ip_renderer, intrusive_ptr<DirectLightingIntegrator> ip_direct_lighting_integrator,
      size_t i_min_specular_depth = 5, double i_stop_probability = 0.1);

    /**
    * Computes radiance along the specified ray. The method will only be called when the ray does intersect some primitive in the scene.
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param i_intersection Intersection of the specified ray with the nearest primitive in the scene.
    * @param ip_sample Sample instance containing requested samples sequences. Can be null.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @return Resulting radiance value.
    */
    virtual Spectrum_d Radiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const;

    /**
    * Requests 1D and 2D samples sequences needed for the surface integrator.
    * This implementation just redirects the call to the DirectLightingIntegrator::RequestSamples() method.
    * This method should be called before the Radiance() method is called.
    */
    virtual void RequestSamples(intrusive_ptr<Sampler> ip_sampler);

  private:
    intrusive_ptr<DirectLightingIntegrator> mp_direct_lighting_integrator;

    size_t m_min_specular_depth;
    double m_stop_probability;
  };

#endif // DIRECT_LIGHTING_SURFACE_INTEGRATOR_H