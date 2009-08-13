#ifndef RAY_H
#define RAY_H

#include <Common\Common.h>
#include <limits>
#include "Point3D.h"
#include "Vector3D.h"

class RayDifferential;

class Ray
  {
  public:
    Ray();
    Ray(const Point3Dd i_origin, const Vector3Dd &i_direction, double i_mint=0.0, double i_maxt=DBL_INF);
    Ray(const RayDifferential &i_ray_differential);

    Point3Dd operator()(double i_t) const;

  public:
    Point3Dd m_origin;
    Vector3Dd m_direction;
    double m_mint, m_maxt;
  };

class RayDifferential
  {
  public:
    RayDifferential();
    RayDifferential(const Ray &i_ray);

  public:
    Ray m_base_ray;
    Point3Dd m_origin_dx, m_origin_dy;
    Vector3Dd m_direction_dx, m_direction_dy;

    bool m_has_differentials;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Ray::Ray():
m_mint(0.), m_maxt(DBL_INF)
  {
  }

inline Ray::Ray(const Point3Dd i_origin, const Vector3Dd &i_direction, double i_mint, double i_maxt):
m_origin(i_origin), m_direction(i_direction), m_mint(i_mint), m_maxt(i_maxt)
  {
  }

inline Ray::Ray(const RayDifferential &i_ray_differential):
m_origin(i_ray_differential.m_base_ray.m_origin), m_direction(i_ray_differential.m_base_ray.m_direction),
m_mint(i_ray_differential.m_base_ray.m_mint), m_maxt(i_ray_differential.m_base_ray.m_maxt)
  {
  }

inline Point3Dd Ray::operator()(double i_t) const
  {
  return m_origin+m_direction*i_t;
  }


inline RayDifferential::RayDifferential():
  m_base_ray(), m_has_differentials(false)
  {
  }

inline RayDifferential::RayDifferential(const Ray &i_ray):
m_base_ray(i_ray), m_has_differentials(false)
  {
  }


#endif // RAY_H
