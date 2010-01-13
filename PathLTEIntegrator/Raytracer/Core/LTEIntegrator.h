#ifndef LTE_INTEGRATOR_H
#define LTE_INTEGRATOR_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include "Spectrum.h"
#include "Scene.h"
#include "Sample.h"
#include "Sampler.h"
#include "Intersection.h"
#include "VolumeIntegrator.h"

/**
* An abstract class defining the contract for Light Transport Equation (LTE) integrator implementations.
* An LTE integrator is an algorithm for numerical evaluation of the light transport equation taking into account both the surface scattering and volume scattering.
* @sa VolumeIntegrator
*/
class LTEIntegrator: public ReferenceCounted
  {
  public:
    /**
    * Computes radiance along the specified ray.
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param ip_sample Sample instance containing requested samples sequences. Can be NULL.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @return Resulting radiance value.
    */
    Spectrum_d Radiance(const RayDifferential &i_ray, const Sample *ip_sample, MemoryPool &i_pool) const;

    /**
    * Requests 1D and 2D samples sequences needed for the LTE integrator.
    * This method should be called before the Radiance() method is called.
    *
    * The method calls VolumeIntegrator::RequestSamples() method and private virtual _RequestSamples() method for surface integration samples.
    */
    void RequestSamples(intrusive_ptr<Sampler> ip_sampler);

    virtual ~LTEIntegrator();

  protected:
    /**
    * Creates LTEIntegrator instance with the specified scene and volume integrator.
    * @param ip_scene Scene instance. Should not be NULL.
    * @param ip_volume_integrator Volume integrator for computing volume radiance and transmittance. Can be NULL in which case it is assumed there is no participating media.
    */
    LTEIntegrator(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<VolumeIntegrator> ip_volume_integrator);

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

    /**
    * The helper method that computes volume radiance and transmittance by calling the volume integrator.
    */
    Spectrum_d _VolumeRadianceAndTranmsittance(const RayDifferential &i_ray, const Sample *ip_sample, Spectrum_d &o_transmittance) const;

    /**
    * The helper method that computes volume transmittance by calling the volume integrator.
    */
    Spectrum_d _VolumeTransmittance(const Ray &i_ray, const Sample *ip_sample) const;

  private:
    /**
    * Computes surface radiance along the specified ray.
    * The method will only be called when the ray does intersect some primitive in the scene.
    * This is a virtual private method that derived classes must implement. This method is called by non-virtual public Radiance() method (NVI pattern).
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param i_intersection Intersection of the specified ray with the nearest primitive in the scene.
    * @param ip_sample Sample instance containing requested samples sequences.
    * Can be NULL, in this case this is up to the implementation how to generate sample values.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @return Resulting radiance value.
    */
    virtual Spectrum_d _SurfaceRadiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const = 0;

    /**
    * Requests 1D and 2D samples sequences needed for the surface part of the LTE integration.
    * This is a virtual private method that derived classes can implement. This method is called by non-virtual public RequestSamples() method (NVI pattern).
    * Default implementation does nothing.
    */
    virtual void _RequestSamples(intrusive_ptr<Sampler> ip_sampler);

  private:
    // Not implemented, not a value type.
    LTEIntegrator(const LTEIntegrator&);
    LTEIntegrator &operator=(const LTEIntegrator&);

  private:
    intrusive_ptr<const Scene> mp_scene;
    intrusive_ptr<VolumeIntegrator> mp_volume_integrator;
  };

#endif // LTE_INTEGRATOR_H