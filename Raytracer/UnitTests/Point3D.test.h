#ifndef POINT3D_TEST_H
#define POINT3D_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Point3D.h>
#include <sstream>

class Point3DTestSuite : public CxxTest::TestSuite
  {
  public:
    void testPointDefaultConstr()
      {
      Point3D_d p1;
      TS_ASSERT_EQUALS(p1, Point3D_d(0.0,0.0,0.0));
      }

    void testPointConstrWithCoords()
      {
      Point3D_d p1(1.1,2.1,-3.1);
      TS_ASSERT_EQUALS(p1, Point3D_d(1.1,2.1,-3.1));
      }

    void testPointAdd()
      {
      Point3D_d p1(1.5,2.0,3.0);
      Point3D_d p2(1.0,0.0,2.0);
      Point3D_d p_sum=p1+p2;
      TS_ASSERT_EQUALS(p_sum, Point3D_d(2.5,2.0,5.0));
      }

    void testPointAddAssign()
      {
      Point3D_d p1(1.5,2.0,3.0);
      Point3D_d p2(1.0,0.0,2.0);
      Point3D_d p_sum=p1;
      p_sum+=p2;
      TS_ASSERT_EQUALS(p_sum, Point3D_d(2.5,2.0,5.0));
      }

    void testPointSub()
      {
      Point3D_d p1(1.5,2.0, 3.0);
      Point3D_d p2(1.0,10.0,2.0);
      Point3D_d p_sub=p1-p2;
      TS_ASSERT_EQUALS(p_sub, Point3D_d(0.5,-8.0,1.0));
      }

    void testPointSubAssign()
      {
      Point3D_d p1(1.5,2.0, 3.0);
      Point3D_d p2(1.0,10.0,2.0);
      Point3D_d p_sub=p1;
      p_sub-=p2;
      TS_ASSERT_EQUALS(p_sub, Point3D_d(0.5,-8.0,1.0));
      }

    void testPointMult()
      {
      Point3D_d p1(1.0,2.0,3.0);
      Point3D_d p_scalar=p1*1.5;
      TS_ASSERT_EQUALS(p_scalar, Point3D_d(1.5,3.0,4.5));
      }

    void testPointPreMult()
      {
      Point3D_d p1(1.0,2.0,3.0);
      Point3D_d p_scalar=1.5*p1;
      TS_ASSERT_EQUALS(p_scalar, Point3D_d(1.5,3.0,4.5));
      }

    void testPointMultAssign()
      {
      Point3D_d p1(1.0,2.0,3.0);
      Point3D_d p_scalar=p1;
      p_scalar*=1.5;
      TS_ASSERT_EQUALS(p_scalar, Point3D_d(1.5,3.0,4.5));
      }

    void testPointDiv()
      {
      Point3D_d p1(1.0,2.0,3.0);
      Point3D_d p_div=p1/2.0;
      TS_ASSERT_EQUALS(p_div, Point3D_d(0.5,1.0,1.5));
      }

    void testPointDivAssign()
      {
      Point3D_d p1(1.0,2.0,3.0);
      Point3D_d p_div=p1;
      p_div/=2.0;
      TS_ASSERT_EQUALS(p_div, Point3D_d(0.5,1.0,1.5));
      }

    // Disable compiler warning since we divide by zero intentionally.
#pragma warning( push )
#pragma warning( disable : 4723 )
    void testPointDivByZero()
      {
      Point3D_d p1(-1.0,2.0,3.0);
      Point3D_d p_div=p1/0.0;
      TS_ASSERT_EQUALS(p_div, Point3D_d(-DBL_INF,DBL_INF,DBL_INF));
      }
#pragma warning( pop )

    void testPointEqual()
      {
      Point3D_d p1(1.0,2.0,3.0);
      Point3D_d p2(1.0,2.0,3.0);
      Point3D_d p3(-1.0,-2.0,-3.0);
      TS_ASSERT((p1==p2) && !(p1==p3));
      }

    void testPointNotEqual()
      {
      Point3D_d p1(1.0,2.0,3.0);
      Point3D_d p2(1.0,2.0,3.0);
      Point3D_d p3(-1.0,-2.0,-3.0);
      TS_ASSERT((p1!=p3) && !(p1!=p2));
      }

    void testPointIndexOperator()
      {
      Point3D_d p1(1.0,2.0,3.0);
      TS_ASSERT(p1[0]==1.0 && p1[1]==2.0 && p1[2]==3.0);
      }

    void testPointIntegerMult()
      {
      Point3D<int> p1(1,2,3);
      Point3D<int> p_scalar=p1*1.5;
      TS_ASSERT_EQUALS(p_scalar, Point3D<int>(1,3,4));
      }

    void testPointIntegerDiv()
      {
      Point3D<int> p1(1,2,3);
      Point3D<int> p_div=p1/2;
      TS_ASSERT_EQUALS(p_div, Point3D<int>(0,1,1));
      }

    void testPointOutputStream()
      {
      Point3D_d p1(1.1,-2.2,3.0);
      std::stringstream sstream;
      sstream << p1;
      std::string s(sstream.str().c_str());
      TS_ASSERT(s=="1.1 -2.2 3");
      }

    void testPointInputStream()
      {
      std::stringstream sstream;
      sstream << "1.1 -2.2 3.0";
      Point3D_d p_read;
      sstream >> p_read;
      TS_ASSERT_EQUALS(p_read, Point3D_d(1.1,-2.2,3.0));
      }

    void testPointConversion()
      {
      Point3D_d p1(1.8,-2.3,3.0);
      Point3D<int> p_int=Convert<int>(p1);
      TS_ASSERT_EQUALS(p_int, Point3D<int>(1,-2,3));
      }
  };

#endif // POINT3D_TEST_H