#ifndef POINT3D_H
#define POINT3D_H

#include <Common\Common.h>
#include "Constants.h"
#include <cmath>

template<typename T>
class Point3D
  {
  public:
    Point3D();
    Point3D(T i_x, T i_y, T i_z);

    Point3D<T> operator+(const Point3D<T> &i_point) const;
    Point3D<T> &operator+=(const Point3D<T> &i_point);

    Point3D<T> operator-(const Point3D<T> &i_point) const;
    Point3D<T> &operator-=(const Point3D<T> &i_point);

    Point3D<T> operator*(T i_value) const;
    Point3D<T> &operator*=(T i_value);

    Point3D<T> operator/(T i_value) const;
    Point3D<T> &operator/=(T i_value);

    T operator[](unsigned char i_index) const;
    T &operator[](unsigned char i_index);

  private:
    T m_coordinates[3];
  };

template<typename T>
Point3D<T> operator*(T i_value, const Point3D<T> &i_point);

template<typename T2, typename T>
Point3D<T2> Convert(const Point3D<T> &i_point);

typedef Point3D<float> Point3D_f;
typedef Point3D<double> Point3D_d;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
Point3D<T>::Point3D()
  {
  m_coordinates[0]=0;
  m_coordinates[1]=0;
  m_coordinates[2]=0;
  }

template<typename T>
Point3D<T>::Point3D(T i_x, T i_y, T i_z)
  {
  m_coordinates[0]=i_x;
  m_coordinates[1]=i_y;
  m_coordinates[2]=i_z;
  }

template<typename T>
Point3D<T> Point3D<T>::operator+(const Point3D<T> &i_point) const
  {
  return Point3D<T>(m_coordinates[0]+i_point.m_coordinates[0], m_coordinates[1]+i_point.m_coordinates[1], m_coordinates[2]+i_point.m_coordinates[2]);
  }

template<typename T>
Point3D<T> &Point3D<T>::operator+=(const Point3D<T> &i_point)
  {
  m_coordinates[0]+=i_point.m_coordinates[0];
  m_coordinates[1]+=i_point.m_coordinates[1];
  m_coordinates[2]+=i_point.m_coordinates[2];

  return *this;
  }

template<typename T>
Point3D<T> Point3D<T>::operator-(const Point3D<T> &i_point) const
  {
  return Point3D<T>(m_coordinates[0]-i_point.m_coordinates[0], m_coordinates[1]-i_point.m_coordinates[1], m_coordinates[2]-i_point.m_coordinates[2]);
  }

template<typename T>
Point3D<T> &Point3D<T>::operator-=(const Point3D<T> &i_point)
  {
  m_coordinates[0]-=i_point.m_coordinates[0];
  m_coordinates[1]-=i_point.m_coordinates[1];
  m_coordinates[2]-=i_point.m_coordinates[2];

  return *this;
  }

template<typename T>
Point3D<T> Point3D<T>::operator*(T i_value) const
  {
  return Point3D<T>(m_coordinates[0]*i_value, m_coordinates[1]*i_value, m_coordinates[2]*i_value);
  }

template<typename T>
Point3D<T> &Point3D<T>::operator*=(T i_value)
  {
  m_coordinates[0]*=i_value;
  m_coordinates[1]*=i_value;
  m_coordinates[2]*=i_value;

  return *this;
  }

template<typename T>
Point3D<T> Point3D<T>::operator/(T i_value) const
  {
  ASSERT(fabs(i_value) > (T)0);

  T inv = (T)1.0 / i_value;
  return (*this) * inv;
  }

template<typename T>
Point3D<T> &Point3D<T>::operator/=(T i_value)
  {
  ASSERT(fabs(i_value) > (T)0);

  T inv = (T)1.0 / i_value;
  (*this)*=inv;
  return *this;
  }

template<typename T>
T Point3D<T>::operator[](unsigned char i_index) const
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_coordinates[i_index];
  }

template<typename T>
T &Point3D<T>::operator[](unsigned char i_index)
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_coordinates[i_index];
  }

template<typename T>
Point3D<T> operator*(T i_value, const Point3D<T> &i_point)
  {
  return i_point*i_value;
  }

template<typename T2, typename T>
Point3D<T2> Convert(const Point3D<T> &i_point)
  {
  return Point3D<T2>((T2)i_point[0], (T2)i_point[1], (T2)i_point[2]);
  }

#endif // POINT3D_H