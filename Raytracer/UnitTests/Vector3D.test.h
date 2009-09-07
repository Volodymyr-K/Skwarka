#ifndef VECTOR3D_TEST_H
#define VECTOR3D_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Vector3D.h>
#include <sstream>

class Vector3DTestSuite : public CxxTest::TestSuite
  {
  public:
    void testVectorDefaultConstr()
      {
      Vector3D_d v1;
      TS_ASSERT_EQUALS(v1, Vector3D_d(0.0,0.0,0.0));
      }

    void testVectorConstrWithCoords()
      {
      Vector3D_d v1(1.1,2.1,-3.1);
      TS_ASSERT_EQUALS(v1, Vector3D_d(1.1,2.1,-3.1));
      }

    void testVectorConstrWithPoint()
      {
      Point3D_d p1(1.1,2.1,-3.1);
      Vector3D_d v1(p1);
      TS_ASSERT_EQUALS(v1, Vector3D_d(1.1,2.1,-3.1));
      }

    void testVectorAdd()
      {
      Vector3D_d v1(1.5,2.0,3.0);
      Vector3D_d v2(1.0,0.0,2.0);
      Vector3D_d v_sum=v1+v2;
      TS_ASSERT_EQUALS(v_sum, Vector3D_d(2.5,2.0,5.0));
      }

    void testVectorAddAssign()
      {
      Vector3D_d v1(1.5,2.0,3.0);
      Vector3D_d v2(1.0,0.0,2.0);
      Vector3D_d v_sum=v1;
      v_sum+=v2;
      TS_ASSERT_EQUALS(v_sum, Vector3D_d(2.5,2.0,5.0));
      }

    void testVectorSub()
      {
      Vector3D_d v1(1.5,2.0, 3.0);
      Vector3D_d v2(1.0,10.0,2.0);
      Vector3D_d v_sub=v1-v2;
      TS_ASSERT_EQUALS(v_sub, Vector3D_d(0.5,-8.0,1.0));
      }

    void testVectorSubAssign()
      {
      Vector3D_d v1(1.5,2.0, 3.0);
      Vector3D_d v2(1.0,10.0,2.0);
      Vector3D_d v_sub=v1;
      v_sub-=v2;
      TS_ASSERT_EQUALS(v_sub, Vector3D_d(0.5,-8.0,1.0));
      }

    void testVectorScalarMult()
      {
      Vector3D_d v1(1.0,2.0,3.0);
      Vector3D_d v_scalar=v1*1.5;
      TS_ASSERT_EQUALS(v_scalar, Vector3D_d(1.5,3.0,4.5));
      }

    void testVectorScalarPreMult()
      {
      Vector3D_d v1(1.0,2.0,3.0);
      Vector3D_d v_scalar=1.5*v1;
      TS_ASSERT_EQUALS(v_scalar, Vector3D_d(1.5,3.0,4.5));
      }

    void testVectorScalarMultAssign()
      {
      Vector3D_d v1(1.0,2.0,3.0);
      Vector3D_d v_scalar=v1;
      v_scalar*=1.5;
      TS_ASSERT_EQUALS(v_scalar, Vector3D_d(1.5,3.0,4.5));
      }

    void testVectorDiv()
      {
      Vector3D_d v1(1.0,2.0,3.0);
      Vector3D_d v_div=v1/2.0;
      TS_ASSERT_EQUALS(v_div, Vector3D_d(0.5,1.0,1.5));
      }

    void testVectorDivAssign()
      {
      Vector3D_d v1(1.0,2.0,3.0);
      Vector3D_d v_div=v1;
      v_div/=2.0;
      TS_ASSERT_EQUALS(v_div, Vector3D_d(0.5,1.0,1.5));
      }

    // Disable compiler warning since we divide by zero intentionally.
#pragma warning( push )
#pragma warning( disable : 4723 )
    void testVectorDivByZero()
      {
      Vector3D_d v1(-1.0,2.0,3.0);
      Vector3D_d v_div=v1/0.0;
      TS_ASSERT_EQUALS(v_div, Vector3D_d(-DBL_INF,DBL_INF,DBL_INF));
      }
#pragma warning( pop )

    void testVectorEqual()
      {
      Vector3D_d v1(1.0,2.0,3.0);
      Vector3D_d v2(1.0,2.0,3.0);
      Vector3D_d v3(-1.0,-2.0,-3.0);
      TS_ASSERT((v1==v2) && !(v1==v3));
      }

    void testVectorDotMult()
      {
      Vector3D_d v1(1.0,2.0,4.0);
      Vector3D_d v2(-1.0,3.0,1.0);
      double v_dot=v1*v2;
      TS_ASSERT_EQUALS(v_dot, 9.0);
      }

    void testVectorCrossMult()
      {
      Vector3D_d v1(1.0,2.0,4.0);
      Vector3D_d v2(-1.0,3.0,1.0);
      Vector3D_d v_cross=v1^v2;
      TS_ASSERT_EQUALS(v_cross, Vector3D_d(-10.0,-5.0,5.0));
      }

    void testVectorLength()
      {
      Vector3D_d v1(1.0,2.0,3.0);
      double length = v1.Length();
      TS_ASSERT_EQUALS(length, sqrt(14.0));
      }

    void testVectorLengthSqr()
      {
      Vector3D_d v1(1.0,2.0,3.0);
      double length = v1.LengthSqr();
      TS_ASSERT_EQUALS(length, 14.0);
      }

    void testVectorNormalize()
      {
      Vector3D_d v1(1.0,2.0,3.0);
      v1.Normalize();

      double length=sqrt(14.0);
      TS_ASSERT_EQUALS(v1, Vector3D_d(1.0/length,2.0/length,3.0/length));
      }

    void testVectorNormalizeZeroVector()
      {
      Vector3D_d v1(0.0,0.0,0.0);
      Vector3D_d normalized = v1.Normalized();

      // Vector should not be divided by zero when normalizing.
      TS_ASSERT_EQUALS(normalized, Vector3D_d(0.0,0.0,0.0));
      }

    void testVectorNormalized()
      {
      Vector3D_d v1(1.0,2.0,3.0);
      Vector3D_d normalized = v1.Normalized();

      double length=sqrt(14.0);
      TS_ASSERT_EQUALS(normalized, Vector3D_d(1.0/length,2.0/length,3.0/length));
      }

    void testVectorIndexOperator()
      {
      Vector3D_d v1(1.0,2.0,3.0);
      TS_ASSERT(v1[0]==1.0 && v1[1]==2.0 && v1[2]==3.0);
      }

    void testVectorIntegerScalarMult()
      {
      Vector3D<int> v1(1,2,3);
      Vector3D<int> v_scalar=v1*1.5;
      TS_ASSERT_EQUALS(v_scalar, Vector3D<int>(1,3,4));
      }

    void testVectorIntegerDiv()
      {
      Vector3D<int> v1(1,2,3);
      Vector3D<int> v_div=v1/2;
      TS_ASSERT_EQUALS(v_div, Vector3D<int>(0,1,1));
      }

    void testVectorVectorAdd()
      {
      Vector3D_d v1(1.5,2.0,3.0);
      Vector3D_d p1(1.0,0.0,2.0);
      Vector3D_d p_sum=p1+v1;
      TS_ASSERT_EQUALS(p_sum, Vector3D_d(2.5,2.0,5.0));
      }

    void testVectorVectorSub()
      {
      Vector3D_d v1(1.5,2.0, 3.0);
      Vector3D_d p1(1.0,10.0,2.0);
      Vector3D_d p_sub=p1-v1;
      TS_ASSERT_EQUALS(p_sub, Vector3D_d(-0.5,8.0,-1.0));
      }

    void testVectorOutputStream()
      {
      Vector3D_d v1(1.1,-2.2,3.0);
      std::stringstream sstream;
      sstream << v1;
      std::string s(sstream.str().c_str());
      TS_ASSERT(s=="1.1 -2.2 3");
      }

    void testVectorInputStream()
      {
      std::stringstream sstream;
      sstream << "1.1 -2.2 3.0";
      Vector3D_d v_read;
      sstream >> v_read;
      TS_ASSERT_EQUALS(v_read, Vector3D_d(1.1,-2.2,3.0));
      }

    void testVectorConversion()
      {
      Vector3D_d v1(1.8,-2.3,3.0);
      Vector3D<int> v_int=Convert<int>(v1);
      TS_ASSERT_EQUALS(v_int, Vector3D<int>(1,-2,3));
      }
  };

#endif // VECTOR3D_TEST_H