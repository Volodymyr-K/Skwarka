#ifndef UTIL_H
#define UTIL_H

/**
* Solves 2x2 system of linear equations.
*/
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

/**
* Clamps value to a specified range.
*/
template<typename T>
T Clamp(T i_value, T i_low, T i_high)
  {
  if (i_value < i_low)
    return i_low;
  else if (i_value > i_high)
    return i_high;
  else
    return i_value;
  }

#endif // UTIL_H