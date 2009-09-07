#ifndef VECTOR2D_TEST_H
#define VECTOR2D_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Vector2D.h>
#include <sstream>

class Vector2DTestSuite : public CxxTest::TestSuite
  {
  public:
    void testVectorDefaultConstr()
      {
      Vector2D_d v1;
      TS_ASSERT_EQUALS(v1, Vector2D_d(0.0,0.0));
      }

    void testVectorConstrWithCoords()
      {
      Vector2D_d v1(1.1,-2.1);
      TS_ASSERT_EQUALS(v1, Vector2D_d(1.1,-2.1));
      }

    void testVectorConstrWithPoint()
      {
      Point2D_d p1(1.1,-2.1);
      Vector2D_d v1(p1);
      TS_ASSERT_EQUALS(v1, Vector2D_d(1.1,-2.1));
      }

    void testVectorAdd()
      {
      Vector2D_d v1(1.5,2.0);
      Vector2D_d v2(1.0,0.0);
      Vector2D_d v_sum=v1+v2;
      TS_ASSERT_EQUALS(v_sum, Vector2D_d(2.5,2.0));
      }

    void testVectorAddAssign()
      {
      Vector2D_d v1(1.5,2.0);
      Vector2D_d v2(1.0,0.0);
      Vector2D_d v_sum=v1;
      v_sum+=v2;
      TS_ASSERT_EQUALS(v_sum, Vector2D_d(2.5,2.0));
      }

    void testVectorSub()
      {
      Vector2D_d v1(1.5,2.0);
      Vector2D_d v2(1.0,10.0);
      Vector2D_d v_sub=v1-v2;
      TS_ASSERT_EQUALS(v_sub, Vector2D_d(0.5,-8.0));
      }

    void testVectorSubAssign()
      {
      Vector2D_d v1(1.5,2.0);
      Vector2D_d v2(1.0,10.0);
      Vector2D_d v_sub=v1;
      v_sub-=v2;
      TS_ASSERT_EQUALS(v_sub, Vector2D_d(0.5,-8.0));
      }

    void testVectorScalarMult()
      {
      Vector2D_d v1(1.0,2.0);
      Vector2D_d v_scalar=v1*1.5;
      TS_ASSERT_EQUALS(v_scalar, Vector2D_d(1.5,3.0));
      }

    void testVectorScalarPreMult()
      {
      Vector2D_d v1(1.0,2.0);
      Vector2D_d v_scalar=1.5*v1;
      TS_ASSERT_EQUALS(v_scalar, Vector2D_d(1.5,3.0));
      }

    void testVectorScalarMultAssign()
      {
      Vector2D_d v1(1.0,2.0);
      Vector2D_d v_scalar=v1;
      v_scalar*=1.5;
      TS_ASSERT_EQUALS(v_scalar, Vector2D_d(1.5,3.0));
      }

    void testVectorDiv()
      {
      Vector2D_d v1(1.0,2.0);
      Vector2D_d v_div=v1/2.0;
      TS_ASSERT_EQUALS(v_div, Vector2D_d(0.5,1.0));
      }

    void testVectorDivAssign()
      {
      Vector2D_d v1(1.0,2.0);
      Vector2D_d v_div=v1;
      v_div/=2.0;
      TS_ASSERT_EQUALS(v_div, Vector2D_d(0.5,1.0));
      }

    // Disable compiler warning since we divide by zero intentionally.
#pragma warning( push )
#pragma warning( disable : 4723 )
    void testVectorDivByZero()
      {
      Vector2D_d v1(-1.0,2.0);
      Vector2D_d v_div=v1/0.0;
      TS_ASSERT_EQUALS(v_div, Vector2D_d(-DBL_INF,DBL_INF));
      }
#pragma warning( pop )

    void testVectorEqual()
      {
      Vector2D_d v1(1.0,2.0);
      Vector2D_d v2(1.0,2.0);
      Vector2D_d v3(-1.0,-2.0);
      TS_ASSERT((v1==v2) && !(v1==v3));
      }

    void testVectorDotMult()
      {
      Vector2D_d v1(1.0,3.0);
      Vector2D_d v2(-1.0,4.0);
      double v_dot=v1*v2;
      TS_ASSERT_EQUALS(v_dot, 11.0);
      }

    void testVectorCrossMult()
      {
      Vector2D_d v1(1.0,3.0);
      Vector2D_d v2(-1.0,4.0);
      double v_cross=v1^v2;
      TS_ASSERT_EQUALS(v_cross, 7);
      }

    void testVectorLength()
      {
      Vector2D_d v1(1.0,2.0);
      double length = v1.Length();
      TS_ASSERT_EQUALS(length, sqrt(5.0));
      }

    void testVectorLengthSqr()
      {
      Vector2D_d v1(1.0,2.0);
      double length = v1.LengthSqr();
      TS_ASSERT_EQUALS(length, 5.0);
      }

    void testVectorNormalize()
      {
      Vector2D_d v1(1.0,2.0);
      v1.Normalize();

      double length=sqrt(5.0);
      TS_ASSERT_EQUALS(v1, Vector2D_d(1.0/length,2.0/length));
      }

    void testVectorNormalizeZeroVector()
      {
      Vector2D_d v1(0.0,0.0);
      Vector2D_d normalized = v1.Normalized();

      // Vector should not be divided by zero when normalizing.
      TS_ASSERT_EQUALS(normalized, Vector2D_d(0.0,0.0));
      }

    void testVectorNormalized()
      {
      Vector2D_d v1(1.0,2.0);
      Vector2D_d normalized = v1.Normalized();

      double length=sqrt(5.0);
      TS_ASSERT_EQUALS(normalized, Vector2D_d(1.0/length,2.0/length));
      }

    void testVectorIndexOperator()
      {
      Vector2D_d v1(1.0,2.0);
      TS_ASSERT(v1[0]==1.0 && v1[1]==2.0);
      }

    void testVectorIntegerScalarMult()
      {
      Vector2D<int> v1(1,2);
      Vector2D<int> v_scalar=v1*1.5;
      TS_ASSERT_EQUALS(v_scalar, Vector2D<int>(1,3));
      }

    void testVectorIntegerDiv()
      {
      Vector2D<int> v1(1,2);
      Vector2D<int> v_div=v1/2;
      TS_ASSERT_EQUALS(v_div, Vector2D<int>(0,1));
      }

    void testVectorVectorAdd()
      {
      Vector2D_d v1(1.5,2.0);
      Vector2D_d p1(1.0,0.0);
      Vector2D_d p_sum=p1+v1;
      TS_ASSERT_EQUALS(p_sum, Vector2D_d(2.5,2.0));
      }

    void testVectorVectorSub()
      {
      Vector2D_d v1(1.5,2.0);
      Vector2D_d p1(1.0,10.0);
      Vector2D_d p_sub=p1-v1;
      TS_ASSERT_EQUALS(p_sub, Vector2D_d(-0.5,8.0));
      }

    void testVectorOutputStream()
      {
      Vector2D_d v1(1.1,-2);
      std::stringstream sstream;
      sstream << v1;
      std::string s(sstream.str().c_str());
      TS_ASSERT(s=="1.1 -2");
      }

    void testVectorInputStream()
      {
      std::stringstream sstream;
      sstream << "1.1 -2.2";
      Vector2D_d v_read;
      sstream >> v_read;
      TS_ASSERT_EQUALS(v_read, Vector2D_d(1.1,-2.2));
      }

    void testVectorConversion()
      {
      Vector2D_d v1(1.8,-2.3);
      Vector2D<int> v_int=Convert<int>(v1);
      TS_ASSERT_EQUALS(v_int, Vector2D<int>(1,-2));
      }
  };

#endif // VECTOR2D_TEST_H