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

#ifndef VOLUME_REGION_MOCK_H
#define VOLUME_REGION_MOCK_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/VolumeRegion.h>

/*
VolumeRegion mock implementation.
Always returns the same value everywhere (pretty much as HomogeneousVolumeRegion does).
*/
class VolumeRegionMock: public VolumeRegion
  {
  public:
    VolumeRegionMock(BBox3D_d i_bounds, Spectrum_d &i_emission, SpectrumCoef_d &i_absorption, SpectrumCoef_d &i_scattering):
    m_bounds(i_bounds), m_emission(i_emission), m_absorption(i_absorption), m_scattering(i_scattering)
      {
      ASSERT(InRange(i_emission, 0.0, DBL_INF));
      ASSERT(InRange(i_absorption, 0.0, DBL_INF));
      ASSERT(InRange(i_scattering, 0.0, DBL_INF));

      m_attenuation = m_absorption + m_scattering;
      }

    BBox3D_d GetBounds() const
      {
      return m_bounds;
      }

    bool Intersect(const Ray &i_ray, double *op_t_begin, double *op_t_end) const
      {
      return m_bounds.Intersect(i_ray, op_t_begin, op_t_end);
      }

    Spectrum_d Emission(const Point3D_d &i_point) const
      {
      return m_bounds.Inside(i_point) ? m_emission : Spectrum_d(0.0);
      }

    SpectrumCoef_d Absorption(const Point3D_d &i_point) const
      {
      return m_bounds.Inside(i_point) ? m_absorption : SpectrumCoef_d(0.0);
      }

    SpectrumCoef_d Scattering(const Point3D_d &i_point) const
      {
      return m_bounds.Inside(i_point) ? m_scattering : SpectrumCoef_d(0.0);
      }

    SpectrumCoef_d Attenuation(const Point3D_d &i_point) const
      {
      return m_bounds.Inside(i_point) ? m_attenuation : SpectrumCoef_d(0.0);
      }

    double Phase(const Point3D_d &i_point, const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
      {
      return m_bounds.Inside(i_point) ? 1.0/(4.0*M_PI) : 0.0;
      }

    SpectrumCoef_d OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const
      {
      ASSERT(i_ray.m_direction.IsNormalized());
      ASSERT(i_step > 0.0 && i_offset_sample >= 0.0 && i_offset_sample < 1.0);

      double t_begin, t_end;
      if (m_bounds.Intersect(i_ray, &t_begin, &t_end))
        return fabs(t_end-t_begin) * m_attenuation;
      else
        return SpectrumCoef_d(0.0);
      }


  private:
    BBox3D_d m_bounds;

    Spectrum_d m_emission;
    SpectrumCoef_d m_absorption, m_scattering, m_attenuation;
  };

#endif // VOLUME_REGION_MOCK_H