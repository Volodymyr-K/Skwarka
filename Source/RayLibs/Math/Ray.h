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

#ifndef RAY_H
#define RAY_H

#include <Common/Common.h>
#include <limits>
#include "Point3D.h"
#include "Vector3D.h"

class RayDifferential;

/**
* Represents 3D ray.
* More strictly, the class represents a segment of a ray in 3D, possibly infinite in any or both directions.
* A ray is defined by its origin (a 3D point), direction (a 3D vector) and min_t and max_t parameters.
* @sa RayDifferential
*/
class Ray
  {
  public:
    Ray();
    /**
    * Constructs Ray object from the specified origin, direction vector, min and max parameters.
    * @param i_origin A point on the ray. i_min_t and i_max_t parameters are counted from this point.
    * @param i_direction Direction of the ray.
    * @param i_min_t Lower bound of the parametric region.
    * @param i_max_t Upper bound of the parametric region.
    */
    Ray(const Point3D_d &i_origin, const Vector3D_d &i_direction, double i_min_t=0.0, double i_max_t=DBL_INF);

    /**
    * Constructs Ray object from the specified RayDifferential object.
    * It simply discards the differentials and initializes the object with it's base ray.
    */
    explicit Ray(const RayDifferential &i_ray_differential);

    /**
    * Returns a 3D point on the ray corresponding to the specified t parameter.
    */
    Point3D_d operator()(double i_t) const;

  public:
    // Public data members.
    Point3D_d m_origin;
    Vector3D_d m_direction;
    double m_min_t, m_max_t;
  };

/**
* Represents 3D ray with its screen-space differentials and depth information.
* The differentials are two additional rays corresponding to the neighbor x and y image pixels.
* The differential rays do not have bounded parametric region.
* @sa Ray
*/
class RayDifferential
  {
  public:
    RayDifferential();
    explicit RayDifferential(const Ray &i_ray);

  public:
    // Public data members.

    /**
    * Base ray corresponding to the current image pixel.
    */
    Ray m_base_ray;

    /**
    * Origin of the x-differential ray.
    */
    Point3D_d m_origin_dx;

    /**
    * Origin of the y-differential ray.
    */
    Point3D_d m_origin_dy;

    /**
    * Direction of the x-differential ray.
    */
    Vector3D_d m_direction_dx;

    /**
    * Direction of the y-differential ray.
    */
    Vector3D_d m_direction_dy;

    /**
    * True if the differential rays are present and false otherwise.
    */
    bool m_has_differentials;

    /**
    * Ray specular depth, i.e. how many time the ray was reflected or refracted.
    */
    size_t m_specular_depth;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Ray::Ray():
m_min_t(0.), m_max_t(DBL_INF)
  {
  }

inline Ray::Ray(const Point3D_d &i_origin, const Vector3D_d &i_direction, double i_min_t, double i_max_t):
m_origin(i_origin), m_direction(i_direction), m_min_t(i_min_t), m_max_t(i_max_t)
  {
  }

inline Ray::Ray(const RayDifferential &i_ray_differential):
m_origin(i_ray_differential.m_base_ray.m_origin), m_direction(i_ray_differential.m_base_ray.m_direction),
m_min_t(i_ray_differential.m_base_ray.m_min_t), m_max_t(i_ray_differential.m_base_ray.m_max_t)
  {
  }

inline Point3D_d Ray::operator()(double i_t) const
  {
  return m_origin+m_direction*i_t;
  }

inline RayDifferential::RayDifferential():
m_base_ray(), m_has_differentials(false), m_specular_depth(0)
  {
  }

inline RayDifferential::RayDifferential(const Ray &i_ray):
m_base_ray(i_ray), m_has_differentials(false), m_specular_depth(0)
  {
  }

#endif // RAY_H