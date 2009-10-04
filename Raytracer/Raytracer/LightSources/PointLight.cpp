#include "PointLight.h"
#include <Math/Constants.h>
#include <Math/SamplingRoutines.h>

PointLight::PointLight(const Point3D_d &i_position, const Spectrum_d &i_intensity): DeltaLightSource(),
m_position(i_position), m_intensity(i_intensity)
  {
  ASSERT(InRange(i_intensity,0.0,DBL_INF));
  }

void PointLight::Initialize(intrusive_ptr<Scene> ip_scene)
  {
  // Nothing to do for a point light.
  }

Spectrum_d PointLight::Power() const
  {
  return 4.0*M_PI*m_intensity;
  }

Spectrum_d PointLight::Lighting(const Point3D_d &i_point, Ray &o_lighting_ray) const
  {
  o_lighting_ray.m_origin = i_point;
  o_lighting_ray.m_direction = Vector3D_d(m_position-i_point);
  o_lighting_ray.m_min_t = 0.0;
  o_lighting_ray.m_max_t = 1.0;

  return m_intensity / o_lighting_ray.m_direction.LengthSqr();
  }

Spectrum_d PointLight::SamplePhoton(const Point2D_d &i_sample, Ray &o_photon_ray, double &o_pdf) const
  {
  o_photon_ray.m_origin = m_position;
  o_photon_ray.m_direction = SamplingRoutines::UniformSphereSampling(i_sample);
  o_photon_ray.m_min_t = 0.0;
  o_photon_ray.m_max_t = DBL_INF;

  o_pdf = SamplingRoutines::UniformSpherePDF();

  return m_intensity;
  }