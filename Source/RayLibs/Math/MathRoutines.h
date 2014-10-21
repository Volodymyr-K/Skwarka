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

#ifndef MATH_ROUTINES_H
#define MATH_ROUTINES_H

#include "Constants.h"
#include "Geometry.h"

/**
* This namespace contains helper routines for various math operations that are commonly used.
*/
namespace MathRoutines
  {
  /**
  * Solves 2x2 system of linear equations.
  */
  bool SolveLinearSystem2x2(const double i_A[2][2], const double i_B[2], double o_x[2]);

  /**
  * Clamps value to a specified range.
  */
  template<typename T>
  T Clamp(T i_value, T i_low, T i_high);

  /**
  * Linearly interpolates the value in the specified range.
  * If i_weight parameter is 0 the method returns i_low, if it is 1 the method returns i_high.
  * i_weight can be less than 0.0 or greater than 1.0, the method will still interpolate the values accordingly.
  */
  template<typename T>
  T LinearInterpolate(double i_weight, T i_low, T i_high);

  /**
  * Given the CDF and a sample value the method finds the sampled item.
  * The CDF is given by the range specified by the two random-access iterators.
  * The specified range can be a sub-range of an original CDF but the sample value needs to be in the specified range. 
  * The probability of sampling this item is written to op_pdf (if not NULL).
  */
  template<typename Iterator>
  Iterator BinarySearchCDF(Iterator i_begin, Iterator i_end, double i_sample, double *op_pdf = NULL);

  /**
  * Returns true if the integer is a power of 2.
  * false is returned if i_value is 0.
  */
  bool IsPowerOf2(unsigned int i_value);

  /**
  * Returns the floor of binary logarithm of the specified integer.
  * -1 is returned if i_value is 0.
  */
  int FloorLog2(unsigned int i_value);

  /**
  * Returns the ceil of binary logarithm of the specified integer.
  * -1 is returned if i_value is 0.
  */
  int CeilLog2(unsigned int i_value);

  /**
  * Returns the largest power of 2 less or equal than the specified integer.
  * 0 is returned if i_value is 0.
  */
  unsigned int RoundDownPow2(unsigned int i_value);

  /**
  * Returns the smallest power of 2 greater or equal than the specified integer.
  * 1 is returned if i_value is 0.
  */
  unsigned int RoundUpPow2(unsigned int i_value);

  /**
  * Returns the remainder of i_a / i_b integer division.
  * In contrast to the built-in % operator the function returns correct value for negative values of i_a.
  * The sign of i_b does not matter, but it should not be zero.
  */
  template<typename T>
  T Mod(T i_a, T i_b);

  /**
  * Returns binary logarithm of the specified value.
  * @param i_x Should be greater than zero.
  */
  double Log2(double i_x);

  /**
  * Creates an arbitrary orthonormal coordinate system with one of the base vectors specified.
  * @param i_e1 First base vector. Should be normalized.
  * @param[out] o_e2 Second base vector. Will be normalized and perpendicular to the other two base vectors.
  * @param[out] o_e3 Third base vector. Will be normalized and perpendicular to the other two base vectors.
  */
  template<typename T>
  void CoordinateSystem(const Vector3D<T> &i_e1, Vector3D<T> &o_e2, Vector3D<T> &o_e3);

  /**
  * Returns solid angle subtended by the specified (axis-aligned) box from the specified point.
  * If the point is inside (or on the boundaries of) the box the method returns full solid angle, i.e. 4*PI.
  */
  template<typename T>
  T SubtendedSolidAngle(const Point3D<T> &i_point, BBox3D<T> i_bbox);

  /**
  * Given a normalized vector the method returns sine and cosine values for phi and theta angles.
  * @warning The specified i_vector should be normalized.
  * For the sake of performance the method does not check if it is normalized, this is up to the calling code to ensure this.
  */
  template<typename T>
  void SphericalAngles(const Vector3D<T> &i_vector, T &o_sin_theta, T &o_cos_theta, T &o_sin_phi, T &o_cos_phi);

  /**
  * Given a normalized vector the method returns spherical theta angle in [0;PI] range.
  * @warning The specified i_vector should be normalized.
  * For the sake of performance the method does not check if it is normalized, this is up to the calling code to ensure this.
  */
  template<typename T>
  T SphericalTheta(const Vector3D<T> &i_vector);

  /**
  * Given a normalized vector the method returns spherical phi angle in [0;2*PI) range.
  * @warning The specified i_vector should be normalized.
  * For the sake of performance the method does not check if it is normalized, this is up to the calling code to ensure this.
  */
  template<typename T>
  T SphericalPhi(const Vector3D<T> &i_vector);

  /**
  * Returns directions for the specified phi and theta angles.
  */
  template<typename T>
  Vector3D<T> SphericalDirection(double i_phi, double i_theta);

  /**
  * Converts degrees to radians.
  */
  double DegreesToRadians(double i_degrees);

  /**
  * Converts radians to degrees.
  */
  double RadiansToDegrees(double i_radiands);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace MathRoutines
  {

  inline bool SolveLinearSystem2x2(const double i_A[2][2], const double i_B[2], double o_x[2])
    {
    double det = i_A[0][0]*i_A[1][1] - i_A[0][1]*i_A[1][0];
    if (fabs(det) < 1e-5)
      return false;
    double invDet = 1.0/det;
    o_x[0] = (i_A[1][1]*i_B[0] - i_A[0][1]*i_B[1]) * invDet;
    o_x[1] = (i_A[0][0]*i_B[1] - i_A[1][0]*i_B[0]) * invDet;
    return true;
    }

  template<typename T>
  T Clamp(T i_value, T i_low, T i_high)
    {
    ASSERT(i_low<=i_high);

    if (i_value < i_low)
      return i_low;
    else if (i_value > i_high)
      return i_high;
    else
      return i_value;
    }

  template<typename T>
  T LinearInterpolate(double i_weight, T i_low, T i_high)
    {
    return (T) ( (1.0 - i_weight) * i_low + i_weight * i_high );
    }

  template<typename Iterator>
  Iterator BinarySearchCDF(Iterator i_begin, Iterator i_end, double i_sample, double *op_pdf)
    {
    ASSERT(i_begin<i_end);
    ASSERT(i_sample>=0 && i_sample<1.0);

    Iterator begin = i_begin, end = i_end;

    --end;
    while(begin<end)
      {
      ASSERT((*begin) <= (*end) && i_sample < (*end));

      Iterator medium = begin + std::distance(begin,end)/2;
      if ((*medium) <= i_sample)
        begin = medium+1;
      else
        end = medium;
      }

    if (op_pdf)
      if (begin == i_begin)
        *op_pdf = *begin;
      else
        *op_pdf = *begin - *(begin-1);

    return begin;
    }

  inline bool IsPowerOf2(unsigned int i_value)
    {
    if (i_value==0)
      return false;

    return (i_value & (i_value - 1)) == 0;
    }

  inline int FloorLog2(unsigned int i_value)
    {
    int pos = 0;
    if (i_value >= 1<<16) {i_value >>= 16;pos += 16;}
    if (i_value >= 1<< 8) {i_value >>=  8;pos +=  8;}
    if (i_value >= 1<< 4) {i_value >>=  4;pos +=  4;}
    if (i_value >= 1<< 2) {i_value >>=  2;pos +=  2;}
    if (i_value >= 1<< 1) pos +=  1;

    return ((i_value == 0) ? (-1) : pos);
    }

  inline int CeilLog2(unsigned int i_value)
    {
    int pos = IsPowerOf2(i_value) ? 0 : 1;
    if (i_value >= 1<<16) {i_value >>= 16;pos += 16;}
    if (i_value >= 1<< 8) {i_value >>=  8;pos +=  8;}
    if (i_value >= 1<< 4) {i_value >>=  4;pos +=  4;}
    if (i_value >= 1<< 2) {i_value >>=  2;pos +=  2;}
    if (i_value >= 1<< 1) pos +=  1;

    return ((i_value == 0) ? (-1) : pos);
    }

  inline unsigned int RoundUpPow2(unsigned int i_value)
    {
    if (i_value==0) return 1;

    --i_value;
    i_value |= i_value >> 1;
    i_value |= i_value >> 2;
    i_value |= i_value >> 4;
    i_value |= i_value >> 8;
    i_value |= i_value >> 16;
    return i_value+1;
    }

  inline unsigned int RoundDownPow2(unsigned int i_value)
    {
    if (i_value==0) return 0;
    else
      if ((i_value & (i_value - 1)) == 0)
        return i_value;
      else
        return RoundUpPow2(i_value)>>1;
    }

  template<typename T>
  T Mod(T i_a, T i_b)
    {
    ASSERT(i_b != 0);

    if (i_a>=0)
      return i_a%i_b;
    else
      if (i_b>0)
        return ((i_a%i_b)+i_b)%i_b;
      else
        return ((i_a%i_b)-i_b)%i_b;
    }

  inline double Log2(double i_x)
    {
    static double inv_log_2 = 1.0 / log(2.0);
    ASSERT(i_x>0);

    return log(i_x) * inv_log_2;
    }

  template<typename T>
  void CoordinateSystem(const Vector3D<T> &i_e1, Vector3D<T> &o_e2, Vector3D<T> &o_e3)
    {
    ASSERT(i_e1.IsNormalized());

    if (fabs(i_e1[0]) > fabs(i_e1[1]))
      {
      double invLen = 1.0 / sqrt(i_e1[0]*i_e1[0] + i_e1[2]*i_e1[2]);
      o_e2 = Vector3D<T>((T)(-i_e1[2]*invLen), (T)0.0, (T)(i_e1[0]*invLen));
      }
    else
      {
      double invLen = 1.0 / sqrt(i_e1[1]*i_e1[1] + i_e1[2]*i_e1[2]);
      o_e2 = Vector3D<T>((T)0.0, (T)(i_e1[2]*invLen), (T)(-i_e1[1]*invLen));
      }
    o_e3 = i_e1^o_e2;
    }

  /**
  * This is a helper method that computes solid angle subtended by a triangle from a point.
  * The triangle is specified by three directions to its vertices from the point.
  * The authors of the algorithm are Oosterom and Strackee.
  * This method is used in the SubtendedSolidAngle() method.
  */
  template<typename T>
  T _SubtendedSolidAngle(const Vector3D<T> &i_dir1, const Vector3D<T> &i_dir2, const Vector3D<T> &i_dir3)
    {
    double determinant = i_dir1*(i_dir2^i_dir3);
    double denom =  1+(i_dir1*i_dir2)+(i_dir1*i_dir3)+(i_dir2*i_dir3);

    if (denom>0.0)
      return fabs(atan(determinant / denom)*2.0);
    else if (denom<0.0)
      return 2.0*(M_PI-fabs(atan(determinant / denom)));
    else
      return 2.0*M_PI;
    }

  template<typename T>
  T SubtendedSolidAngle(const Point3D<T> &i_point, BBox3D<T> i_bbox)
    {
    ASSERT(i_bbox.m_min[0]<=i_bbox.m_max[0]);
    ASSERT(i_bbox.m_min[1]<=i_bbox.m_max[1]);
    ASSERT(i_bbox.m_min[2]<=i_bbox.m_max[2]);

    Vector3D<T> dirs[8];
    for(unsigned char i=0;i<8;++i)
      {
      Point3D<T> vertex((i&1)==0 ? i_bbox.m_min[0] : i_bbox.m_max[0], (i&2)==0 ? i_bbox.m_min[1] : i_bbox.m_max[1], (i&4)==0 ? i_bbox.m_min[2] : i_bbox.m_max[2]);
      dirs[i] = Vector3D<T>(vertex-i_point).Normalized();
      }

    bool inside = true;
    double sum = 0.0;

    if (i_point[0]<i_bbox.m_min[0]-DBL_EPS)
      {
      sum += _SubtendedSolidAngle(dirs[0],dirs[2],dirs[6]) + _SubtendedSolidAngle(dirs[0],dirs[4],dirs[6]);
      inside=false;
      }
    else if (i_point[0]>i_bbox.m_max[0]+DBL_EPS)
      {
      sum += _SubtendedSolidAngle(dirs[1],dirs[3],dirs[7]) + _SubtendedSolidAngle(dirs[1],dirs[5],dirs[7]);
      inside=false;
      }

    if (i_point[1]<i_bbox.m_min[1]-DBL_EPS)
      {
      sum += _SubtendedSolidAngle(dirs[0],dirs[1],dirs[5]) + _SubtendedSolidAngle(dirs[0],dirs[4],dirs[5]);
      inside=false;
      }
    else if (i_point[1]>i_bbox.m_max[1]+DBL_EPS)
      {
      sum += _SubtendedSolidAngle(dirs[2],dirs[3],dirs[7]) + _SubtendedSolidAngle(dirs[2],dirs[6],dirs[7]);
      inside=false;
      }

    if (i_point[2]<i_bbox.m_min[2]-DBL_EPS)
      {
      sum += _SubtendedSolidAngle(dirs[0],dirs[1],dirs[3]) + _SubtendedSolidAngle(dirs[0],dirs[2],dirs[3]);
      inside=false;
      }
    else if (i_point[2]>i_bbox.m_max[2]+DBL_EPS)
      {
      sum += _SubtendedSolidAngle(dirs[4],dirs[5],dirs[7]) + _SubtendedSolidAngle(dirs[4],dirs[6],dirs[7]);
      inside=false;
      }

    ASSERT(sum>=0.0 && sum<=4.0*M_PI);

    if (inside)
      return 4.0*M_PI;
    else
      return sum;
    }

  template<typename T>
  void SphericalAngles(const Vector3D<T> &i_vector, T &o_sin_theta, T &o_cos_theta, T &o_sin_phi, T &o_cos_phi)
    {
    ASSERT(i_vector.IsNormalized());

    o_sin_theta=sqrt(i_vector[0]*i_vector[0] + i_vector[1]*i_vector[1]);
    o_cos_theta=i_vector[2];

    if (o_sin_theta>DBL_3D_EPS)
      {
      double inv_theta_sin=1.0/o_sin_theta;
      o_sin_phi = (T) (i_vector[1] * inv_theta_sin);
      o_cos_phi = (T) (i_vector[0] * inv_theta_sin);
      }
    else
      {
      o_sin_phi = (T)0.0;
      o_cos_phi = (T)1.0;
      }
    }

  template<typename T>
  T SphericalTheta(const Vector3D<T> &i_vector)
    {
    ASSERT(i_vector.IsNormalized());

    return (T) acos(Clamp(i_vector[2], -1.0, 1.0));
    }

  template<typename T>
  T SphericalPhi(const Vector3D<T> &i_vector)
    {
    ASSERT(i_vector.IsNormalized());

    double p = atan2(i_vector[1], i_vector[0]);
    return (T) ( (p < 0.0) ? p + 2.0*M_PI : p );
    }

  template<typename T>
  Vector3D<T> SphericalDirection(double i_phi, double i_theta)
    {
    double sint = sin(i_theta), cost = cos(i_theta);
    return Vector3D<T>((T)(sint*cos(i_phi)), (T)(sint*sin(i_phi)), (T)(cost));
    }

  inline double DegreesToRadians(double i_degrees)
    {
    return i_degrees*M_PI/180.0;
    }

  inline double RadiansToDegrees(double i_radiands)
    {
    return i_radiands*180.0/M_PI;
    }
  }

#endif // MATH_ROUTINES_H