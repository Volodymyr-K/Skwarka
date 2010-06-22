#ifndef POINT3D_H
#define POINT3D_H

#include <Common/Common.h>
#include "Constants.h"
#include <cmath>
#include <istream>

/**
* Class template for 3D point entity.
* The template parameter corresponds to the type of the point coordinates.
* @sa Point2D, Vector3D, Vector2D
*/
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

    Point3D<T> operator*(double i_value) const;
    Point3D<T> &operator*=(double i_value);

    Point3D<T> operator/(double i_value) const;
    Point3D<T> &operator/=(double i_value);

    bool operator==(const Point3D<T> &i_point) const;
    bool operator!=(const Point3D<T> &i_point) const;

    T operator[](unsigned char i_index) const;
    T &operator[](unsigned char i_index);

  private:
    T m_coordinates[3];
  };

template<typename T>
Point3D<T> operator*(T i_value, const Point3D<T> &i_point);

/**
* Reads Point3D from the input stream.
*/
template <class charT, class traits, typename T>
std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, Point3D<T> &o_point);

/**
* Prints Point3D to the output stream.
*/
template <class charT, class traits, typename T>
std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const Point3D<T> &i_point);

/**
* Converts Point3D instance to a Point3D parameterized by a specified type.
*/
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
  ASSERT(IsNaN(i_x)==false && IsNaN(i_y)==false && IsNaN(i_z)==false);

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
Point3D<T> Point3D<T>::operator*(double i_value) const
  {
  return Point3D<T>(
    (T) (m_coordinates[0]*i_value), 
    (T) (m_coordinates[1]*i_value), 
    (T) (m_coordinates[2]*i_value));
  }

template<typename T>
Point3D<T> &Point3D<T>::operator*=(double i_value)
  {
  m_coordinates[0]=(T)(m_coordinates[0]*i_value);
  m_coordinates[1]=(T)(m_coordinates[1]*i_value);
  m_coordinates[2]=(T)(m_coordinates[2]*i_value);

  return *this;
  }

template<typename T>
Point3D<T> Point3D<T>::operator/(double i_value) const
  {
  //Dividing by zero is considered correct. The caller code is responsible to handle resulting INF values properly.
  double inv = 1.0 / i_value;
  return (*this) * inv;
  }

template<typename T>
Point3D<T> &Point3D<T>::operator/=(double i_value)
  {
  //Dividing by zero is considered correct. The caller code is responsible to handle resulting INF values properly.
  double inv = 1.0 / i_value;
  (*this)*=inv;
  return *this;
  }

template<typename T>
bool Point3D<T>::operator==(const Point3D<T> &i_point) const
  {
  if (m_coordinates[0] != i_point.m_coordinates[0]) return false;
  if (m_coordinates[1] != i_point.m_coordinates[1]) return false;
  if (m_coordinates[2] != i_point.m_coordinates[2]) return false;
  return true;
  }

template<typename T>
bool Point3D<T>::operator!=(const Point3D<T> &i_point) const
  {
  return !operator==(i_point);
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

template <class charT, class traits, typename T>
inline std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, Point3D<T> &o_point)
  {
  i_stream >> o_point[0] >> o_point[1] >> o_point[2];
  return i_stream;
  }

template <class charT, class traits, typename T>
inline std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const Point3D<T> &i_point)
  {
  /* string stream
  * - with same format
  * - without special field width
  */
  std::basic_ostringstream<charT,traits> s;
  s.copyfmt(o_stream);
  s.width(0);

  s << i_point[0] << ' ' << i_point[1] << ' ' << i_point[2];
  o_stream << s.str();
  return o_stream;
  }

template<typename T2, typename T>
Point3D<T2> Convert(const Point3D<T> &i_point)
  {
  return Point3D<T2>((T2)i_point[0], (T2)i_point[1], (T2)i_point[2]);
  }

#endif // POINT3D_H