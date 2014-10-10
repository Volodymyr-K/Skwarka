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

#include "PointLight.h"
#include <Math/Constants.h>
#include <Math/SamplingRoutines.h>

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_IMPLEMENT(PointLight);

PointLight::PointLight(const Point3D_d &i_position, const Spectrum_d &i_intensity): DeltaLightSource(),
m_position(i_position), m_intensity(i_intensity)
  {
  ASSERT(InRange(i_intensity,0.0,DBL_INF));
  }

Spectrum_d PointLight::Power() const
  {
  return 4.0*M_PI*m_intensity;
  }

Spectrum_d PointLight::Lighting(const Point3D_d &i_point, Ray &o_lighting_ray) const
  {
  Vector3D_d lighting_vector(m_position-i_point);
  double distance = lighting_vector.Length();
  double inv_distance = 1.0 / distance;

  o_lighting_ray.m_origin = i_point;
  o_lighting_ray.m_direction = lighting_vector * inv_distance;

  o_lighting_ray.m_min_t = 0.0;
  o_lighting_ray.m_max_t = distance;

  return m_intensity * (inv_distance*inv_distance);
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