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

#include "AggregateVolumeRegion.h"
#include <Raytracer/Core/SpectrumRoutines.h>

AggregateVolumeRegion::AggregateVolumeRegion(const std::vector<intrusive_ptr<const VolumeRegion>> &i_volume_regions): m_volume_regions(i_volume_regions)
  {
  // Precompute aggregated bounding box.
  m_bounds = BBox3D_d();
  for(size_t i=0;i<i_volume_regions.size();++i)
    m_bounds.Unite(i_volume_regions[i]->GetBounds());
  }

BBox3D_d AggregateVolumeRegion::GetBounds() const
  {
  return m_bounds;
  }

bool AggregateVolumeRegion::Intersect(const Ray &i_ray, double *op_t_begin, double *op_t_end) const
  {
  double t_begin = DBL_INF;
  double t_end = -DBL_INF;

  for (size_t i=0;i<m_volume_regions.size();++i)
    {
    double tmp_begin, tmp_end;
    if (m_volume_regions[i]->Intersect(i_ray, &tmp_begin, &tmp_end))
      {
      t_begin = std::min(t_begin, tmp_begin);
      t_end = std::max(t_end, tmp_end);
      }
    }

  if (op_t_begin) *op_t_begin = t_begin;
  if (op_t_end) *op_t_end = t_end;

  return t_begin < t_end;
  }

Spectrum_d AggregateVolumeRegion::Emission(const Point3D_d &i_point) const
  {
  Spectrum_d ret;

  for (size_t i=0;i<m_volume_regions.size();++i)
    ret += m_volume_regions[i]->Emission(i_point);

  return ret;
  }

SpectrumCoef_d AggregateVolumeRegion::Absorption(const Point3D_d &i_point) const
  {
  SpectrumCoef_d ret;

  for (size_t i=0;i<m_volume_regions.size();++i)
    ret += m_volume_regions[i]->Absorption(i_point);

  return ret;
  }

SpectrumCoef_d AggregateVolumeRegion::Scattering(const Point3D_d &i_point) const
  {
  SpectrumCoef_d ret;

  for (size_t i=0;i<m_volume_regions.size();++i)
    ret += m_volume_regions[i]->Scattering(i_point);

  return ret;
  }

SpectrumCoef_d AggregateVolumeRegion::Attenuation(const Point3D_d &i_point) const
  {
  SpectrumCoef_d ret;

  for (size_t i=0;i<m_volume_regions.size();++i)
    ret += m_volume_regions[i]->Attenuation(i_point);

  return ret;
  }

double AggregateVolumeRegion::Phase(const Point3D_d &i_point, const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  double ret = 0.0, sum_weights = 0.0;

  for (size_t i=0;i<m_volume_regions.size();++i)
    {
    double scattering = SpectrumRoutines::Luminance(m_volume_regions[i]->Scattering(i_point));
    sum_weights += scattering;
    ret += scattering * m_volume_regions[i]->Phase(i_point, i_incoming, i_outgoing);
    }

  if (sum_weights > 0.0)
    return ret / sum_weights;
  else
    return 0.0;
  }

SpectrumCoef_d AggregateVolumeRegion::OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const
  {
  SpectrumCoef_d ret;

  for (size_t i=0;i<m_volume_regions.size();++i)
    ret += m_volume_regions[i]->OpticalThickness(i_ray, i_step, i_offset_sample);

  return ret;
  }
