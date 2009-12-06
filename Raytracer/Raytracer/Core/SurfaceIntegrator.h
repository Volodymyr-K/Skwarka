#ifndef SURFACE_INTEGRATOR_H
#define SURFACE_INTEGRATOR_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include "Spectrum.h"
#include "Sample.h"
#include "Sampler.h"
#include "Intersection.h"
#include "Renderer.h"

/**
* An abstract class defining the contract for surface integrator implementations.
* A surface integrator is an algorithm for numerical evaluation of the light transport equation taking into account only surface scattering (i.e. no participating media).
* @sa VolumeIntegrator
*/
class SurfaceIntegrator: public ReferenceCounted
  {
  public:
    /**
    * Computes radiance along the specified ray. The method will only be called when the ray does intersect some primitive in the scene.
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param i_intersection Intersection of the specified ray with the nearest primitive in the scene.
    * @param ip_sample Sample instance containing requested samples sequences.
    * Can be null, in this case this is up to the implementation how to generate sample values.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @return Resulting radiance value.
    */
    virtual Spectrum_d Radiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const = 0;

    /**
    * Requests 1D and 2D samples sequences needed for the surface integrator.
    * This method should be called before the Radiance() method is called.
    * Default implementation does nothing.
    */
    virtual void RequestSamples(intrusive_ptr<Sampler> ip_sampler);

    virtual ~SurfaceIntegrator();

  protected:
    /**
    * Creates SurfaceIntegrator instance with the specified renderer.
    */
    SurfaceIntegrator(intrusive_ptr<const Renderer> ip_renderer);

    /**
    * The method for derived classes to compute radiance for the specularly reflected ray.
    * The method calls Renderer::Radiance() method to compute the radiance (which can result in recursive call depending on the renderer implementation).
    */
    Spectrum_d _SpecularReflect(const RayDifferential &i_ray, const Intersection &i_intersection, const BSDF *ip_bsdf, const Sample *ip_sample, MemoryPool &i_pool) const;

    /**
    * The method for derived classes to compute radiance for the specularly transmitted ray.
    * The method calls Renderer::Radiance() method to compute the radiance (which can result in recursive call depending on the renderer implementation).
    */
    Spectrum_d _SpecularTransmit(const RayDifferential &i_ray, const Intersection &i_intersection, const BSDF *ip_bsdf, const Sample *ip_sample, MemoryPool &i_pool) const;

  private:
    // Not implemented, not a value type.
    SurfaceIntegrator(const SurfaceIntegrator&);
    SurfaceIntegrator &operator=(const SurfaceIntegrator&);

  private:
    intrusive_ptr<const Renderer> mp_renderer;
  };

#endif // SURFACE_INTEGRATOR_H