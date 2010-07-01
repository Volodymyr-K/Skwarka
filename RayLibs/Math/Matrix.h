#ifndef MATRIX_H
#define MATRIX_H

#include <Common/Common.h>
#include <cstring>

/**
* Class template for 4x4 matrix.
* The template parameter corresponds to the type of the matrix elements.
* @sa Transform
*/
template<typename T>
class Matrix4x4
  {
  public:
    /**
    * Constructs Matrix4x4 instance.
    * If i_identity parameter is true an identity matrix is constructed.
    * If i_identity parameter is false a zero matrix is constructed.
    */
    Matrix4x4(bool i_identity=false);

    Matrix4x4(
      T i_v00, T i_v01, T i_v02, T i_v03,
      T i_v10, T i_v11, T i_v12, T i_v13,
      T i_v20, T i_v21, T i_v22, T i_v23,
      T i_v30, T i_v31, T i_v32, T i_v33);

    Matrix4x4(T i_values[4][4]);

    Matrix4x4<T> operator+(const Matrix4x4<T> &i_matrix) const;
    Matrix4x4<T> &operator+=(const Matrix4x4<T> &i_matrix);

    Matrix4x4<T> operator-(const Matrix4x4<T> &i_matrix) const;
    Matrix4x4<T> &operator-=(const Matrix4x4<T> &i_matrix);

    Matrix4x4<T> operator*(T i_value) const;
    Matrix4x4<T> &operator*=(T i_value);

    /**
    * Pre-multiplies the matrix by the input matrix. The input matrix is the left operand and this matrix is the right operand of multiplication.
    */
    void PreMultiply(const Matrix4x4<T> &i_matrix);

    /**
    * Post-multiplies the matrix by the input matrix. The input matrix is the right operand and this matrix is the left operand of multiplication.
    */
    void PostMultiply(const Matrix4x4<T> &i_matrix);

    /**
    * Computes the inverted matrix.
    * @return true on success and false if the matrix is singular.
    */
    bool Inverted(Matrix4x4<T> &o_inverted_matrix) const;

    /**
    * Returns transposed matrix.
    */
    Matrix4x4 Transposed() const;

  public:
    // Public data members.
    T m_values[4][4];
  };

template<typename T>
Matrix4x4<T> operator*(T i_value, const Matrix4x4<T> &i_matrix);

template<typename T>
Matrix4x4<T> operator*(const Matrix4x4<T> &i_left, const Matrix4x4<T> &i_right);

typedef Matrix4x4<float> Matrix4x4_f;
typedef Matrix4x4<double> Matrix4x4_d;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
Matrix4x4<T>::Matrix4x4(bool i_identity)
  {
  for (unsigned char i = 0; i < 4; ++i)
    for (unsigned char j = 0; j < 4; ++j)
      if (i == j)
        m_values[i][j] = i_identity ? (T)1.0 : (T)0.0;
      else
        m_values[i][j] = (T)0.0;
  }

template<typename T>
Matrix4x4<T>::Matrix4x4(
          T i_v00, T i_v01, T i_v02, T i_v03,
          T i_v10, T i_v11, T i_v12, T i_v13,
          T i_v20, T i_v21, T i_v22, T i_v23,
          T i_v30, T i_v31, T i_v32, T i_v33)
  {
  m_values[0][0]=i_v00;m_values[0][1]=i_v01;m_values[0][2]=i_v02;m_values[0][3]=i_v03;
  m_values[1][0]=i_v10;m_values[1][1]=i_v11;m_values[1][2]=i_v12;m_values[1][3]=i_v13;
  m_values[2][0]=i_v20;m_values[2][1]=i_v21;m_values[2][2]=i_v22;m_values[2][3]=i_v23;
  m_values[3][0]=i_v30;m_values[3][1]=i_v31;m_values[3][2]=i_v32;m_values[3][3]=i_v33;
  }

template<typename T>
Matrix4x4<T>::Matrix4x4(T i_values[4][4])
  {
  for (unsigned char i = 0; i < 4; ++i)
    for (unsigned char j = 0; j < 4; ++j)
      m_values[i][j]=i_values[i][j];
  }

template<typename T>
Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T> &i_matrix) const
  {
  Matrix4x4<T> ret;
  for (unsigned char i = 0; i < 4; ++i)
    for (unsigned char j = 0; j < 4; ++j)
      ret.m_values[i][j]=m_values[i][j]+i_matrix.m_values[i][j];

  return ret;
  }

template<typename T>
Matrix4x4<T> &Matrix4x4<T>::operator+=(const Matrix4x4<T> &i_matrix)
  {
  Matrix4x4<T> ret;
  for (unsigned char i = 0; i < 4; ++i)
    for (unsigned char j = 0; j < 4; ++j)
      m_values[i][j]+=i_matrix.m_values[i][j];

  return *this;
  }

template<typename T>
Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T> &i_matrix) const
  {
  Matrix4x4<T> ret;
  for (unsigned char i = 0; i < 4; ++i)
    for (unsigned char j = 0; j < 4; ++j)
      ret.m_values[i][j]=m_values[i][j]-i_matrix.m_values[i][j];

  return ret;
  }

template<typename T>
Matrix4x4<T> &Matrix4x4<T>::operator-=(const Matrix4x4<T> &i_matrix)
  {
  Matrix4x4<T> ret;
  for (unsigned char i = 0; i < 4; ++i)
    for (unsigned char j = 0; j < 4; ++j)
      m_values[i][j]-=i_matrix.m_values[i][j];

  return *this;
  }

template<typename T>
Matrix4x4<T> Matrix4x4<T>::operator*(T i_value) const
  {
  Matrix4x4<T> ret;
  for (unsigned char i = 0; i < 4; ++i)
    for (unsigned char j = 0; j < 4; ++j)
      ret.m_values[i][j]=m_values[i][j]*i_value;

  return ret;
  }

template<typename T>
Matrix4x4<T> &Matrix4x4<T>::operator*=(T i_value)
  {
  Matrix4x4<T> ret;
  for (unsigned char i = 0; i < 4; ++i)
    for (unsigned char j = 0; j < 4; ++j)
      m_values[i][j]*=i_value;

  return *this;
  }

template<typename T>
void Matrix4x4<T>::PreMultiply(const Matrix4x4<T> &i_matrix)
  {
  T tmp[4][4];
  memcpy(tmp, m_values, 4*4*sizeof(T));

  for (unsigned char i = 0; i < 4; ++i)
    for (unsigned char j = 0; j < 4; ++j)
      m_values[i][j]=
      i_matrix.m_values[i][0]*tmp[0][j]+
      i_matrix.m_values[i][1]*tmp[1][j]+
      i_matrix.m_values[i][2]*tmp[2][j]+
      i_matrix.m_values[i][3]*tmp[3][j];
  }

template<typename T>
void Matrix4x4<T>::PostMultiply(const Matrix4x4<T> &i_matrix)
  {
  T tmp[4][4];
  memcpy(tmp, m_values, 4*4*sizeof(T));

  for (unsigned char i = 0; i < 4; ++i)
    for (unsigned char j = 0; j < 4; ++j)
      m_values[i][j]=
      tmp[i][0]*i_matrix.m_values[0][j]+
      tmp[i][1]*i_matrix.m_values[1][j]+
      tmp[i][2]*i_matrix.m_values[2][j]+
      tmp[i][3]*i_matrix.m_values[3][j];
  }

template<typename T>
Matrix4x4<T> Matrix4x4<T>::Transposed() const
  {
  return Matrix4x4<T>(
    m_values[0][0], m_values[1][0], m_values[2][0], m_values[3][0],
    m_values[0][1], m_values[1][1], m_values[2][1], m_values[3][1],
    m_values[0][2], m_values[1][2], m_values[2][2], m_values[3][2],
    m_values[0][3], m_values[1][3], m_values[2][3], m_values[3][3]);
  }

/**
* The method uses numerically stable Gauss-Jordan elimination method.
*/
template<typename T>
bool Matrix4x4<T>::Inverted(Matrix4x4<T> &o_inverted_matrix) const
  {
  int indxc[4], indxr[4];
  int ipiv[4] = {0, 0, 0, 0};
  T minv[4][4];
  memcpy(minv, m_values, 4*4*sizeof(T));

  bool singular = false;
  for (char i=0;i<4;++i)
    {
    char irow = -1, icol = -1;
    T big = (T)0.0;
    // Choose pivot
    for (char j=0;j<4;++j)
      {
      if (ipiv[j] != 1)
        {
        for (char k=0;k<4;++k)
          {
          if (ipiv[k] == 0) {
            if (fabs(minv[j][k]) >= big)
              {
              big = fabs(minv[j][k]);
              irow = j;
              icol = k;
              }
            }
          else if (ipiv[k] > 1)
            singular=true;
          }
        }
      }

    ++ipiv[icol];
    // Swap rows _irow_ and icol for pivot
    if (irow != icol)
      {
      for (char k=0;k<4;++k)
        std::swap(minv[irow][k], minv[icol][k]);
      }

    indxr[i] = irow;
    indxc[i] = icol;
    if (minv[icol][icol] == (T)0.0)
      singular=true;

    // Set m_values[icol][icol] to one by scaling row icol appropriately
    T pivinv = (T)1.0 / minv[icol][icol];
    minv[icol][icol] = (T)1.0;
    for (char j=0;j<4;++j)
      minv[icol][j] *= pivinv;

    // Subtract this row from others to zero out their columns
    for (char j=0;j<4;++j)
      {
      if (j != icol)
        {
        T save = minv[j][icol];
        minv[j][icol] = 0;
        for (char k=0;k<4;++k)
          minv[j][k] -= minv[icol][k]*save;
        }
      }
    }

  // Swap columns to reflect permutation
  for (char j=3;j>=0;--j)
    {
    if (indxr[j] != indxc[j])
      {
      for (char k=0;k<4;++k)
        std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
      }
    }

  o_inverted_matrix=Matrix4x4<T>(minv);
  return singular==false;
  }

template<typename T>
Matrix4x4<T> operator*(T i_value, const Matrix4x4<T> &i_matrix)
  {
  return i_matrix*i_value;
  }

template<typename T>
Matrix4x4<T> operator*(const Matrix4x4<T> &i_left, const Matrix4x4<T> &i_right)
  {
  T tmp[4][4];
  for (unsigned char i = 0; i < 4; ++i)
    for (unsigned char j = 0; j < 4; ++j)
      tmp[i][j]=
      i_left.m_values[i][0]*i_right.m_values[0][j]+
      i_left.m_values[i][1]*i_right.m_values[1][j]+
      i_left.m_values[i][2]*i_right.m_values[2][j]+
      i_left.m_values[i][3]*i_right.m_values[3][j];

  return Matrix4x4<T>(tmp);
  }

/**
* Serializes Matrix4x4 to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void serialize(Archive &i_ar, Matrix4x4<T> &i_matrix, const unsigned int i_version)
  {
  i_ar & i_matrix.m_values;
  }

#endif // MATRIX_H