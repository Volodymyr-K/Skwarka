#ifndef VOLUME_INTEGRATOR_MOCK_H
#define VOLUME_INTEGRATOR_MOCK_H

#include <Common/Common.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Core/VolumeIntegrator.h>
#include <Raytracer/Core/Renderer.h>
#include <Raytracer/Core/Scene.h>
#include <cmath>

/*
VolumeIntegrator mock implementation.
Does not scatter or emit light, just absorb it.
*/
class VolumeIntegratorMock: public VolumeIntegrator
  {
  public:
    VolumeIntegratorMock();

    virtual Spectrum_d RadianceAndTransmittance(const RayDifferential &i_ray, const Sample *ip_sample, Spectrum_d &o_transmittance) const;

    virtual Spectrum_d Transmittance(const Ray &i_ray, const Sample *ip_sample) const;

  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline VolumeIntegratorMock::VolumeIntegratorMock(): VolumeIntegrator()
  {
  }

inline Spectrum_d VolumeIntegratorMock::RadianceAndTransmittance(const RayDifferential &i_ray, const Sample *ip_sample, Spectrum_d &o_transmittance) const
  {
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());
  double ray_length = i_ray.m_base_ray.m_direction.Length()*fabs(i_ray.m_base_ray.m_max_t-i_ray.m_base_ray.m_min_t);
  o_transmittance = Spectrum_d( exp(-ray_length) );
  return Spectrum_d(0.0);
  }

inline Spectrum_d VolumeIntegratorMock::Transmittance(const Ray &i_ray, const Sample *ip_sample) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());
  double ray_length = i_ray.m_direction.Length()*fabs(i_ray.m_max_t-i_ray.m_min_t);
  return Spectrum_d( exp(-ray_length) );
  }

#endif // VOLUME_INTEGRATOR_MOCK_H