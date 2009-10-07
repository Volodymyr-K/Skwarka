#ifndef SURFACE_INTEGRATOR_H
#define SURFACE_INTEGRATOR_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include "Spectrum.h"
#include "Sample.h"
#include "Sampler.h"
#include <utility>

/**
* An abstract class defining the contract for surface integrator implementations.
* A surface integrator is an algorithm for numerical evaluation of the light transport equation taking into account only surface interaction (i.e. no participating media).
* @sa VolumeIntegrator
*/
class SurfaceIntegrator: public ReferenceCounted
  {
  public:
    /**
    * Computes radiance along the the specified ray.
    * @param i_ray Ray for which the radiance is to be computed.
    * @param ip_sample Sample instance containing requested samples sequences.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @param[out] o_alpha Computed alpha value. If the ray hits an opaque primitive the value is set to 1.0.
    * If the ray does not hit any primitive and no infinite light contributes to it the alpha value is 0.0.
    * @return Resulting radiance value.
    */
    virtual Spectrum_d Radiance(const RayDifferential &i_ray, const Sample *ip_sample, MemoryPool &i_pool, double &o_alpha) const = 0;

    /**
    * Precomputes light distribution in the scene (e.g. tracing photons etc.).
    * This method will be called before the Radiance() method is called.
    * Default implementation does nothing.
    */
    virtual void Preprocess();

    /**
    * Requests 1D and 2D samples sequences needed for the surface integrator.
    * This method will be called before the Radiance() method is called.
    * Default implementation does nothing.
    */
    virtual void RequestSamples(intrusive_ptr<Sampler> ip_sampler);

    virtual ~SurfaceIntegrator();

  protected:
    SurfaceIntegrator();

  private:
    // Not implemented, not a value type.
    SurfaceIntegrator(const SurfaceIntegrator&);
    SurfaceIntegrator &operator=(const SurfaceIntegrator&);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline SurfaceIntegrator::SurfaceIntegrator()
  {
  }

inline SurfaceIntegrator::~SurfaceIntegrator()
  {
  }

inline void SurfaceIntegrator::Preprocess()
  {
  }

inline void SurfaceIntegrator::RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  }

#endif // SURFACE_INTEGRATOR_H