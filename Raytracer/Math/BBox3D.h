#ifndef BBOX3D_H
#define BBOX3D_H

#include <Common/Common.h>
#include "Point3D.h"
#include "Triangle3D.h"
#include <utility>

/**
* Class template for 3D bounding box.
* The bounding box is a simple 3D box with edges parallel to the axis.
* The template parameter corresponds to the type of the vector coordinates.
*/
template<typename T>
class BBox3D
  {
  public:
    BBox3D();
    BBox3D(const Point3D<T> &i_min, const Point3D<T> &i_max);

    /**
    * Returns the volume of the box.
    */
    T Volume() const;

    /**
    * Checks if the specified ray intersects the box.
    */
    bool Intersect(const Ray &i_ray) const;

    /**
    * Checks if the specified point is inside the box.
    * If the point is on the edge, face or vertex of the box it is still counted as being inside.
    */
    bool Inside(const Point3D<T> &i_point) const;

    /**
    * Checks if the specified triangle is inside the box.
    * The triangle is counted as being inside if all it's vertices are inside the box.
    */
    bool Inside(const Triangle3D<T> &i_triangle) const;

  public:
    // Public data members.
    Point3D<T> m_min, m_max;
  };

typedef BBox3D<float> BBox3D_f;
typedef BBox3D<double> BBox3D_d;

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
bool BBox3D<T>::Intersect(const Ray &i_ray) const
  {
  double t0 = i_ray.m_min_t, t1 = i_ray.m_max_t;
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

template<typename T>
bool BBox3D<T>::Inside(const Point3D<T> &i_point) const
  {
  return i_point[0]>=m_min[0] && i_point[1]>=m_min[1] && i_point[2]>=m_min[2] &&
    i_point[0]<=m_max[0] && i_point[1]<=m_max[1] && i_point[2]<=m_max[2];
  }

template<typename T>
bool BBox3D<T>::Inside(const Triangle3D<T> &i_triangle) const
  {
  return Inside(i_triangle[0]) && Inside(i_triangle[1]) && Inside(i_triangle[2]);
  }

#endif // BBOX3D_H