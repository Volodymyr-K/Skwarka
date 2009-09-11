#ifndef UTIL_H
#define UTIL_H

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

  }
#endif // UTIL_H