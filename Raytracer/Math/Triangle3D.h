#ifndef TRIANGLE3D_H
#define TRIANGLE3D_H

#include <Common\Common.h>
#include "Point3D.h"
#include "Vector3D.h"

template<typename T>
class Triangle3D
  {
  public:
    Triangle3D();
    Triangle3D(const Point3D<T> &i_vertex1, const Point3D<T> &i_vertex2, const Point3D<T> &i_vertex3);

    Vector3Dd GetNormal() const;
    double GetArea() const;

    const Point3D<T> &operator[](unsigned char i_index) const;
    Point3D<T> &operator[](unsigned char i_index);

  private:
    Point3D<T> m_vertices[3];
  };

typedef Triangle3D<float> Triangle3Df;
typedef Triangle3D<double> Triangle3Dd;

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
Vector3Dd Triangle3D<T>::GetNormal() const
  {
  Vector3Dd normal = Vector3Dd(m_vertices[1]-m_vertices[0])^Vector3Dd(m_vertices[2]-m_vertices[0]);
  normal.Normalize();
  return normal;
  }

template<typename T>
double Triangle3D<T>::GetArea() const
  {
  Vector3Dd cross = Vector3Dd(m_vertices[1]-m_vertices[0]) ^ Vector3Dd(m_vertices[2]-m_vertices[0]);
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