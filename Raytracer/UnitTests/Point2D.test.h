#ifndef POINT2D_TEST_H
#define POINT2D_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Point2D.h>
#include <sstream>

class Point2DTestSuite : public CxxTest::TestSuite
  {
  public:
    void testPointDefaultConstr()
      {
      Point2D_d p1;
      TS_ASSERT_EQUALS(p1, Point2D_d(0.0,0.0));
      }

    void testPointConstrWithCoords()
      {
      Point2D_d p1(1.1,-2.1);
      TS_ASSERT_EQUALS(p1, Point2D_d(1.1,-2.1));
      }

    void testPointAdd()
      {
      Point2D_d p1(1.5,2.0);
      Point2D_d p2(1.0,0.0);
      Point2D_d p_sum=p1+p2;
      TS_ASSERT_EQUALS(p_sum, Point2D_d(2.5,2.0));
      }

    void testPointAddAssign()
      {
      Point2D_d p1(1.5,2.0);
      Point2D_d p2(1.0,0.0);
      Point2D_d p_sum=p1;
      p_sum+=p2;
      TS_ASSERT_EQUALS(p_sum, Point2D_d(2.5,2.0));
      }

    void testPointSub()
      {
      Point2D_d p1(1.5,2.0);
      Point2D_d p2(1.0,10.0);
      Point2D_d p_sub=p1-p2;
      TS_ASSERT_EQUALS(p_sub, Point2D_d(0.5,-8.0));
      }

    void testPointSubAssign()
      {
      Point2D_d p1(1.5,2.0);
      Point2D_d p2(1.0,10.0);
      Point2D_d p_sub=p1;
      p_sub-=p2;
      TS_ASSERT_EQUALS(p_sub, Point2D_d(0.5,-8.0));
      }

    void testPointMult()
      {
      Point2D_d p1(1.0,2.0);
      Point2D_d p_scalar=p1*1.5;
      TS_ASSERT_EQUALS(p_scalar, Point2D_d(1.5,3.0));
      }

    void testPointPreMult()
      {
      Point2D_d p1(1.0,2.0);
      Point2D_d p_scalar=1.5*p1;
      TS_ASSERT_EQUALS(p_scalar, Point2D_d(1.5,3.0));
      }

    void testPointMultAssign()
      {
      Point2D_d p1(1.0,2.0);
      Point2D_d p_scalar=p1;
      p_scalar*=1.5;
      TS_ASSERT_EQUALS(p_scalar, Point2D_d(1.5,3.0));
      }

    void testPointDiv()
      {
      Point2D_d p1(1.0,-2.0);
      Point2D_d p_div=p1/2.0;
      TS_ASSERT_EQUALS(p_div, Point2D_d(0.5,-1.0));
      }

    void testPointDivAssign()
      {
      Point2D_d p1(1.0,-2.0);
      Point2D_d p_div=p1;
      p_div/=2.0;
      TS_ASSERT_EQUALS(p_div, Point2D_d(0.5,-1.0));
      }

    // Disable compiler warning since we divide by zero intentionally.
#pragma warning( push )
#pragma warning( disable : 4723 )
    void testPointDivByZero()
      {
      Point2D_d p1(-1.0,2.0);
      Point2D_d p_div=p1/0.0;
      TS_ASSERT_EQUALS(p_div, Point2D_d(-DBL_INF,DBL_INF));
      }
#pragma warning( pop )

    void testPointEqual()
      {
      Point2D_d p1(1.0,2.0);
      Point2D_d p2(1.0,2.0);
      Point2D_d p3(-1.0,-2.0);
      TS_ASSERT((p1==p2) && !(p1==p3));
      }

    void testPointNotEqual()
      {
      Point2D_d p1(1.0,2.0);
      Point2D_d p2(1.0,2.0);
      Point2D_d p3(-1.0,-2.0);
      TS_ASSERT((p1!=p3) && !(p1!=p2));
      }

    void testPointIndexOperator()
      {
      Point2D_d p1(1.0,2.0);
      TS_ASSERT(p1[0]==1.0 && p1[1]==2.0);
      }

    void testPointIntegerMult()
      {
      Point2D<int> p1(1,2);
      Point2D<int> p_scalar=p1*1.5;
      TS_ASSERT_EQUALS(p_scalar, Point2D<int>(1,3));
      }

    void testPointIntegerDiv()
      {
      Point2D<int> p1(1,2);
      Point2D<int> p_div=p1/2;
      TS_ASSERT_EQUALS(p_div, Point2D<int>(0,1));
      }

    void testPointOutputStream()
      {
      Point2D_d p1(1.1,-2);
      std::stringstream sstream;
      sstream << p1;
      std::string s(sstream.str().c_str());
      TS_ASSERT(s=="1.1 -2");
      }

    void testPointInputStream()
      {
      std::stringstream sstream;
      sstream << "1.1 -2.2";
      Point2D_d p_read;
      sstream >> p_read;
      TS_ASSERT_EQUALS(p_read, Point2D_d(1.1,-2.2));
      }

    void testPointConversion()
      {
      Point2D_d p1(1.8,-2.3);
      Point2D<int> p_int=Convert<int>(p1);
      TS_ASSERT_EQUALS(p_int, Point2D<int>(1,-2));
      }
  };

#endif // POINT2D_TEST_H