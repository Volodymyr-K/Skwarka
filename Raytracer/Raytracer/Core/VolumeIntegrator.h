#ifndef VOLUME_INTEGRATOR_H
#define VOLUME_INTEGRATOR_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "Spectrum.h"
#include "Sample.h"
#include "Sampler.h"

/**
* An abstract class defining the contract for volume integrator implementations.
* A volume integrator is an algorithm for numerical evaluation of the light transport equation taking into account only volume scattering (i.e. no primitives).
* @sa SurfaceIntegrator
*/
class VolumeIntegrator: public ReferenceCounted
  {
  public:
    /**
    * Computes radiance along the specified ray.
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param ip_sample Sample instance containing requested samples sequences.
    * Can be null, in this case this is up to the implementation how to generate sample values.
    * @param[out] o_transmittance Media transmittance for the specified ray. Each spectrum component will be in [0;1] range.
    * @return Resulting radiance value.
    */
    virtual Spectrum_d Radiance(const RayDifferential &i_ray, const Sample *ip_sample, Spectrum_d &o_transmittance) const = 0;

    /**
    * Computes media transmittance for the specified ray. 
    * This method accounts for the media transmittance only and does not account for primitives intersected by the ray.
    * @param i_ray Ray for which the transmittance is to be computed. The direction component of the ray should be normalized.
    * @param ip_sample Sample instance containing requested samples sequences.
    * Can be null, in this case this is up to the implementation how to generate sample values.
    * @return Resulting transmittance value. Each spectrum component will be in [0;1] range.
    */
    virtual Spectrum_d Transmittance(const Ray &i_ray, const Sample *ip_sample) const = 0;

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

inline void VolumeIntegrator::RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  }

#endif // VOLUME_INTEGRATOR_H