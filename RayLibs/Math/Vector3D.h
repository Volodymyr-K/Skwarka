#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <Common/Common.h>
#include "Point3D.h"
#include "Constants.h"
#include <cmath>
#include <istream>

/**
* Class template for 3D vector entity.
* The template parameter corresponds to the type of the vector coordinates.
* @sa Vector2D, Point3D, Point2D
*/
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

    Vector3D<T> operator*(double i_value) const;
    Vector3D<T> &operator*=(double i_value);

    Vector3D<T> operator/(double i_value) const;
    Vector3D<T> &operator/=(double i_value);

    bool operator==(const Vector3D<T> &i_vector) const;
    bool operator!=(const Vector3D<T> &i_vector) const;

    /**
    * Vector dot product.
    */
    T operator*(const Vector3D<T> &i_vector) const;

    /**
    * Vector cross product.
    */
    Vector3D<T> operator^(const Vector3D<T> &i_vector) const;

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
    Vector3D<T> Normalized() const;

    /**
    * Returns true if the vector is normalized.
    */
    bool IsNormalized() const;

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

/**
* Reads Vector3D from the input stream.
*/
template <class charT, class traits, typename T>
std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, Vector3D<T> &o_vector);

/**
* Prints Vector3D to the output stream.
*/
template <class charT, class traits, typename T>
std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const Vector3D<T> &i_vector);

/**
* Converts Vector3D instance to a Vector3D parameterized by a specified type.
*/
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
  ASSERT(IsNaN(i_x)==false && IsNaN(i_y)==false && IsNaN(i_z)==false);

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
Vector3D<T> Vector3D<T>::operator*(double i_value) const
  {
  return Vector3D<T>(
    (T) (m_coordinates[0]*i_value), 
    (T) (m_coordinates[1]*i_value), 
    (T) (m_coordinates[2]*i_value));
  }

template<typename T>
Vector3D<T> &Vector3D<T>::operator*=(double i_value)
  {
  m_coordinates[0]=(T)(m_coordinates[0]*i_value);
  m_coordinates[1]=(T)(m_coordinates[1]*i_value);
  m_coordinates[2]=(T)(m_coordinates[2]*i_value);

  return *this;
  }

template<typename T>
Vector3D<T> Vector3D<T>::operator/(double i_value) const
  {
  //Dividing by zero is considered correct. The caller code is responsible to handle resulting INF values properly.
  double inv = 1.0 / i_value;
  return (*this) * inv;
  }

template<typename T>
Vector3D<T> &Vector3D<T>::operator/=(double i_value)
  {
  //Dividing by zero is considered correct. The caller code is responsible to handle resulting INF values properly.
  double inv = 1.0 / i_value;
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
bool Vector3D<T>::Normalize()
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
Vector3D<T> Vector3D<T>::Normalized() const
  {
  Vector3D<T> tmp(*this);
  tmp.Normalize();
  return tmp;
  }

template<typename T>
bool Vector3D<T>::IsNormalized() const
  {
  // The epsilon value has been increased to account for the rounding error when converting vectors between float and double.
  return fabs(LengthSqr()-1.0) < (3e-7);
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

template <class charT, class traits, typename T>
std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, Vector3D<T> &o_vector)
  {
  i_stream >> o_vector[0] >> o_vector[1] >> o_vector[2];
  return i_stream;
  }

template <class charT, class traits, typename T>
inline std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const Vector3D<T> &i_vector)
  {
  /* string stream
  * - with same format
  * - without special field width
  */
  std::basic_ostringstream<charT,traits> s;
  s.copyfmt(o_stream);
  s.width(0);

  s << i_vector[0] << ' ' << i_vector[1] << ' ' << i_vector[2];
  o_stream << s.str();
  return o_stream;
  }

template<typename T2, typename T>
Vector3D<T2> Convert(const Vector3D<T> &i_vector)
  {
  return Vector3D<T2>((T2)i_vector[0], (T2)i_vector[1], (T2)i_vector[2]);
  }

/**
* Serializes Vector3D to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void serialize(Archive &i_ar, Vector3D<T> &i_vector, const unsigned int i_version)
  {
  i_ar & i_vector[0];
  i_ar & i_vector[1];
  i_ar & i_vector[2];
  }

#endif // VECTOR3D_H