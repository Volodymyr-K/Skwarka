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

#ifndef MATRIX_3X3_H
#define MATRIX_3X3_H

#include <Common/Common.h>
#include "Constants.h"
#include <cstring>

/**
* Class template for 3x3 matrix.
* The template parameter corresponds to the type of the matrix elements.
* @sa Transform
*/
template<typename T>
class Matrix3x3
  {
  public:
    /**
    * Constructs Matrix3x3 instance.
    * If i_identity parameter is true an identity matrix is constructed.
    * If i_identity parameter is false a zero matrix is constructed.
    */
    Matrix3x3(bool i_identity=false);

    Matrix3x3(
      T i_v00, T i_v01, T i_v02,
      T i_v10, T i_v11, T i_v12,
      T i_v20, T i_v21, T i_v22);

    Matrix3x3(T i_values[3][3]);

    Matrix3x3<T> operator+(const Matrix3x3<T> &i_matrix) const;
    Matrix3x3<T> &operator+=(const Matrix3x3<T> &i_matrix);

    Matrix3x3<T> operator-(const Matrix3x3<T> &i_matrix) const;
    Matrix3x3<T> &operator-=(const Matrix3x3<T> &i_matrix);

    Matrix3x3<T> operator*(T i_value) const;
    Matrix3x3<T> &operator*=(T i_value);

    /**
    * Pre-multiplies the matrix by the input matrix. The input matrix is the left operand and this matrix is the right operand of multiplication.
    */
    void PreMultiply(const Matrix3x3<T> &i_matrix);

    /**
    * Post-multiplies the matrix by the input matrix. The input matrix is the right operand and this matrix is the left operand of multiplication.
    */
    void PostMultiply(const Matrix3x3<T> &i_matrix);

    /**
    * Computes the inverted matrix.
    * @return true on success and false if the matrix is singular.
    */
    bool Inverted(Matrix3x3<T> &o_inverted_matrix) const;

    /**
    * Multiplies the matrix with the input vector.
    */
    void Multiply(const T i_vector[3], T o_vector[3]) const;

    /**
    * Returns transposed matrix.
    */
    Matrix3x3 Transposed() const;

  public:
    // Public data members.
    T m_values[3][3];
  };

template<typename T>
Matrix3x3<T> operator*(T i_value, const Matrix3x3<T> &i_matrix);

template<typename T>
Matrix3x3<T> operator*(const Matrix3x3<T> &i_left, const Matrix3x3<T> &i_right);

typedef Matrix3x3<float> Matrix3x3_f;
typedef Matrix3x3<double> Matrix3x3_d;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
Matrix3x3<T>::Matrix3x3(bool i_identity)
  {
  for (unsigned char i = 0; i < 3; ++i)
    for (unsigned char j = 0; j < 3; ++j)
      if (i == j)
        m_values[i][j] = i_identity ? (T)1.0 : (T)0.0;
      else
        m_values[i][j] = (T)0.0;
  }

template<typename T>
Matrix3x3<T>::Matrix3x3(
          T i_v00, T i_v01, T i_v02,
          T i_v10, T i_v11, T i_v12,
          T i_v20, T i_v21, T i_v22)
  {
  m_values[0][0]=i_v00;m_values[0][1]=i_v01;m_values[0][2]=i_v02;
  m_values[1][0]=i_v10;m_values[1][1]=i_v11;m_values[1][2]=i_v12;
  m_values[2][0]=i_v20;m_values[2][1]=i_v21;m_values[2][2]=i_v22;
  }

template<typename T>
Matrix3x3<T>::Matrix3x3(T i_values[3][3])
  {
  for (unsigned char i = 0; i < 3; ++i)
    for (unsigned char j = 0; j < 3; ++j)
      m_values[i][j]=i_values[i][j];
  }

template<typename T>
Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T> &i_matrix) const
  {
  Matrix3x3<T> ret;
  for (unsigned char i = 0; i < 3; ++i)
    for (unsigned char j = 0; j < 3; ++j)
      ret.m_values[i][j]=m_values[i][j]+i_matrix.m_values[i][j];

  return ret;
  }

template<typename T>
Matrix3x3<T> &Matrix3x3<T>::operator+=(const Matrix3x3<T> &i_matrix)
  {
  Matrix3x3<T> ret;
  for (unsigned char i = 0; i < 3; ++i)
    for (unsigned char j = 0; j < 3; ++j)
      m_values[i][j]+=i_matrix.m_values[i][j];

  return *this;
  }

template<typename T>
Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T> &i_matrix) const
  {
  Matrix3x3<T> ret;
  for (unsigned char i = 0; i < 3; ++i)
    for (unsigned char j = 0; j < 3; ++j)
      ret.m_values[i][j]=m_values[i][j]-i_matrix.m_values[i][j];

  return ret;
  }

template<typename T>
Matrix3x3<T> &Matrix3x3<T>::operator-=(const Matrix3x3<T> &i_matrix)
  {
  Matrix3x3<T> ret;
  for (unsigned char i = 0; i < 3; ++i)
    for (unsigned char j = 0; j < 3; ++j)
      m_values[i][j]-=i_matrix.m_values[i][j];

  return *this;
  }

template<typename T>
Matrix3x3<T> Matrix3x3<T>::operator*(T i_value) const
  {
  Matrix3x3<T> ret;
  for (unsigned char i = 0; i < 3; ++i)
    for (unsigned char j = 0; j < 3; ++j)
      ret.m_values[i][j]=m_values[i][j]*i_value;

  return ret;
  }

template<typename T>
Matrix3x3<T> &Matrix3x3<T>::operator*=(T i_value)
  {
  Matrix3x3<T> ret;
  for (unsigned char i = 0; i < 3; ++i)
    for (unsigned char j = 0; j < 3; ++j)
      m_values[i][j]*=i_value;

  return *this;
  }

template<typename T>
void Matrix3x3<T>::PreMultiply(const Matrix3x3<T> &i_matrix)
  {
  T tmp[3][3];
  memcpy(tmp, m_values, 3*3*sizeof(T));

  for (unsigned char i = 0; i < 3; ++i)
    for (unsigned char j = 0; j < 3; ++j)
      m_values[i][j]=
      i_matrix.m_values[i][0]*tmp[0][j]+
      i_matrix.m_values[i][1]*tmp[1][j]+
      i_matrix.m_values[i][2]*tmp[2][j];
  }

template<typename T>
void Matrix3x3<T>::PostMultiply(const Matrix3x3<T> &i_matrix)
  {
  T tmp[3][3];
  memcpy(tmp, m_values, 3*3*sizeof(T));

  for (unsigned char i = 0; i < 3; ++i)
    for (unsigned char j = 0; j < 3; ++j)
      m_values[i][j]=
      tmp[i][0]*i_matrix.m_values[0][j]+
      tmp[i][1]*i_matrix.m_values[1][j]+
      tmp[i][2]*i_matrix.m_values[2][j];
  }

template<typename T>
Matrix3x3<T> Matrix3x3<T>::Transposed() const
  {
  return Matrix3x3<T>(
    m_values[0][0], m_values[1][0], m_values[2][0],
    m_values[0][1], m_values[1][1], m_values[2][1],
    m_values[0][2], m_values[1][2], m_values[2][2]);
  }

template<typename T>
bool Matrix3x3<T>::Inverted(Matrix3x3<T> &o_inverted_matrix) const
  {
  T a11 = m_values[0][0], a12=m_values[0][1], a13=m_values[0][2];
  T a21 = m_values[1][0], a22=m_values[1][1], a23=m_values[1][2];
  T a31 = m_values[2][0], a32=m_values[2][1], a33=m_values[2][2];
  T det = a11*(a33*a22-a32*a23)-a21*(a33*a12-a32*a13)+a31*(a23*a12-a22*a13);

  if (IsNaN(det) || fabs(det)<DBL_EPS)
    return false;

  T values[3][3];
  values[0][0]=a33*a22-a32*a23;    values[0][1]=-(a33*a12-a32*a13); values[0][2]=a23*a12-a22*a13;
  values[1][0]=-(a33*a21-a31*a23); values[1][1]=a33*a11-a31*a13;    values[1][2]=-(a23*a11-a21*a13);
  values[2][0]=a32*a21-a31*a22;    values[2][1]=-(a32*a11-a31*a12); values[2][2]=a22*a11-a21*a12;

  T inv_det = 1.0/det;
  for(unsigned char i=0;i<3;++i)
    for(unsigned char j=0;j<3;++j)
      values[i][j] *= inv_det;

  o_inverted_matrix = Matrix3x3<T>(values);
  return true;
  }

template<typename T>
void Matrix3x3<T>::Multiply(const T i_vector[3], T o_vector[3]) const
  {
  for(unsigned char i=0;i<3;++i)
    o_vector[i]=m_values[i][0]*i_vector[0]+m_values[i][1]*i_vector[1]+m_values[i][2]*i_vector[2];
  }

template<typename T>
Matrix3x3<T> operator*(T i_value, const Matrix3x3<T> &i_matrix)
  {
  return i_matrix*i_value;
  }

template<typename T>
Matrix3x3<T> operator*(const Matrix3x3<T> &i_left, const Matrix3x3<T> &i_right)
  {
  T tmp[3][3];
  for (unsigned char i = 0; i < 3; ++i)
    for (unsigned char j = 0; j < 3; ++j)
      tmp[i][j]=
      i_left.m_values[i][0]*i_right.m_values[0][j]+
      i_left.m_values[i][1]*i_right.m_values[1][j]+
      i_left.m_values[i][2]*i_right.m_values[2][j];

  return Matrix3x3<T>(tmp);
  }

#endif // MATRIX_3X3_H