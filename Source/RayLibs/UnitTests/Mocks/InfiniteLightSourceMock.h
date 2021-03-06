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

#ifndef INFINITY_LIGHT_SOURCE_MOCK_H
#define INFINITY_LIGHT_SOURCE_MOCK_H

#include <Common/Common.h>
#include <Raytracer/Core/LightSources.h>
#include <Math/Constants.h>
#include <Math/SamplingRoutines.h>

/*
InfiniteLightSource mock implementation.
Simulates constant light from the entire sphere.
*/
class InfiniteLightSourceMock: public InfiniteLightSource
  {
  public:
    InfiniteLightSourceMock(Spectrum_d i_radiance, const BBox3D_d &i_world_bounds);

    virtual Spectrum_d Radiance(const RayDifferential &i_ray) const;
    virtual Spectrum_d Power() const;
    virtual Spectrum_d SampleLighting(const Point2D_d &i_sample, Vector3D_d &o_direction, double &o_pdf) const;
    virtual double LightingPDF(const Vector3D_d &i_lighting_direction) const;
    virtual Spectrum_d SamplePhoton(const Point2D_d &i_position_sample, const Point2D_d &i_direction_sample, Ray &o_photon_ray, double &o_pdf) const;
    virtual Spectrum_d Irradiance(const Vector3D_d &i_normal) const;
    virtual Spectrum_d Fluence() const;

  private:
    Spectrum_d m_radiance;
    BBox3D_d m_world_bounds;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline InfiniteLightSourceMock::InfiniteLightSourceMock(Spectrum_d i_radiance, const BBox3D_d &i_world_bounds):
  m_radiance(i_radiance), m_world_bounds(i_world_bounds)
  {
  }

inline Spectrum_d InfiniteLightSourceMock::Radiance(const RayDifferential &i_ray) const
  {
  return m_radiance;
  }

inline Spectrum_d InfiniteLightSourceMock::Power() const
  {
  return (M_PI*m_world_bounds.Area())*m_radiance;
  }

inline Spectrum_d InfiniteLightSourceMock::SampleLighting(const Point2D_d &i_sample, Vector3D_d &o_direction, double &o_pdf) const
  {
  o_direction=SamplingRoutines::UniformSphereSampling(i_sample);
  o_pdf=SamplingRoutines::UniformSpherePDF();
  return m_radiance;
  }

inline double InfiniteLightSourceMock::LightingPDF(const Vector3D_d &i_lighting_direction) const
  {
  return SamplingRoutines::UniformSpherePDF();
  }

inline Spectrum_d InfiniteLightSourceMock::SamplePhoton(const Point2D_d &i_position_sample, const Point2D_d &i_direction_sample, Ray &o_photon_ray, double &o_pdf) const
  {
  double x = fabs(m_world_bounds.m_max[0]-m_world_bounds.m_min[0]);
  double y = fabs(m_world_bounds.m_max[1]-m_world_bounds.m_min[1]);
  double z = fabs(m_world_bounds.m_max[2]-m_world_bounds.m_min[2]);
  double radius = 0.5*sqrt(x*x+y*y+z*z);
  Point3D_d center = (m_world_bounds.m_max+m_world_bounds.m_min)/2.0;

  Point3D_d ray_origin = center+SamplingRoutines::UniformSphereSampling(i_position_sample)*radius;
  Point3D_d point2 = center+SamplingRoutines::UniformSphereSampling(i_direction_sample)*radius;
  Vector3D_d ray_direction = Vector3D_d(point2 - ray_origin).Normalized();

  o_photon_ray=Ray(ray_origin, ray_direction);
  double cos_theta = Vector3D_d(center - ray_origin).Normalized() * ray_direction;

  o_pdf = cos_theta / ((4.0 * M_PI * radius * radius));
  return m_radiance;
  }

inline Spectrum_d InfiniteLightSourceMock::Irradiance(const Vector3D_d &i_normal) const
  {
  return M_PI*m_radiance;
  }

inline Spectrum_d InfiniteLightSourceMock::Fluence() const
  {
  return 4.0*M_PI*m_radiance;
  }

#endif // INFINITY_LIGHT_SOURCE_MOCK_H