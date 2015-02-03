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

#ifndef POINT2D_H
#define POINT2D_H

#include <Common/Common.h>
#include "Constants.h"
#include <cmath>
#include <istream>

/**
* Class template for 2D point entity.
* The template parameter corresponds to the type of the point coordinates.
* @sa Point3D, Vector3D, Vector2D
*/
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

    Point2D<T> operator*(double i_value) const;
    Point2D<T> &operator*=(double i_value);

    Point2D<T> operator/(double i_value) const;
    Point2D<T> &operator/=(double i_value);

    bool operator==(const Point2D<T> &i_point) const;
    bool operator!=(const Point2D<T> &i_point) const;

    T operator[](size_t i_index) const;
    T &operator[](size_t i_index);

  private:
    T m_coordinates[2];
  };

template<typename T>
Point2D<T> operator*(T i_value, const Point2D<T> &i_point);

/**
* Reads Point2D from the input stream.
*/
template <class charT, class traits, typename T>
std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, Point2D<T> &o_point);

/**
* Prints Point2D to the output stream.
*/
template <class charT, class traits, typename T>
std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const Point2D<T> &i_point);

/**
* Converts Point2D instance to a Point2D parameterized by a specified type.
*/
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
  ASSERT(IsNaN(i_x)==false && IsNaN(i_y)==false);

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
Point2D<T> Point2D<T>::operator*(double i_value) const
  {
  return Point2D<T>(
    (T) (m_coordinates[0]*i_value), 
    (T) (m_coordinates[1]*i_value));
  }

template<typename T>
Point2D<T> &Point2D<T>::operator*=(double i_value)
  {
  m_coordinates[0]=(T)(m_coordinates[0]*i_value);
  m_coordinates[1]=(T)(m_coordinates[1]*i_value);

  return *this;
  }

template<typename T>
Point2D<T> Point2D<T>::operator/(double i_value) const
  {
  //Dividing by zero is considered correct. The caller code is responsible to handle resulting INF values properly.
  double inv = 1.0 / i_value;
  return (*this) * inv;
  }

template<typename T>
Point2D<T> &Point2D<T>::operator/=(double i_value)
  {
  //Dividing by zero is considered correct. The caller code is responsible to handle resulting INF values properly.
  double inv = 1.0 / i_value;
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
T Point2D<T>::operator[](size_t i_index) const
  {
  ASSERT(i_index<2);
  return m_coordinates[i_index];
  }

template<typename T>
T &Point2D<T>::operator[](size_t i_index)
  {
  ASSERT(i_index<2);
  return m_coordinates[i_index];
  }

template<typename T>
Point2D<T> operator*(T i_value, const Point2D<T> &i_point)
  {
  return i_point*i_value;
  }

template <class charT, class traits, typename T>
inline std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, Point2D<T> &o_point)
  {
  i_stream >> o_point[0] >> o_point[1];
  return i_stream;
  }

template <class charT, class traits, typename T>
inline std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const Point2D<T> &i_point)
  {
  /* string stream
  * - with same format
  * - without special field width
  */
  std::basic_ostringstream<charT,traits> s;
  s.copyfmt(o_stream);
  s.width(0);

  s << i_point[0] << ' ' << i_point[1];
  o_stream << s.str();
  return o_stream;
  }

template<typename T2, typename T>
Point2D<T2> Convert(const Point2D<T> &i_point)
  {
  return Point2D<T2>((T2)i_point[0], (T2)i_point[1]);
  }

#endif // POINT2D_H