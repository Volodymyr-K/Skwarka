#ifndef BBOX3D_H
#define BBOX3D_H

#include <Common\Common.h>
#include "Point3D.h"
#include "Triangle3D.h"
#include <utility>

template<typename T>
class BBox3D
  {
  public:
    BBox3D();
    BBox3D(const Point3D<T> &i_min, const Point3D<T> &i_max);

    T Volume() const;
    bool Intersect(const Ray &i_ray) const;

  public:
    Point3D<T> m_min, m_max;
  };

template<typename T>
bool Inside(const BBox3D<T> &i_bbox, const Point3D<T> &i_point);

template<typename T>
bool Inside(const BBox3D<T> &i_bbox, const Triangle3D<T> &i_triangle);

typedef BBox3D<float> BBox3Df;
typedef BBox3D<double> BBox3Dd;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
BBox3D<T>::BBox3D()
  {
  }

template<typename T>
BBox3D<T>::BBox3D(const Point3D<T> &i_min, const Point3D<T> &i_max):
  m_min(i_min), m_max(i_max)
  {
  }

template<typename T>
T BBox3D<T>::Volume() const
  {
  Point3D<T> diagonal = m_max-m_min;
  return diagonal[0]*diagonal[1]*diagonal[2];
  }

template<typename T>
bool Inside(const BBox3D<T> &i_bbox, const Point3D<T> &i_point)
  {
  return i_point[0]>=i_bbox.m_min[0] && i_point[1]>=i_bbox.m_min[1] && i_point[2]>=i_bbox.m_min[2] &&
    i_point[0]<=i_bbox.m_max[0] && i_point[1]<=i_bbox.m_max[1] && i_point[2]<=i_bbox.m_max[2];
  }

template<typename T>
bool Inside(const BBox3D<T> &i_bbox, const Triangle3D<T> &i_triangle)
  {
  return Inside(i_bbox, i_triangle[0]) && Inside(i_bbox, i_triangle[1]) && Inside(i_bbox, i_triangle[2]);
  }

template<typename T>
bool BBox3D<T>::Intersect(const Ray &i_ray) const 
  {
  double t0 = i_ray.m_mint, t1 = i_ray.m_maxt;
  for (int i = 0; i < 3; ++i)
    {
    // Update interval for _i_th bounding box slab
    double invRayDir = 1.0 / i_ray.m_direction[i];
    double tNear = (m_min[i] - i_ray.m_origin[i]) * invRayDir;
    double tFar  = (m_max[i] - i_ray.m_origin[i]) * invRayDir;

    if (tNear > tFar) std::swap(tNear, tFar);
    t0 = tNear > t0 ? tNear : t0;
    t1 = tFar  < t1 ? tFar  : t1;
    if (t0 > t1) return false;
    }
  return true;
  }

#endif // BBOX3D_H