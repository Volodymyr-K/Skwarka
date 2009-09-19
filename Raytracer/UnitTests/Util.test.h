#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Util.h>

class UtilTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_SolveLinearSystem2x2()
      {
      double A[2][2]={{-1.5,2.0},{0.5,1.5}};
      double B[2]={1.2,5.0};
      double x[2];

      bool solved=MathRoutines::SolveLinearSystem2x2(A,B,x);
      TS_ASSERT(solved);

      double det=A[0][0]*A[1][1]-A[1][0]*A[0][1];
      double x0=(B[0]*A[1][1]-B[1]*A[0][1])/det;
      double x1=(A[0][0]*B[1]-A[1][0]*B[0])/det;
      TS_ASSERT_EQUALS(x[0],x0);
      TS_ASSERT_EQUALS(x[1],x1);
      }

    void test_SolveLinearSystem2x2Degenerated()
      {
      double A[2][2]={{-1.5,2.0},{3.0,-4.0}};
      double B[2]={1.2,5.0};
      double x[2];

      bool solved=MathRoutines::SolveLinearSystem2x2(A,B,x);
      TS_ASSERT(solved==false);
      }

    // Test when value is less than lower bound.
    void test_Clamp1()
      {
      double clamped = MathRoutines::Clamp(1.0, 2.0, 3.0);
      TS_ASSERT(clamped==2.0);
      }

    // Test when value is higher than upper bound.
    void test_Clamp2()
      {
      double clamped = MathRoutines::Clamp(5.0, 2.0, 3.0);
      TS_ASSERT(clamped==3.0);
      }

    // Test when value is inside the bounds.
    void test_Clamp3()
      {
      double clamped = MathRoutines::Clamp(2.2, 2.0, 3.0);
      TS_ASSERT(clamped==2.2);
      }

    // Tests for a special case when 0 is passed.
    void test_IsPowerOf2_ZeroCase()
      {
      TS_ASSERT_EQUALS(MathRoutines::IsPowerOf2(0), false);
      }

    void test_IsPowerOf2_PowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::IsPowerOf2(16), true);
      }

    void test_IsPowerOf2_NotPowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::IsPowerOf2(15), false);
      }

    // Tests for a special case when 0 is passed.
    void test_FloorLog2_ZeroCase()
      {
      TS_ASSERT_EQUALS(MathRoutines::FloorLog2(0),-1);
      }

    void test_FloorLog2_OneCase()
      {
      TS_ASSERT_EQUALS(MathRoutines::FloorLog2(1),0);
      }

    void test_FloorLog2_PowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::FloorLog2(16),4);
      }

    void test_FloorLog2_NotPowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::FloorLog2(15),3);
      }

    // Tests for a special case when 0 is passed.
    void test_CeilLog2_ZeroCase()
      {
      TS_ASSERT_EQUALS(MathRoutines::CeilLog2(0),-1);
      }

    void test_CeilLog2_OneCase()
      {
      TS_ASSERT_EQUALS(MathRoutines::CeilLog2(1),0);
      }

    void test_CeilLog2_PowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::CeilLog2(16),4);
      }

    void test_CeilLog2_NotPowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::CeilLog2(15),4);
      }

    void test_CoordinateSystem()
      {
      Vector3D_d e1(1.5,-0.5,1.0);
      e1.Normalize();

      Vector3D_d e2,e3;
      MathRoutines::CoordinateSystem(e1,e2,e3);

      TS_ASSERT(e2.IsNormalized());
      TS_ASSERT(e3.IsNormalized());

      TS_ASSERT_DELTA((e1^e2)[0],e3[0],(1e-10));
      TS_ASSERT_DELTA((e1^e2)[1],e3[1],(1e-10));
      TS_ASSERT_DELTA((e1^e2)[2],e3[2],(1e-10));

      TS_ASSERT_DELTA((e2^e3)[0],e1[0],(1e-10));
      TS_ASSERT_DELTA((e2^e3)[1],e1[1],(1e-10));
      TS_ASSERT_DELTA((e2^e3)[2],e1[2],(1e-10));
      }
  };

#endif // UTIL_TEST_H