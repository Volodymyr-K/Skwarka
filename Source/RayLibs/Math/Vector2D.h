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

#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <Common/Common.h>
#include "Point2D.h"
#include "Constants.h"
#include <cmath>
#include <istream>

/**
* Class template for 2D vector entity.
* The template parameter corresponds to the type of the vector coordinates.
* @sa Vector3D, Point3D, Point2D
*/
template<typename T>
class Vector2D
  {
  public:
    Vector2D();
    Vector2D(T i_x, T i_y);
    explicit Vector2D<T>(const Point2D<T> &i_point);

    Vector2D<T> operator+(const Vector2D<T> &i_vector) const;
    Vector2D<T> &operator+=(const Vector2D<T> &i_vector);

    Vector2D<T> operator-(const Vector2D<T> &i_vector) const;
    Vector2D<T> &operator-=(const Vector2D<T> &i_vector);

    Vector2D<T> operator*(double i_value) const;
    Vector2D<T> &operator*=(double i_value);

    Vector2D<T> operator/(double i_value) const;
    Vector2D<T> &operator/=(double i_value);

    bool operator==(const Vector2D<T> &i_vector) const;
    bool operator!=(const Vector2D<T> &i_vector) const;

    /**
    * Vector dot product.
    */
    T operator*(const Vector2D<T> &i_vector) const;

    /**
    * Vector cross product.
    */
    T operator^(const Vector2D<T> &i_vector) const;

    T Length() const;
    T LengthSqr() const;

    /**
    * Normalizes the vector.
    * If the vector has zero length it is not changed.
    * @return true if the vector was normalized and false if it had zero length.
    */
    bool Normalize();

    /**
    * Returns a normalized vector.
    * If the vector has zero length the zero vector is returned.
    */
    Vector2D<T> Normalized() const;

    /**
    * Returns true if the vector is normalized.
    */
    bool IsNormalized() const;

    T operator[](size_t i_index) const;
    T &operator[](size_t i_index);
  private:
    T m_coordinates[2];
  };

template<typename T>
Point2D<T> operator+(const Point2D<T> &i_point, const Vector2D<T> &i_vector);

template<typename T>
Point2D<T> operator-(const Point2D<T> &i_point, const Vector2D<T> &i_vector);

template<typename T>
Vector2D<T> operator*(T i_value, const Vector2D<T> &i_vector);

/**
* Reads Vector2D from the input stream.
*/
template <class charT, class traits, typename T>
std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, Vector2D<T> &o_vector);

/**
* Prints Vector2D to the output stream.
*/
template <class charT, class traits, typename T>
std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const Vector2D<T> &i_vector);

/**
* Converts Vector2D instance to a Vector2D parameterized by a specified type.
*/
template<typename T2, typename T>
Vector2D<T2> Convert(const Vector2D<T> &i_vector);

typedef Vector2D<int> Vector2D_i;
typedef Vector2D<float> Vector2D_f;
typedef Vector2D<double> Vector2D_d;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
Vector2D<T>::Vector2D()
  {
  m_coordinates[0]=0;
  m_coordinates[1]=0;
  }

template<typename T>
Vector2D<T>::Vector2D(T i_x, T i_y)
  {
  ASSERT(IsNaN(i_x)==false && IsNaN(i_y)==false);

  m_coordinates[0]=i_x;
  m_coordinates[1]=i_y;
  }

template<typename T>
Vector2D<T>::Vector2D(const Point2D<T> &i_point)
  {
  m_coordinates[0]=i_point[0];
  m_coordinates[1]=i_point[1];
  }

template<typename T>
Vector2D<T> Vector2D<T>::operator+(const Vector2D<T> &i_vector) const
  {
  return Vector2D<T>(m_coordinates[0]+i_vector.m_coordinates[0], m_coordinates[1]+i_vector.m_coordinates[1]);
  }

template<typename T>
Vector2D<T> &Vector2D<T>::operator+=(const Vector2D<T> &i_vector)
  {
  m_coordinates[0]+=i_vector.m_coordinates[0];
  m_coordinates[1]+=i_vector.m_coordinates[1];

  return *this;
  }

template<typename T>
Vector2D<T> Vector2D<T>::operator-(const Vector2D<T> &i_vector) const
  {
  return Vector2D<T>(m_coordinates[0]-i_vector.m_coordinates[0], m_coordinates[1]-i_vector.m_coordinates[1]);
  }

template<typename T>
Vector2D<T> &Vector2D<T>::operator-=(const Vector2D<T> &i_vector)
  {
  m_coordinates[0]-=i_vector.m_coordinates[0];
  m_coordinates[1]-=i_vector.m_coordinates[1];

  return *this;
  }

template<typename T>
Vector2D<T> Vector2D<T>::operator*(double i_value) const
  {
  return Vector2D<T>(
    (T) (m_coordinates[0]*i_value), 
    (T) (m_coordinates[1]*i_value));
  }

template<typename T>
Vector2D<T> &Vector2D<T>::operator*=(double i_value)
  {
  m_coordinates[0]=(T)(m_coordinates[0]*i_value);
  m_coordinates[1]=(T)(m_coordinates[1]*i_value);

  return *this;
  }

template<typename T>
Vector2D<T> Vector2D<T>::operator/(double i_value) const
  {
  //Dividing by zero is considered correct. The caller code is responsible to handle resulting INF values properly.
  double inv = 1.0 / i_value;
  return (*this) * inv;
  }

template<typename T>
Vector2D<T> &Vector2D<T>::operator/=(double i_value)
  {
  //Dividing by zero is considered correct. The caller code is responsible to handle resulting INF values properly.
  double inv = 1.0 / i_value;
  (*this)*=inv;
  return *this;
  }

template<typename T>
bool Vector2D<T>::operator==(const Vector2D<T> &i_vector) const
  {
  if (m_coordinates[0] != i_vector.m_coordinates[0]) return false;
  if (m_coordinates[1] != i_vector.m_coordinates[1]) return false;
  return true;
  }

template<typename T>
bool Vector2D<T>::operator!=(const Vector2D<T> &i_vector) const
  {
  return !operator==(i_vector);
  }

template<typename T>
T Vector2D<T>::operator*(const Vector2D<T> &i_vector) const
  {
  return m_coordinates[0]*i_vector[0]+m_coordinates[1]*i_vector[1];
  }

template<typename T>
T Vector2D<T>::operator^(const Vector2D<T> &i_vector) const
  {
  return m_coordinates[0]*i_vector.m_coordinates[1]-m_coordinates[1]*i_vector.m_coordinates[0];
  }

template<typename T>
T Vector2D<T>::Length() const
  {
  return std::sqrt(m_coordinates[0]*m_coordinates[0]+m_coordinates[1]*m_coordinates[1]);
  }

template<typename T>
T Vector2D<T>::LengthSqr() const
  {
  return m_coordinates[0]*m_coordinates[0]+m_coordinates[1]*m_coordinates[1];
  }

template<typename T>
bool Vector2D<T>::Normalize()
  {
  T length_sqr = LengthSqr();
  if(length_sqr > 0.0)
    {
    (*this)/=std::sqrt(length_sqr);
    return true;
    }
  else
    return false;
  }

template<typename T>
Vector2D<T> Vector2D<T>::Normalized() const
  {
  Vector2D<T> tmp(*this);
  tmp.Normalize();
  return tmp;
  }

template<typename T>
bool Vector2D<T>::IsNormalized() const
  {
  return fabs(LengthSqr()-1.0)<DBL_3D_EPS;
  }

template<typename T>
T Vector2D<T>::operator[](size_t i_index) const
  {
  ASSERT(i_index<2);
  return m_coordinates[i_index];
  }

template<typename T>
T &Vector2D<T>::operator[](size_t i_index)
  {
  ASSERT(i_index<2);
  return m_coordinates[i_index];
  }

template<typename T>
Point2D<T> operator+(const Point2D<T> &i_point, const Vector2D<T> &i_vector)
  {
  return Point2D<T>(i_point[0]+i_vector[0], i_point[1]+i_vector[1]);
  }

template<typename T>
Point2D<T> operator-(const Point2D<T> &i_point, const Vector2D<T> &i_vector)
  {
  return Point2D<T>(i_point[0]-i_vector[0], i_point[1]-i_vector[1]);
  }

template<typename T>
Vector2D<T> operator*(T i_value, const Vector2D<T> &i_vector)
  {
  return i_vector*i_value;
  }

template <class charT, class traits, typename T>
std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, Vector2D<T> &o_vector)
  {
  i_stream >> o_vector[0] >> o_vector[1];
  return i_stream;
  }

template <class charT, class traits, typename T>
inline std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const Vector2D<T> &i_vector)
  {
  /* string stream
  * - with same format
  * - without special field width
  */
  std::basic_ostringstream<charT,traits> s;
  s.copyfmt(o_stream);
  s.width(0);

  s << i_vector[0] << ' ' << i_vector[1];
  o_stream << s.str();
  return o_stream;
  }

template<typename T2, typename T>
Vector2D<T2> Convert(const Vector2D<T> &i_vector)
  {
  return Vector2D<T2>((T2)i_vector[0], (T2)i_vector[1]);
  }

#endif // VECTOR2D_H