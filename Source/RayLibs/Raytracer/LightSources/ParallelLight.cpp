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

#include "ParallelLight.h"
#include <Math/Constants.h>
#include <Math/MathRoutines.h>
#include <Math/SamplingRoutines.h>
#include <utility>

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_IMPLEMENT(ParallelLight);

ParallelLight::ParallelLight(const Vector3D_d &i_direction, const Spectrum_d &i_radiance, const BBox3D_d &i_world_bounds): DeltaLightSource(),
m_direction(i_direction), m_radiance(i_radiance), m_world_bounds(i_world_bounds)
  {
  ASSERT(m_direction.IsNormalized());
  ASSERT(InRange(i_radiance,0.0,DBL_INF));
  m_direction.Normalize();

  double min_x=i_world_bounds.m_min[0], max_x=i_world_bounds.m_max[0];
  double min_y=i_world_bounds.m_min[1], max_y=i_world_bounds.m_max[1];
  double min_z=i_world_bounds.m_min[2], max_z=i_world_bounds.m_max[2];

  if (min_x>max_x) std::swap(min_x, max_x);
  if (min_y>max_y) std::swap(min_y, max_y);
  if (min_z>max_z) std::swap(min_z, max_z);

  double x_extent = max_x-min_x, y_extent = max_y-min_y, z_extent = max_z-min_z;
  m_bbox_projected_area = x_extent*y_extent*fabs(i_direction[2])+x_extent*z_extent*fabs(i_direction[1])+y_extent*z_extent*fabs(i_direction[0]);
  m_inv_bbox_projected_area = (m_bbox_projected_area > DBL_EPS) ? 1.0 / m_bbox_projected_area : 0.0;

  m_power = m_bbox_projected_area * i_radiance;

  double emitting_x = i_direction[0]>0 ? min_x : max_x;
  double emitting_y = i_direction[1]>0 ? min_y : max_y;
  double emitting_z = i_direction[2]>0 ? min_z : max_z;

  m_bbox_emitting_triangles[0]=Triangle3D_d(Point3D_d(emitting_x,min_y,min_z), Point3D_d(emitting_x,min_y,max_z), Point3D_d(emitting_x,max_y,min_z));
  m_bbox_emitting_triangles[1]=Triangle3D_d(Point3D_d(emitting_x,max_y,max_z), Point3D_d(emitting_x,min_y,max_z), Point3D_d(emitting_x,max_y,min_z));

  m_bbox_emitting_triangles[2]=Triangle3D_d(Point3D_d(min_x,emitting_y,min_z), Point3D_d(max_x,emitting_y,min_z), Point3D_d(min_x,emitting_y,max_z));
  m_bbox_emitting_triangles[3]=Triangle3D_d(Point3D_d(max_x,emitting_y,max_z), Point3D_d(max_x,emitting_y,min_z), Point3D_d(min_x,emitting_y,max_z));

  m_bbox_emitting_triangles[4]=Triangle3D_d(Point3D_d(min_x,min_y,emitting_z), Point3D_d(min_x,max_y,emitting_z), Point3D_d(max_x,min_y,emitting_z));
  m_bbox_emitting_triangles[5]=Triangle3D_d(Point3D_d(max_x,max_y,emitting_z), Point3D_d(min_x,max_y,emitting_z), Point3D_d(max_x,min_y,emitting_z));

  m_area_CDF[0]=m_bbox_emitting_triangles[0].GetArea()*fabs(i_direction[0]);
  m_area_CDF[1]=m_bbox_emitting_triangles[1].GetArea()*fabs(i_direction[0]);
  m_area_CDF[2]=m_bbox_emitting_triangles[2].GetArea()*fabs(i_direction[1]);
  m_area_CDF[3]=m_bbox_emitting_triangles[3].GetArea()*fabs(i_direction[1]);
  m_area_CDF[4]=m_bbox_emitting_triangles[4].GetArea()*fabs(i_direction[2]);
  m_area_CDF[5]=m_bbox_emitting_triangles[5].GetArea()*fabs(i_direction[2]);

  for(size_t i=1;i<6;++i) m_area_CDF[i] += m_area_CDF[i-1];
  ASSERT(fabs(m_area_CDF[5]-m_bbox_projected_area) < 1e-10);

  for(size_t i=0;i<6;++i) m_area_CDF[i] /= m_area_CDF[5];
  }

Spectrum_d ParallelLight::Power() const
  {
  return m_power;
  }

Spectrum_d ParallelLight::Lighting(const Point3D_d &i_point, Ray &o_lighting_ray) const
  {
  o_lighting_ray.m_origin = i_point;
  o_lighting_ray.m_direction = m_direction*(-1.0);

  o_lighting_ray.m_min_t = 0.0;
  o_lighting_ray.m_max_t = DBL_INF;

  return m_radiance;
  }

Spectrum_d ParallelLight::SamplePhoton(const Point2D_d &i_sample, Ray &o_photon_ray, double &o_pdf) const
  {
  Point2D_d sample = i_sample;

  double pdf;
  const double *p_sampled = MathRoutines::BinarySearchCDF(&(m_area_CDF[0]), (&m_area_CDF[5])+1, sample[0], &pdf);
  int index = p_sampled-&(m_area_CDF[0]);
  ASSERT(index>=0 && index<6);

  if (index>0)
    sample[0] = (sample[0]-m_area_CDF[index-1])/pdf;
  else
    sample[0] = sample[0]/pdf;
  ASSERT(sample[0]>=0.0 && sample[0]<1.0);

  double b1, b2, b0;
  SamplingRoutines::UniformTriangleSampling(sample, b1, b2);
  b0 = 1.0-b1-b2;

  Point3D_d sampled_point = m_bbox_emitting_triangles[index][0]*b0 + m_bbox_emitting_triangles[index][1]*b1 + m_bbox_emitting_triangles[index][2]*b2;

  o_photon_ray.m_origin = sampled_point - m_direction*1.0;
  o_photon_ray.m_direction = m_direction;
  o_photon_ray.m_min_t = 0.0;
  o_photon_ray.m_max_t = DBL_INF;

  o_pdf = m_inv_bbox_projected_area;

  return m_radiance;
  }