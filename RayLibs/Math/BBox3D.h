#ifndef BBOX3D_H
#define BBOX3D_H

#include <Common/Common.h>
#include "Ray.h"
#include "Point3D.h"
#include "Triangle3D.h"
#include <utility>
#include <numeric>

/**
* Class template for 3D bounding box.
* The bounding box is a simple 3D box with edges parallel to the axis.
* The template parameter corresponds to the type of the vector coordinates.
*/
template<typename T>
class BBox3D
  {
  public:

    /**
    * Default constructor initializes the bounding box with infinity values (positive for m_min and negative for m_max).
    */
    BBox3D();

    /**
    * Creates BBox3D instance with the specified coorindates.
    */
    BBox3D(const Point3D<T> &i_min, const Point3D<T> &i_max);

    /**
    * Returns the volume of the box.
    * @return Volume of the box. Can be negative if any of the m_max's coordinate is less than the corresponding m_min's coordinate.
    */
    T Volume() const;

    /**
    * Returns surface area of the box.
    * @return Surface area of the box. Greater or equal than zero.
    */
    T Area() const;

    /**
    * Checks if the specified ray intersects the box and computes ray parametric coordinates of the intersection region.
    * @param i_ray Input ray.
    * @param op_t_begin Parametric coordinate of the begin of the intersection region.Can be NULL.
    * @param op_t_end Parametric coordinate of the end of the intersection region. Can be NULL.
    */
    bool Intersect(const Ray &i_ray, double *op_t_begin = NULL, double *op_t_end = NULL) const;

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

    /**
    * Unites the bounding box with the specified 3D point.
    */
    template<typename T2>
    void Unite(const Point3D<T2> &i_point);

    /**
    * Unites the bounding box with the specified bounding box.
    */
    template<typename T2>
    void Unite(const BBox3D<T2> &i_bbox);

  public:
    // Public data members.
    Point3D<T> m_min, m_max;
  };

/**
* Returns BBox3D uniting the specified bounding box and 3D point.
*/
template<typename T1, typename T2>
BBox3D<T1> Union(const BBox3D<T1> &i_bbox1, const Point3D<T2> &i_point);

/**
* Returns BBox3D uniting two specified bounding boxes.
*/
template<typename T1, typename T2>
BBox3D<T1> Union(const BBox3D<T1> &i_bbox1, const BBox3D<T2> &i_bbox2);

/**
* Converts BBox3D instance to a BBox3D parameterized by a specified type.
*/
template<typename T2, typename T>
BBox3D<T2> Convert(const BBox3D<T> &i_bbox);

typedef BBox3D<float> BBox3D_f;
typedef BBox3D<double> BBox3D_d;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
BBox3D<T>::BBox3D():
m_min(Point3D<T>(std::numeric_limits<T>::infinity(),std::numeric_limits<T>::infinity(),std::numeric_limits<T>::infinity())),
m_max(Point3D<T>(-std::numeric_limits<T>::infinity(),-std::numeric_limits<T>::infinity(),-std::numeric_limits<T>::infinity()))
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
T BBox3D<T>::Area() const
  {
  T x = (T)fabs(m_max[0]-m_min[0]);
  T y = (T)fabs(m_max[1]-m_min[1]);
  T z = (T)fabs(m_max[2]-m_min[2]);
  return (T) 2.0*(x*y+x*z+y*z);
  }

template<typename T>
bool BBox3D<T>::Intersect(const Ray &i_ray, double *op_t_begin, double *op_t_end) const
  {
  double t0 = i_ray.m_min_t, t1 = i_ray.m_max_t;
  for (int i = 0; i < 3; ++i)
    {
    // Update interval for i-th bounding box slab.
    // No need to check for zero i_ray.m_direction[i] due to the way INF values behave.
    double invRayDir = 1.0 / i_ray.m_direction[i];
    double tNear = (m_min[i] - i_ray.m_origin[i]) * invRayDir;
    double tFar  = (m_max[i] - i_ray.m_origin[i]) * invRayDir;

    if (tNear > tFar) std::swap(tNear, tFar);
    t0 = tNear > t0 ? tNear : t0;
    t1 = tFar  < t1 ? tFar  : t1;
    if (t0 > t1) return false;
    }

  if (op_t_begin) *op_t_begin = t0;
  if (op_t_end) *op_t_end = t1;

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

template<typename T>
template<typename T2>
void BBox3D<T>::Unite(const Point3D<T2> &i_point)
  {
  m_min[0] = std::min(m_min[0], (T)i_point[0]);
  m_min[1] = std::min(m_min[1], (T)i_point[1]);
  m_min[2] = std::min(m_min[2], (T)i_point[2]);
  m_max[0] = std::max(m_max[0], (T)i_point[0]);
  m_max[1] = std::max(m_max[1], (T)i_point[1]);
  m_max[2] = std::max(m_max[2], (T)i_point[2]);
  }

template<typename T>
template<typename T2>
void BBox3D<T>::Unite(const BBox3D<T2> &i_bbox)
  {
  m_min[0] = std::min(m_min[0], (T)i_bbox.m_min[0]);
  m_min[1] = std::min(m_min[1], (T)i_bbox.m_min[1]);
  m_min[2] = std::min(m_min[2], (T)i_bbox.m_min[2]);
  m_max[0] = std::max(m_max[0], (T)i_bbox.m_max[0]);
  m_max[1] = std::max(m_max[1], (T)i_bbox.m_max[1]);
  m_max[2] = std::max(m_max[2], (T)i_bbox.m_max[2]);
  }

template<typename T1, typename T2>
BBox3D<T1> Union(const BBox3D<T1> &i_bbox1, const Point3D<T2> &i_point)
  {
  BBox3D<T1> ret;
  ret.m_min[0] = std::min(i_bbox1.m_min[0], (T1)i_point[0]);
  ret.m_min[1] = std::min(i_bbox1.m_min[1], (T1)i_point[1]);
  ret.m_min[2] = std::min(i_bbox1.m_min[2], (T1)i_point[2]);
  ret.m_max[0] = std::max(i_bbox1.m_max[0], (T1)i_point[0]);
  ret.m_max[1] = std::max(i_bbox1.m_max[1], (T1)i_point[1]);
  ret.m_max[2] = std::max(i_bbox1.m_max[2], (T1)i_point[2]);

  return ret;
  }

template<typename T1, typename T2>
BBox3D<T1> Union(const BBox3D<T1> &i_bbox1, const BBox3D<T2> &i_bbox2)
  {
  BBox3D<T1> ret;
  ret.m_min[0] = std::min(i_bbox1.m_min[0], (T1)i_bbox2.m_min[0]);
  ret.m_min[1] = std::min(i_bbox1.m_min[1], (T1)i_bbox2.m_min[1]);
  ret.m_min[2] = std::min(i_bbox1.m_min[2], (T1)i_bbox2.m_min[2]);
  ret.m_max[0] = std::max(i_bbox1.m_max[0], (T1)i_bbox2.m_max[0]);
  ret.m_max[1] = std::max(i_bbox1.m_max[1], (T1)i_bbox2.m_max[1]);
  ret.m_max[2] = std::max(i_bbox1.m_max[2], (T1)i_bbox2.m_max[2]);

  return ret;
  }

template<typename T2, typename T>
BBox3D<T2> Convert(const BBox3D<T> &i_bbox)
  {
  return BBox3D<T2>(Convert<T2>(i_bbox.m_min), Convert<T2>(i_bbox.m_max));
  }

#endif // BBOX3D_H