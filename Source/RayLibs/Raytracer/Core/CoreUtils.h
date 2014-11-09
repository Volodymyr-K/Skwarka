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

#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#include <Math/Geometry.h>
#include "DifferentialGeometry.h"
#include "Intersection.h"
#include <windows.h>
#include <algorithm>

/**
* This namespace contains helper routines for the raytracer core.
*/
namespace CoreUtils
  {

  /**
  * Sets ray differentials for the specularly reflected ray.
  * The method does not change the base ray of the o_exitant_ray.
  * @param i_incident_ray Incident ray. The direction component should be normalized.
  * @param i_dg Describes intersection point.
  * @param[out] o_exitant_ray The ray whose differential components will be set.
  */
  void SetReflectedDifferentials(const RayDifferential &i_incident_ray, const DifferentialGeometry &i_dg, RayDifferential &o_exitant_ray);

  /**
  * Sets ray differentials for the specularly transmitted ray.
  * The method does not change the base ray of the o_exitant_ray.
  * In case of total internal reflection the exitant ray won't have the differentials (the m_has_differentials field will be false).
  * @param i_incident_ray Incident ray. The direction component should be normalized.
  * @param i_dg Describes intersection point.
  * @param i_refractive_index Refractive index of the object (the object is always considered to be "below" the surface normal).
  * @param[out] o_exitant_ray The ray whose differential components will be set.
  */
  void SetTransmittedDifferentials(const RayDifferential &i_incident_ray, const DifferentialGeometry &i_dg, double i_refractive_index, RayDifferential &o_exitant_ray);

  /**
  * Helper method that given an intersection and an outgoing direction returns the minimum ray parameter that avoids intersecting with the same triangle.
  * The ray origin is assumed to be at the specified intersection point.
  */
  double GetNextMinT(const Intersection &i_intersection, const Vector3D_d &i_direction);

  /**
  * Sets priority for the current thread.
  * @param i_priority Value that defines thread priority. Should be one of THREAD_PRIORITY_xxx constants defined in windows.h
  * @return Previous priority of the current thread.
  */
  int SetCurrentThreadPriority(int i_priority);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace CoreUtils
  {

  inline void SetReflectedDifferentials(const RayDifferential &i_incident_ray, const DifferentialGeometry &i_dg, RayDifferential &o_exitant_ray)
    {
    ASSERT(i_incident_ray.m_base_ray.m_direction.IsNormalized());
    o_exitant_ray.m_has_differentials = false;

    if (i_incident_ray.m_has_differentials==false)
      return;

    ASSERT(i_incident_ray.m_direction_dx.IsNormalized());
    ASSERT(i_incident_ray.m_direction_dy.IsNormalized());

    o_exitant_ray.m_origin_dx = i_dg.m_point_dx;
    o_exitant_ray.m_direction_dx = i_incident_ray.m_direction_dx - (2.0*(i_incident_ray.m_direction_dx*i_dg.m_normal_dx)) * i_dg.m_normal_dx;
    ASSERT(o_exitant_ray.m_direction_dx.IsNormalized());

    o_exitant_ray.m_origin_dy = i_dg.m_point_dy;
    o_exitant_ray.m_direction_dy = i_incident_ray.m_direction_dy - (2.0*(i_incident_ray.m_direction_dy*i_dg.m_normal_dy)) * i_dg.m_normal_dy;
    ASSERT(o_exitant_ray.m_direction_dy.IsNormalized());

    o_exitant_ray.m_has_differentials = true;
    }

  inline void SetTransmittedDifferentials(const RayDifferential &i_incident_ray, const DifferentialGeometry &i_dg, double i_refractive_index, RayDifferential &o_exitant_ray)
    {
    ASSERT(i_incident_ray.m_base_ray.m_direction.IsNormalized());
    ASSERT(i_refractive_index > 0.0);
    o_exitant_ray.m_has_differentials = false;

    if (i_incident_ray.m_has_differentials==false)
      return;

    ASSERT(i_incident_ray.m_direction_dx.IsNormalized());
    ASSERT(i_incident_ray.m_direction_dy.IsNormalized());

    double Z_sign = 1.0;
    double eta = i_refractive_index;
    if (i_incident_ray.m_base_ray.m_direction * i_dg.m_shading_normal < 0.0)
      {
      // If the ray enters the refractive object.
      eta = 1.0 / i_refractive_index;
      Z_sign = -1.0;
      }

    double cos_theta_incident_dx = -(i_incident_ray.m_direction_dx * i_dg.m_normal_dx);
    double sin_theta_incident_sqr_dx = 1.0-cos_theta_incident_dx*cos_theta_incident_dx;
    double sin_theta_exitant_sqr_dx = eta*eta*sin_theta_incident_sqr_dx;

    double cos_theta_incident_dy = -(i_incident_ray.m_direction_dy * i_dg.m_normal_dy);
    double sin_theta_incident_sqr_dy = 1.0-cos_theta_incident_dy*cos_theta_incident_dy;
    double sin_theta_exitant_sqr_dy = eta*eta*sin_theta_incident_sqr_dy;

    // If not total internal reflection.
    if (sin_theta_exitant_sqr_dx < 1.0 && sin_theta_exitant_sqr_dy < 1.0)
      {
      double cos_theta_exitant_dx = Z_sign*sqrt(1.0-sin_theta_exitant_sqr_dx);
      double cos_theta_exitant_dy = Z_sign*sqrt(1.0-sin_theta_exitant_sqr_dy);

      o_exitant_ray.m_origin_dx = i_dg.m_point_dx;
      o_exitant_ray.m_direction_dx = eta*i_incident_ray.m_direction_dx+(eta*cos_theta_incident_dx+cos_theta_exitant_dx)*i_dg.m_normal_dx;
      ASSERT(o_exitant_ray.m_direction_dx.IsNormalized());

      o_exitant_ray.m_origin_dy = i_dg.m_point_dy;
      o_exitant_ray.m_direction_dy = eta*i_incident_ray.m_direction_dy+(eta*cos_theta_incident_dy+cos_theta_exitant_dy)*i_dg.m_normal_dy;
      ASSERT(o_exitant_ray.m_direction_dy.IsNormalized());

      o_exitant_ray.m_has_differentials = true;
      }
    }

  inline double GetNextMinT(const Intersection &i_intersection, const Vector3D_d &i_direction)
    {
    ASSERT(i_direction.IsNormalized());
    Transform world_to_mesh = i_intersection.mp_primitive->GetMeshToWorldTransform().Inverted();

    double divisor = world_to_mesh(i_direction)*i_intersection.m_cross;
    if (divisor == 0.0)
      return 0.0;
    else
      return std::max(0.0,(i_intersection.m_dot * (1.0/divisor)) + (1e-14));
    }

  inline int SetCurrentThreadPriority(int i_priority)
    {
    const HANDLE h_thread = ::GetCurrentThread();
    const int prev_priority = ::GetThreadPriority(h_thread);
    ::SetThreadPriority(h_thread, i_priority);
    return prev_priority;
    }
  }

#endif // CORE_UTILS_H