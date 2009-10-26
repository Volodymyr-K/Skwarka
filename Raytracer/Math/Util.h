#ifndef UTIL_H
#define UTIL_H

#include "Vector3D.h"

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
  T LinearInterpolate(T i_weight, T i_low, T i_high);

  /**
  * Given the CDF and a sample value the method finds the sampled item.
  * The CDF is given by the range specified by the two random-access iterators.
  * The specified range can be a sub-range of an original CDF but the sample value needs to be in the specified range. 
  */
  template<typename Iterator>
  Iterator BinarySearchCDF(Iterator i_begin, Iterator i_end, double i_sample);

  /**
  * Returns true if the integer is a power of 2.
  * false is returned if i_value is 0.
  */
  bool IsPowerOf2(unsigned int i_value);

  /**
  * Returns the floor form of binary logarithm for an integer.
  * -1 is returned if i_value is 0.
  */
  int FloorLog2(unsigned int i_value);

  /**
  * Returns the ceil form of binary logarithm for an integer.
  * -1 is returned if i_value is 0.
  */
  int CeilLog2(unsigned int i_value);

  /**
  * Creates an arbitrary orthonormal coordinate system with one of the base vectors specified.
  * @param i_e1 First base vector. Should be normalized.
  * @param[out] o_e2 Second base vector. Will be normalized and perpendicular to the other two base vectors.
  * @param[out] o_e3 Third base vector. Will be normalized and perpendicular to the other two base vectors.
  */
  void CoordinateSystem(const Vector3D_d &i_e1, Vector3D_d &o_e2, Vector3D_d &o_e3);
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
  T LinearInterpolate(T i_weight, T i_low, T i_high)
    {
    return (T) ( (1.0 - i_weight) * i_low + i_weight * i_high );
    }

  template<typename Iterator>
  Iterator BinarySearchCDF(Iterator i_begin, Iterator i_end, double i_sample)
    {
    ASSERT(i_begin<i_end);
    ASSERT(i_sample>=0 && i_sample<1.0);

    --i_end;
    while(i_begin<i_end)
      {
      ASSERT((*i_begin) <= (*i_end) && i_sample < (*i_end));

      Iterator medium = i_begin + std::distance(i_begin,i_end)/2;
      if ((*medium) <= i_sample)
        i_begin = medium+1;
      else
        i_end = medium;
      }
    return i_begin;
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

  inline void CoordinateSystem(const Vector3D_d &i_e1, Vector3D_d &o_e2, Vector3D_d &o_e3)
    {
    ASSERT(i_e1.IsNormalized());

    if (fabs(i_e1[0]) > fabs(i_e1[1]))
      {
      double invLen = 1.0 / sqrt(i_e1[0]*i_e1[0] + i_e1[2]*i_e1[2]);
      o_e2 = Vector3D_d(-i_e1[2] * invLen, 0.0, i_e1[0] * invLen);
      }
    else
      {
      double invLen = 1.0 / sqrt(i_e1[1]*i_e1[1] + i_e1[2]*i_e1[2]);
      o_e2 = Vector3D_d(0.0, i_e1[2] * invLen, -i_e1[1] * invLen);
      }
    o_e3 = i_e1^o_e2;
    }

  }
#endif // UTIL_H