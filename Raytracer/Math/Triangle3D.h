#ifndef TRIANGLE3D_H
#define TRIANGLE3D_H

#include <Common/Common.h>
#include "Point3D.h"
#include "Vector3D.h"

/**
* Class template for 3D triangle entity.
* The template parameter corresponds to the type of the vertices's coordinates.
* Triangle is represented by a (ordered) triple of Point3D. The order of the vertices defines the orientation.
* @sa Point3D
*/
template<typename T>
class Triangle3D
  {
  public:
    Triangle3D();
    Triangle3D(const Point3D<T> &i_vertex1, const Point3D<T> &i_vertex2, const Point3D<T> &i_vertex3);

    /**
    * Returns triangle's normal. The orientation depends on the order of triangle's vertices.
    * The method does not cache the normal value so it is computed each time the method is called.
    */
    Vector3D_d GetNormal() const;

    /**
    * Computes triangle's area.
    */
    double GetArea() const;

    const Point3D<T> &operator[](unsigned char i_index) const;
    Point3D<T> &operator[](unsigned char i_index);

  private:
    Point3D<T> m_vertices[3];
  };

typedef Triangle3D<float> Triangle3D_f;
typedef Triangle3D<double> Triangle3D_d;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
Triangle3D<T>::Triangle3D()
  {
  }

template<typename T>
Triangle3D<T>::Triangle3D(const Point3D<T> &i_vertex1, const Point3D<T> &i_vertex2, const Point3D<T> &i_vertex3)
  {
  m_vertices[0]=i_vertex1;
  m_vertices[1]=i_vertex2;
  m_vertices[2]=i_vertex3;
  }

template<typename T>
Vector3D_d Triangle3D<T>::GetNormal() const
  {
  Vector3D_d normal = Vector3D_d(m_vertices[1]-m_vertices[0])^Vector3D_d(m_vertices[2]-m_vertices[0]);
  normal.Normalize();
  return normal;
  }

template<typename T>
double Triangle3D<T>::GetArea() const
  {
  Vector3D_d cross = Vector3D_d(m_vertices[1]-m_vertices[0]) ^ Vector3D_d(m_vertices[2]-m_vertices[0]);
  return 0.5*cross.Length();
  }

template<typename T>
const Point3D<T> &Triangle3D<T>::operator[](unsigned char i_index) const
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_vertices[i_index];
  }

template<typename T>
Point3D<T> &Triangle3D<T>::operator[](unsigned char i_index)
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_vertices[i_index];
  }

#endif // TRIANGLE3D_H