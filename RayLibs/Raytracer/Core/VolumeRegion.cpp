#include "VolumeRegion.h"

DensityVolumeRegion::DensityVolumeRegion(const BBox3D_d &i_bounds, Spectrum_d &i_base_emission, SpectrumCoef_d &i_base_absorption,
                                         SpectrumCoef_d &i_base_scattering, intrusive_ptr<const PhaseFunction> ip_phase_function):
m_bounds(i_bounds), m_base_emission(i_base_emission), m_base_absorption(i_base_absorption), m_base_scattering(i_base_scattering), mp_phase_function(ip_phase_function)
  {
  ASSERT(ip_phase_function);
  ASSERT(InRange(i_base_emission, 0.0, DBL_INF));
  ASSERT(InRange(i_base_absorption, 0.0, DBL_INF));
  ASSERT(InRange(i_base_scattering, 0.0, DBL_INF));

  ASSERT(m_bounds.Volume() > 0.0);
  if (m_bounds.m_min[0] > m_bounds.m_max[0]) std::swap(m_bounds.m_min[0], m_bounds.m_max[0]);
  if (m_bounds.m_min[1] > m_bounds.m_max[1]) std::swap(m_bounds.m_min[1], m_bounds.m_max[1]);
  if (m_bounds.m_min[2] > m_bounds.m_max[2]) std::swap(m_bounds.m_min[2], m_bounds.m_max[2]);

  m_base_attenuation = m_base_absorption+m_base_scattering;
  }

BBox3D_d DensityVolumeRegion::GetBounds() const
  {
  return m_bounds;
  }

Spectrum_d DensityVolumeRegion::Emission(const Point3D_d &i_point) const
  {
  return _Density(i_point) * m_base_emission;
  }

SpectrumCoef_d DensityVolumeRegion::Absorption(const Point3D_d &i_point) const
  {
  return _Density(i_point) * m_base_absorption;
  }

SpectrumCoef_d DensityVolumeRegion::Scattering(const Point3D_d &i_point) const
  {
  return _Density(i_point) * m_base_scattering;
  }

SpectrumCoef_d DensityVolumeRegion::Attenuation(const Point3D_d &i_point) const
  {
  return _Density(i_point) * m_base_attenuation;
  }

double DensityVolumeRegion::Phase(const Point3D_d &i_point, const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  return m_bounds.Inside(i_point) ? mp_phase_function->ScatteringPDF(i_incoming, i_outgoing) : 0.0;
  }

SpectrumCoef_d DensityVolumeRegion::OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());
  ASSERT(i_step > 0.0 && i_offset_sample >= 0.0 && i_offset_sample < 1.0);

  double t_begin, t_end;
  if (this->Intersect(i_ray, &t_begin, &t_end)==false)
    return SpectrumCoef_d(0.0);

  double optical_thickness = 0.0;
  double t = t_begin, step = i_step;
  while (t<t_end-DBL_EPS)
    {
    step = std::min(step, t_end-t);
    optical_thickness += _Density( i_ray(t+i_offset_sample * step) ) * step;
    t += step;
    }

  return optical_thickness * m_base_attenuation;
  }