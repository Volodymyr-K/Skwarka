#ifndef UTIL_H
#define UTIL_H

bool SolveLinearSystem2x2(const double A[2][2], const double B[2], double x[2])
  {
  double det = A[0][0]*A[1][1] - A[0][1]*A[1][0];
  if (fabs(det) < 1e-5)
    return false;
  double invDet = 1.0/det;
  x[0] = (A[1][1]*B[0] - A[0][1]*B[1]) * invDet;
  x[1] = (A[0][0]*B[1] - A[1][0]*B[0]) * invDet;
  return true;
  }

#endif // UTIL_H