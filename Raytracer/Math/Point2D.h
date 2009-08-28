#ifndef POINT2D_H
#define POINT2D_H

#include <Common\Common.h>
#include "Constants.h"
#include <cmath>

template<typename T>
class Point2D
  {
  public:
    Point2D();
    Point2D(T i_x, T i_y);

    Point2D<T> operator+(const Point2D<T> &i_point) const;
    Point2D<T> &operator+=(const Point2D<T> &i_point);

    Point2D<T> operator-(const Point2D<T> &i_point) const;
    Point2D<T> &operator-=(const Point2D<T> &i_point);

    Point2D<T> operator*(T i_value) const;
    Point2D<T> &operator*=(T i_value);

    Point2D<T> operator/(T i_value) const;
    Point2D<T> &operator/=(T i_value);

    bool operator==(const Point2D<T> &i_point) const;
    bool operator!=(const Point2D<T> &i_point) const;

    T operator[](unsigned char i_index) const;
    T &operator[](unsigned char i_index);

  private:
    T m_coordinates[2];
  };

template<typename T>
Point2D<T> operator*(T i_value, const Point2D<T> &i_point);

template<typename T2, typename T>
Point2D<T2> Convert(const Point2D<T> &i_point);

typedef Point2D<int> Point2D_i;
typedef Point2D<float> Point2D_f;
typedef Point2D<double> Point2D_d;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
Point2D<T>::Point2D()
  {
  m_coordinates[0]=0;
  m_coordinates[1]=0;
  }

template<typename T>
Point2D<T>::Point2D(T i_x, T i_y)
  {
  m_coordinates[0]=i_x;
  m_coordinates[1]=i_y;
  }

template<typename T>
Point2D<T> Point2D<T>::operator+(const Point2D<T> &i_point) const
  {
  return Point2D<T>(m_coordinates[0]+i_point.m_coordinates[0], m_coordinates[1]+i_point.m_coordinates[1]);
  }

template<typename T>
Point2D<T> &Point2D<T>::operator+=(const Point2D<T> &i_point)
  {
  m_coordinates[0]+=i_point.m_coordinates[0];
  m_coordinates[1]+=i_point.m_coordinates[1];

  return *this;
  }

template<typename T>
Point2D<T> Point2D<T>::operator-(const Point2D<T> &i_point) const
  {
  return Point2D<T>(m_coordinates[0]-i_point.m_coordinates[0], m_coordinates[1]-i_point.m_coordinates[1]);
  }

template<typename T>
Point2D<T> &Point2D<T>::operator-=(const Point2D<T> &i_point)
  {
  m_coordinates[0]-=i_point.m_coordinates[0];
  m_coordinates[1]-=i_point.m_coordinates[1];

  return *this;
  }

template<typename T>
Point2D<T> Point2D<T>::operator*(T i_value) const
  {
  return Point2D<T>(m_coordinates[0]*i_value, m_coordinates[1]*i_value);
  }

template<typename T>
Point2D<T> &Point2D<T>::operator*=(T i_value)
  {
  m_coordinates[0]*=i_value;
  m_coordinates[1]*=i_value;

  return *this;
  }

template<typename T>
Point2D<T> Point2D<T>::operator/(T i_value) const
  {
  ASSERT(fabs(i_value) > (T)0);

  T inv = (T)1.0 / i_value;
  return (*this) * inv;
  }

template<typename T>
Point2D<T> &Point2D<T>::operator/=(T i_value)
  {
  ASSERT(fabs(i_value) > (T)0);

  T inv = (T)1.0 / i_value;
  (*this)*=inv;
  return *this;
  }

template<typename T>
bool Point2D<T>::operator==(const Point2D<T> &i_point) const
  {
  if (m_coordinates[0] != i_point.m_coordinates[0]) return false;
  if (m_coordinates[1] != i_point.m_coordinates[1]) return false;
  return true;
  }

template<typename T>
bool Point2D<T>::operator!=(const Point2D<T> &i_point) const
  {
  return !operator==(i_point);
  }

template<typename T>
T Point2D<T>::operator[](unsigned char i_index) const
  {
  ASSERT(i_index>=0 && i_index<2);
  return m_coordinates[i_index];
  }

template<typename T>
T &Point2D<T>::operator[](unsigned char i_index)
  {
  ASSERT(i_index>=0 && i_index<2);
  return m_coordinates[i_index];
  }

template<typename T>
Point2D<T> operator*(T i_value, const Point2D<T> &i_point)
  {
  return i_point*i_value;
  }

template<typename T2, typename T>
Point2D<T2> Convert(const Point2D<T> &i_point)
  {
  return Point2D<T2>((T2)i_point[0], (T2)i_point[1]);
  }

#endif // POINT2D_H