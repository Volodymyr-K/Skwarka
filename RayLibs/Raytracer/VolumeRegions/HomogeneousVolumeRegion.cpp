#include "HomogeneousVolumeRegion.h"

HomogeneousVolumeRegion::HomogeneousVolumeRegion(const BBox3D_d &i_bounds, Spectrum_d &i_emission, Spectrum_d &i_absorption,
                                                 Spectrum_d &i_scattering, intrusive_ptr<const PhaseFunction> ip_phase_function):
m_bounds(i_bounds), m_emission(i_emission), m_absorption(i_absorption), m_scattering(i_scattering), mp_phase_function(ip_phase_function)
  {
  ASSERT(InRange(i_emission, 0.0, DBL_INF));
  ASSERT(InRange(i_absorption, 0.0, DBL_INF));
  ASSERT(InRange(i_scattering, 0.0, DBL_INF));

  m_attenuation = m_absorption + m_scattering;
  }

BBox3D_d HomogeneousVolumeRegion::GetBounds() const
  {
  return m_bounds;
  }

Spectrum_d HomogeneousVolumeRegion::GetEmission() const
  {
  return m_emission;
  }

Spectrum_d HomogeneousVolumeRegion::GetAbsorption() const
  {
  return m_absorption;
  }

Spectrum_d HomogeneousVolumeRegion::GetScattering() const
  {
  return m_scattering;
  }

intrusive_ptr<const PhaseFunction> HomogeneousVolumeRegion::GetPhaseFunction() const
  {
  return mp_phase_function;
  }

bool HomogeneousVolumeRegion::Intersect(Ray i_ray, double *op_t_begin, double *op_t_end) const
  {
  return m_bounds.Intersect(i_ray, op_t_begin, op_t_end);
  }

Spectrum_d HomogeneousVolumeRegion::Emission(const Point3D_d &i_point) const
  {
  return m_bounds.Inside(i_point) ? m_emission : Spectrum_d(0.0);
  }

Spectrum_d HomogeneousVolumeRegion::Absorption(const Point3D_d &i_point) const
  {
  return m_bounds.Inside(i_point) ? m_absorption : Spectrum_d(0.0);
  }

Spectrum_d HomogeneousVolumeRegion::Scattering(const Point3D_d &i_point) const
  {
  return m_bounds.Inside(i_point) ? m_scattering : Spectrum_d(0.0);
  }

Spectrum_d HomogeneousVolumeRegion::Attenuation(const Point3D_d &i_point) const
  {
  return m_bounds.Inside(i_point) ? m_attenuation : Spectrum_d(0.0);
  }

double HomogeneousVolumeRegion::Phase(const Point3D_d &i_point, const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  return m_bounds.Inside(i_point) ? mp_phase_function->ScatteringPDF(i_incoming, i_outgoing) : 0.0;
  }

Spectrum_d HomogeneousVolumeRegion::OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());
  ASSERT(i_step > 0.0 && i_offset_sample >= 0.0 && i_offset_sample < 1.0);

  double t_begin, t_end;
  if (m_bounds.Intersect(i_ray, &t_begin, &t_end))
    return fabs(t_end-t_begin) * m_attenuation;
  else
    return Spectrum_d(0.0);
  }