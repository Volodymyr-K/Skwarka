#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Util.h>

class UtilTestSuite : public CxxTest::TestSuite
  {
  public:
    void testSolveLinearSystem2x2()
      {
      double A[2][2]={{-1.5,2.0},{0.5,1.5}};
      double B[2]={1.2,5.0};
      double x[2];

      bool solved=SolveLinearSystem2x2(A,B,x);
      TS_ASSERT(solved);

      double det=A[0][0]*A[1][1]-A[1][0]*A[0][1];
      double x0=(B[0]*A[1][1]-B[1]*A[0][1])/det;
      double x1=(A[0][0]*B[1]-A[1][0]*B[0])/det;
      TS_ASSERT_EQUALS(x[0],x0);
      TS_ASSERT_EQUALS(x[1],x1);
      }

    void testSolveLinearSystem2x2Degenerated()
      {
      double A[2][2]={{-1.5,2.0},{3.0,-4.0}};
      double B[2]={1.2,5.0};
      double x[2];

      bool solved=SolveLinearSystem2x2(A,B,x);
      TS_ASSERT(solved==false);
      }

    // Test when value is less than lower bound.
    void testClamp1()
      {
      double clamped = Clamp(1.0, 2.0, 3.0);
      TS_ASSERT(clamped==2.0);
      }

    // Test when value is higher than upper bound.
    void testClamp2()
      {
      double clamped = Clamp(5.0, 2.0, 3.0);
      TS_ASSERT(clamped==3.0);
      }

    // Test when value is inside the bounds.
    void testClamp3()
      {
      double clamped = Clamp(2.2, 2.0, 3.0);
      TS_ASSERT(clamped==2.2);
      }
  };

#endif // UTIL_TEST_H