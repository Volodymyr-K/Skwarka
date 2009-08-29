#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <Common\Common.h>
#include "Point3D.h"
#include "Constants.h"
#include <cmath>

template<typename T>
class Vector3D
  {
  public:
    Vector3D();
    Vector3D(T i_x, T i_y, T i_z);
    explicit Vector3D<T>(const Point3D<T> &i_point);

    Vector3D<T> operator+(const Vector3D<T> &i_vector) const;
    Vector3D<T> &operator+=(const Vector3D<T> &i_vector);

    Vector3D<T> operator-(const Vector3D<T> &i_vector) const;
    Vector3D<T> &operator-=(const Vector3D<T> &i_vector);

    Vector3D<T> operator*(T i_value) const;
    Vector3D<T> &operator*=(T i_value);
    
    Vector3D<T> operator/(T i_value) const;
    Vector3D<T> &operator/=(T i_value);

    bool operator==(const Vector3D<T> &i_vector) const;
    bool operator!=(const Vector3D<T> &i_vector) const;

    T operator*(const Vector3D<T> &i_vector) const;
    Vector3D<T> operator^(const Vector3D<T> &i_vector) const;

    T Length() const;
    T LengthSqr() const;

    void Normalize();
    Vector3D<T> Normalized() const;

    T operator[](unsigned char i_index) const;
    T &operator[](unsigned char i_index);
  private:
    T m_coordinates[3];
  };

template<typename T>
Point3D<T> operator+(const Point3D<T> &i_point, const Vector3D<T> &i_vector);

template<typename T>
Point3D<T> operator-(const Point3D<T> &i_point, const Vector3D<T> &i_vector);

template<typename T>
Vector3D<T> operator*(T i_value, const Vector3D<T> &i_vector);

template<typename T2, typename T>
Vector3D<T2> Convert(const Vector3D<T> &i_vector);

typedef Vector3D<float> Vector3D_f;
typedef Vector3D<double> Vector3D_d;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
Vector3D<T>::Vector3D()
  {
  m_coordinates[0]=0;
  m_coordinates[1]=0;
  m_coordinates[2]=0;
  }

template<typename T>
Vector3D<T>::Vector3D(T i_x, T i_y, T i_z)
  {
  m_coordinates[0]=i_x;
  m_coordinates[1]=i_y;
  m_coordinates[2]=i_z;
  }

template<typename T>
Vector3D<T>::Vector3D(const Point3D<T> &i_point)
  {
  m_coordinates[0]=i_point[0];
  m_coordinates[1]=i_point[1];
  m_coordinates[2]=i_point[2];
  }

template<typename T>
Vector3D<T> Vector3D<T>::operator+(const Vector3D<T> &i_vector) const
  {
  return Vector3D<T>(m_coordinates[0]+i_vector.m_coordinates[0], m_coordinates[1]+i_vector.m_coordinates[1], m_coordinates[2]+i_vector.m_coordinates[2]);
  }

template<typename T>
Vector3D<T> &Vector3D<T>::operator+=(const Vector3D<T> &i_vector)
  {
  m_coordinates[0]+=i_vector.m_coordinates[0];
  m_coordinates[1]+=i_vector.m_coordinates[1];
  m_coordinates[2]+=i_vector.m_coordinates[2];

  return *this;
  }

template<typename T>
Vector3D<T> Vector3D<T>::operator-(const Vector3D<T> &i_vector) const
  {
  return Vector3D<T>(m_coordinates[0]-i_vector.m_coordinates[0], m_coordinates[1]-i_vector.m_coordinates[1], m_coordinates[2]-i_vector.m_coordinates[2]);
  }

template<typename T>
Vector3D<T> &Vector3D<T>::operator-=(const Vector3D<T> &i_vector)
  {
  m_coordinates[0]-=i_vector.m_coordinates[0];
  m_coordinates[1]-=i_vector.m_coordinates[1];
  m_coordinates[2]-=i_vector.m_coordinates[2];

  return *this;
  }

template<typename T>
Vector3D<T> Vector3D<T>::operator*(T i_value) const
  {
  return Vector3D<T>(m_coordinates[0]*i_value, m_coordinates[1]*i_value, m_coordinates[2]*i_value);
  }

template<typename T>
Vector3D<T> &Vector3D<T>::operator*=(T i_value)
  {
  m_coordinates[0]*=i_value;
  m_coordinates[1]*=i_value;
  m_coordinates[2]*=i_value;

  return *this;
  }

template<typename T>
Vector3D<T> Vector3D<T>::operator/(T i_value) const
  {
  ASSERT(fabs(i_value) > (T)0);

  T inv = (T)1.0 / i_value;
  return (*this) * inv;
  }

template<typename T>
Vector3D<T> &Vector3D<T>::operator/=(T i_value)
  {
  ASSERT(fabs(i_value) > (T)0);

  T inv = (T)1.0 / i_value;
  (*this)*=inv;
  return *this;
  }

template<typename T>
bool Vector3D<T>::operator==(const Vector3D<T> &i_vector) const
  {
  if (m_coordinates[0] != i_vector.m_coordinates[0]) return false;
  if (m_coordinates[1] != i_vector.m_coordinates[1]) return false;
  if (m_coordinates[2] != i_vector.m_coordinates[2]) return false;
  return true;
  }

template<typename T>
bool Vector3D<T>::operator!=(const Vector3D<T> &i_vector) const
  {
  return !operator==(i_vector);
  }

template<typename T>
T Vector3D<T>::operator*(const Vector3D<T> &i_vector) const
  {
  return m_coordinates[0]*i_vector[0]+m_coordinates[1]*i_vector[1]+m_coordinates[2]*i_vector[2];
  }

template<typename T>
Vector3D<T> Vector3D<T>::operator^(const Vector3D<T> &i_vector) const
  {
  return Vector3D<T>(
    m_coordinates[1]*i_vector.m_coordinates[2]-m_coordinates[2]*i_vector.m_coordinates[1],
    m_coordinates[2]*i_vector.m_coordinates[0]-m_coordinates[0]*i_vector.m_coordinates[2],
    m_coordinates[0]*i_vector.m_coordinates[1]-m_coordinates[1]*i_vector.m_coordinates[0]);
  }

template<typename T>
T Vector3D<T>::Length() const
  {
  return std::sqrt(m_coordinates[0]*m_coordinates[0]+m_coordinates[1]*m_coordinates[1]+m_coordinates[2]*m_coordinates[2]);
  }

template<typename T>
T Vector3D<T>::LengthSqr() const
  {
  return m_coordinates[0]*m_coordinates[0]+m_coordinates[1]*m_coordinates[1]+m_coordinates[2]*m_coordinates[2];
  }

template<typename T>
void Vector3D<T>::Normalize()
  {
  T length = Length();
  if(length > 0.0)
    (*this)/=length;
  }

template<typename T>
Vector3D<T> Vector3D<T>::Normalized() const
  {
  Vector3D<T> tmp(*this);
  tmp.Normalize();
  return tmp;
  }

template<typename T>
T Vector3D<T>::operator[](unsigned char i_index) const
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_coordinates[i_index];
  }

template<typename T>
T &Vector3D<T>::operator[](unsigned char i_index)
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_coordinates[i_index];
  }

template<typename T>
Point3D<T> operator+(const Point3D<T> &i_point, const Vector3D<T> &i_vector)
  {
  return Point3D<T>(i_point[0]+i_vector[0], i_point[1]+i_vector[1], i_point[2]+i_vector[2]);
  }

template<typename T>
Point3D<T> operator-(const Point3D<T> &i_point, const Vector3D<T> &i_vector)
  {
  return Point3D<T>(i_point[0]-i_vector[0], i_point[1]-i_vector[1], i_point[2]-i_vector[2]);
  }

template<typename T>
Vector3D<T> operator*(T i_value, const Vector3D<T> &i_vector)
  {
  return i_vector*i_value;
  }

template<typename T2, typename T>
Vector3D<T2> Convert(const Vector3D<T> &i_vector)
  {
  return Vector3D<T2>((T2)i_vector[0], (T2)i_vector[1], (T2)i_vector[2]);
  }

#endif // VECTOR3D_H