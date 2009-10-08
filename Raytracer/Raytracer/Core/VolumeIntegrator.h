#ifndef VOLUME_INTEGRATOR_H
#define VOLUME_INTEGRATOR_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include "Spectrum.h"
#include "Sample.h"
#include "Sampler.h"
#include <utility>

/**
* An abstract class defining the contract for volume integrator implementations.
* A volume integrator is an algorithm for numerical evaluation of the light transport equation taking into account only volume scattering (i.e. no primitives).
* @sa SurfaceIntegrator, SceneIntegrator
*/
class VolumeIntegrator: public ReferenceCounted
  {
  public:
    /**
    * Computes radiance along the the specified ray.
    * @param i_ray Ray for which the radiance is to be computed.
    * @param ip_sample Sample instance containing requested samples sequences.
    * @return Resulting radiance value.
    */
    virtual Spectrum_d Radiance(const RayDifferential &i_ray, const Sample *ip_sample) const = 0;

    /**
    * Computes media transmittance for the specified ray.
    * This method accounts for the media transmittance only and does not account for primitives intersected by the ray.
    * @param i_ray Ray for which the transmittance is to be computed.
    * @param ip_sample Sample instance containing requested samples sequences.
    * @return Resulting transmittance value.
    */
    virtual Spectrum_d Transmittance(const Ray &i_ray, const Sample *ip_sample) const = 0;

    /**
    * Precomputes light distribution in the scene (e.g. tracing photons etc.).
    * This method should be called before the Radiance() method is called.
    * Default implementation does nothing.
    */
    virtual void Preprocess();

    /**
    * Requests 1D and 2D samples sequences needed for the volume integrator.
    * This method should be called before the Radiance() method is called.
    * Default implementation does nothing.
    */
    virtual void RequestSamples(intrusive_ptr<Sampler> ip_sampler);

    virtual ~VolumeIntegrator();

  protected:
    VolumeIntegrator();

  private:
    // Not implemented, not a value type.
    VolumeIntegrator(const VolumeIntegrator&);
    VolumeIntegrator &operator=(const VolumeIntegrator&);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline VolumeIntegrator::VolumeIntegrator()
  {
  }

inline VolumeIntegrator::~VolumeIntegrator()
  {
  }

inline void VolumeIntegrator::Preprocess()
  {
  }

inline void VolumeIntegrator::RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  }

#endif // VOLUME_INTEGRATOR_H