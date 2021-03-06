/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef POINT2D_TEST_H
#define POINT2D_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Point2D.h>
#include <sstream>

class Point2DTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Point2D_DefaultConstr()
      {
      Point2D_d p1;
      TS_ASSERT_EQUALS(p1, Point2D_d(0.0,0.0));
      }

    void test_Point2D_ConstrWithCoords()
      {
      Point2D_d p1(1.1,-2.1);
      TS_ASSERT_EQUALS(p1, Point2D_d(1.1,-2.1));
      }

    void test_Point2D_Add()
      {
      Point2D_d p1(1.5,2.0);
      Point2D_d p2(1.0,0.0);
      Point2D_d p_sum=p1+p2;
      TS_ASSERT_EQUALS(p_sum, Point2D_d(2.5,2.0));
      }

    void test_Point2D_AddAssign()
      {
      Point2D_d p1(1.5,2.0);
      Point2D_d p2(1.0,0.0);
      Point2D_d p_sum=p1;
      p_sum+=p2;
      TS_ASSERT_EQUALS(p_sum, Point2D_d(2.5,2.0));
      }

    void test_Point2D_Sub()
      {
      Point2D_d p1(1.5,2.0);
      Point2D_d p2(1.0,10.0);
      Point2D_d p_sub=p1-p2;
      TS_ASSERT_EQUALS(p_sub, Point2D_d(0.5,-8.0));
      }

    void test_Point2D_SubAssign()
      {
      Point2D_d p1(1.5,2.0);
      Point2D_d p2(1.0,10.0);
      Point2D_d p_sub=p1;
      p_sub-=p2;
      TS_ASSERT_EQUALS(p_sub, Point2D_d(0.5,-8.0));
      }

    void test_Point2D_Mult()
      {
      Point2D_d p1(1.0,2.0);
      Point2D_d p_scalar=p1*1.5;
      TS_ASSERT_EQUALS(p_scalar, Point2D_d(1.5,3.0));
      }

    void test_Point2D_PreMult()
      {
      Point2D_d p1(1.0,2.0);
      Point2D_d p_scalar=1.5*p1;
      TS_ASSERT_EQUALS(p_scalar, Point2D_d(1.5,3.0));
      }

    void test_Point2D_MultAssign()
      {
      Point2D_d p1(1.0,2.0);
      Point2D_d p_scalar=p1;
      p_scalar*=1.5;
      TS_ASSERT_EQUALS(p_scalar, Point2D_d(1.5,3.0));
      }

    void test_Point2D_Div()
      {
      Point2D_d p1(1.0,-2.0);
      Point2D_d p_div=p1/2.0;
      TS_ASSERT_EQUALS(p_div, Point2D_d(0.5,-1.0));
      }

    void test_Point2D_DivAssign()
      {
      Point2D_d p1(1.0,-2.0);
      Point2D_d p_div=p1;
      p_div/=2.0;
      TS_ASSERT_EQUALS(p_div, Point2D_d(0.5,-1.0));
      }

    // Disable compiler warning since we divide by zero intentionally.
#pragma warning( push )
#pragma warning( disable : 4723 )
    void test_Point2D_DivByZero()
      {
      Point2D_d p1(-1.0,2.0);
      Point2D_d p_div=p1/0.0;
      TS_ASSERT_EQUALS(p_div, Point2D_d(-DBL_INF,DBL_INF));
      }
#pragma warning( pop )

    void test_Point2D_Equal()
      {
      Point2D_d p1(1.0,2.0);
      Point2D_d p2(1.0,2.0);
      Point2D_d p3(-1.0,-2.0);
      TS_ASSERT((p1==p2) && !(p1==p3));
      }

    void test_Point2D_NotEqual()
      {
      Point2D_d p1(1.0,2.0);
      Point2D_d p2(1.0,2.0);
      Point2D_d p3(-1.0,-2.0);
      TS_ASSERT((p1!=p3) && !(p1!=p2));
      }

    void test_Point2D_IndexOperator()
      {
      Point2D_d p1(1.0,2.0);
      TS_ASSERT(p1[0]==1.0 && p1[1]==2.0);
      }

    void test_Point2D_IntegerMult()
      {
      Point2D<int> p1(1,2);
      Point2D<int> p_scalar=p1*1.5;
      TS_ASSERT_EQUALS(p_scalar, Point2D<int>(1,3));
      }

    void test_Point2D_IntegerDiv()
      {
      Point2D<int> p1(1,2);
      Point2D<int> p_div=p1/2;
      TS_ASSERT_EQUALS(p_div, Point2D<int>(0,1));
      }

    void test_Point2D_OutputStream()
      {
      Point2D_d p1(1.1,-2);
      std::stringstream sstream;
      sstream << p1;
      TS_ASSERT(sstream.str() == "1.1 -2");
      }

    void test_Point2D_InputStream()
      {
      std::stringstream sstream;
      sstream << "1.1 -2.2";
      Point2D_d p_read;
      sstream >> p_read;
      TS_ASSERT_EQUALS(p_read, Point2D_d(1.1,-2.2));
      }

    void test_Point2D_Conversion()
      {
      Point2D_d p1(1.8,-2.3);
      Point2D<int> p_int=Convert<int>(p1);
      TS_ASSERT_EQUALS(p_int, Point2D<int>(1,-2));
      }
  };

#endif // POINT2D_TEST_H