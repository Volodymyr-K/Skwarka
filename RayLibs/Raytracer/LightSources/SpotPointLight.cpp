#include "SpotPointLight.h"
#include <Math/Constants.h>
#include <Math/SamplingRoutines.h>
#include <Math/MathRoutines.h>
#include <utility>

SpotPointLight::SpotPointLight(const Point3D_d &i_position, Vector3D_d i_direction, const Spectrum_d &i_intensity, double i_internal_cone_angle, double i_outer_cone_angle): DeltaLightSource(),
m_position(i_position), m_direction(i_direction), m_intensity(i_intensity), m_internal_cone_angle(i_internal_cone_angle), m_outer_cone_angle(i_outer_cone_angle)
  {
  ASSERT(InRange(i_intensity,0.0,DBL_INF));
  ASSERT(i_direction.IsNormalized());
  ASSERT(i_internal_cone_angle <= i_outer_cone_angle);

  m_direction.Normalize();
  MathRoutines::CoordinateSystem(m_direction, m_e2, m_e3);

  if (m_internal_cone_angle > m_outer_cone_angle)
    std::swap(m_internal_cone_angle, m_outer_cone_angle);

  m_inner_angle_cos = cos(m_internal_cone_angle);
  m_outer_angle_cos = cos(m_outer_cone_angle);
  m_inv_cos_difference = 1.0 / (m_inner_angle_cos - m_outer_angle_cos);

  // The power is computed analytically by integrating the emitted radiance over the sphere.
  double inner_cone_power = 2.0*M_PI*(1.0 - m_inner_angle_cos);
  double power_in_fallof = 2.0*M_PI*(1.0/5.0)*fabs(m_inner_angle_cos-m_outer_angle_cos);
  m_power = (inner_cone_power + power_in_fallof) * m_intensity;
  }

Spectrum_d SpotPointLight::Power() const
  {
  return m_power;
  }

Spectrum_d SpotPointLight::Lighting(const Point3D_d &i_point, Ray &o_lighting_ray) const
  {
  Vector3D_d lighting_vector(m_position-i_point);
  double distance = lighting_vector.Length();
  double inv_distance = 1.0 / distance;

  o_lighting_ray.m_origin = i_point;
  o_lighting_ray.m_direction = lighting_vector * inv_distance;

  o_lighting_ray.m_min_t = 0.0;
  o_lighting_ray.m_max_t = distance;

  return m_intensity * (inv_distance*inv_distance) * _Falloff(lighting_vector.Normalized()*m_direction * (-1.0) );
  }

Spectrum_d SpotPointLight::SamplePhoton(const Point2D_d &i_sample, Ray &o_photon_ray, double &o_pdf) const
  {
  o_photon_ray.m_origin = m_position;
  o_photon_ray.m_min_t = 0.0;
  o_photon_ray.m_max_t = DBL_INF;

  Vector3D_d local = SamplingRoutines::UniformConeSampling(i_sample, m_outer_angle_cos);
  o_photon_ray.m_direction = m_e2*local[0]+m_e3*local[1]+m_direction*local[2];

  o_pdf = SamplingRoutines::UniformConePDF(m_outer_angle_cos);

  return m_intensity * _Falloff(local[2]);
  }

double SpotPointLight::_Falloff(double i_cos) const
  {
  // IMPORTANT!
  // Be careful when changing this method, the light's power is computed analytically and may need to be updated if the implementation is changed.

  if (i_cos < m_outer_angle_cos)
    return 0.0;
  if (i_cos > m_inner_angle_cos)
    return 1.0;

  // Compute falloff inside spotlight cone.
  // The implementation is taken from pbrt, not sure why it is done exactly this way.
  double delta = (i_cos - m_outer_angle_cos) * m_inv_cos_difference;
  ASSERT(delta >= 0.0 && delta <= 1.0);

  return delta*delta*delta*delta;
  }