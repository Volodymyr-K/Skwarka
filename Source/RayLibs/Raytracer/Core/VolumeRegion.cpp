/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "VolumeRegion.h"
#include <Raytracer/Core/SpectrumRoutines.h>

bool VolumeRegion::SampleScattering(const Ray &i_ray, double i_sample, double i_step, double i_offset_sample, double &o_t, double &o_pdf, SpectrumCoef_d &o_transmittance) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());
  ASSERT(i_step > 0.0 && i_offset_sample >= 0.0 && i_offset_sample < 1.0);

  double t_begin, t_end;
  if (this->Intersect(i_ray, &t_begin, &t_end)==false)
    {
    o_pdf=1.0;
    o_transmittance=SpectrumCoef_d(1.0);
    return false;
    }

  double sampled_thickness = -std::log(i_sample);

  SpectrumCoef_d thickness(0.0);
  double thickness_luminance = 0.0;
  double t = t_begin, step = i_step;
  while (t<t_end-DBL_EPS)
    {
    step = std::min(step, t_end-t);

    SpectrumCoef_d attenuation = this->Attenuation(i_ray(t+i_offset_sample * step));
    double attenuation_luminance = SpectrumRoutines::Luminance(attenuation);

    if (thickness_luminance+attenuation_luminance*step > sampled_thickness)
      {
      // We need to be careful since the attenuation at the sampled point can differ from the attenuation_luminance variable,
      // which might result in inaccurate PDF value begin returned. This can lead to visual artifacts during Monte Carlo integration.
      // To address this issue, we decrease the step by half if the attenuation at the sampled point differs by more than 10%
      double rescaled_step = (sampled_thickness-thickness_luminance)/attenuation_luminance;
      double att2 = SpectrumRoutines::Luminance(this->Attenuation(i_ray(t + rescaled_step)));
      if (fabs(att2-attenuation_luminance) > 0.1*attenuation_luminance)
        {
        step *= 0.5;
        continue;
        }

      o_t = t + rescaled_step;
      thickness += attenuation * rescaled_step;

      o_pdf = att2 * i_sample; // i_sample equals the transmittance at this point, because that is what we sampled.
      o_transmittance = Exp(-1.0*thickness);
      return true;
      }

    thickness += attenuation * step;
    thickness_luminance += attenuation_luminance * step;
    t += step;
    }

  // No scattering
  o_pdf = std::exp(-thickness_luminance);
  o_transmittance = Exp(-1.0*thickness);
  return false;
  }

DensityVolumeRegion::DensityVolumeRegion(const BBox3D_d &i_bounds, SpectrumCoef_d &i_base_absorption,
                                         SpectrumCoef_d &i_base_scattering, intrusive_ptr<const PhaseFunction> ip_phase_function):
m_bounds(i_bounds), m_base_absorption(i_base_absorption), m_base_scattering(i_base_scattering), mp_phase_function(ip_phase_function)
  {
  ASSERT(ip_phase_function);
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

SpectrumCoef_d DensityVolumeRegion::Absorption(const Point3D_d &i_point) const
  {
  if (m_bounds.Inside(i_point)==false)
    return SpectrumCoef_d(0.0);

  return _Density(i_point) * m_base_absorption;
  }

SpectrumCoef_d DensityVolumeRegion::Scattering(const Point3D_d &i_point) const
  {
  if (m_bounds.Inside(i_point)==false)
    return SpectrumCoef_d(0.0);

  return _Density(i_point) * m_base_scattering;
  }

SpectrumCoef_d DensityVolumeRegion::Attenuation(const Point3D_d &i_point) const
  {
  if (m_bounds.Inside(i_point)==false)
    return SpectrumCoef_d(0.0);

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

  double density = 0.0;
  double t = t_begin, step = i_step;
  while (t<t_end-DBL_EPS)
    {
    step = std::min(step, t_end-t);
    density += _Density(i_ray(t+i_offset_sample * step)) * step;
    t += step;
    }

  return density * m_base_attenuation;
  }