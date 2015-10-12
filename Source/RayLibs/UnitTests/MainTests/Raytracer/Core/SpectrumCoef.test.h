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

#ifndef SPECTRUM_TEST_H
#define SPECTRUM_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/SpectrumCoef.h>
#include <limits>

class SpectrumCoefTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_SpectrumCoef_DefaultConstr()
      {
      SpectrumCoef_d s1;
      TS_ASSERT_EQUALS(s1, SpectrumCoef_d(0.0,0.0,0.0));
      }

    void test_SpectrumCoef_ConstrWithValue()
      {
      SpectrumCoef_d s1(1.1);
      TS_ASSERT_EQUALS(s1, SpectrumCoef_d(1.1,1.1,1.1));
      }

    void test_SpectrumCoef_ConstrWithCoords()
      {
      SpectrumCoef_d s1(1.1,2.1,-3.1);
      TS_ASSERT_EQUALS(s1, SpectrumCoef_d(1.1,2.1,-3.1));
      }

    void test_SpectrumCoef_Add()
      {
      SpectrumCoef_d s1(1.5,2.0,3.0);
      SpectrumCoef_d s2(1.0,0.0,2.0);
      SpectrumCoef_d s_sum=s1+s2;
      TS_ASSERT_EQUALS(s_sum, SpectrumCoef_d(2.5,2.0,5.0));
      }

    void test_SpectrumCoef_AddAssign()
      {
      SpectrumCoef_d s1(1.5,2.0,3.0);
      SpectrumCoef_d s2(1.0,0.0,2.0);
      SpectrumCoef_d s_sum=s1;
      s_sum+=s2;
      TS_ASSERT_EQUALS(s_sum, SpectrumCoef_d(2.5,2.0,5.0));
      }

    void test_SpectrumCoef_Sub()
      {
      SpectrumCoef_d s1(1.5,2.0, 3.0);
      SpectrumCoef_d s2(1.0,10.0,2.0);
      SpectrumCoef_d s_sub=s1-s2;
      TS_ASSERT_EQUALS(s_sub, SpectrumCoef_d(0.5,-8.0,1.0));
      }

    void test_SpectrumCoef_SubAssign()
      {
      SpectrumCoef_d s1(1.5,2.0, 3.0);
      SpectrumCoef_d s2(1.0,10.0,2.0);
      SpectrumCoef_d s_sub=s1;
      s_sub-=s2;
      TS_ASSERT_EQUALS(s_sub, SpectrumCoef_d(0.5,-8.0,1.0));
      }

    void test_SpectrumCoef_Mult()
      {
      SpectrumCoef_d s1(1.5,2.0, 3.0);
      SpectrumCoef_d s2(1.0,-10.0,2.0);
      SpectrumCoef_d s_mult=s1*s2;
      TS_ASSERT_EQUALS(s_mult, SpectrumCoef_d(1.5,-20.0,6.0));
      }

    void test_SpectrumCoef_MultAssign()
      {
      SpectrumCoef_d s1(1.5,2.0, 3.0);
      SpectrumCoef_d s2(1.0,10.0,2.0);
      SpectrumCoef_d s_mult=s1;
      s_mult*=s2;
      TS_ASSERT_EQUALS(s_mult, SpectrumCoef_d(1.5,20.0,6.0));
      }

    void test_SpectrumCoef_Div()
      {
      SpectrumCoef_d s1(1.5,2.0, 3.0);
      SpectrumCoef_d s2(1.0,-10.0,2.0);
      SpectrumCoef_d s_div=s1/s2;
      TS_ASSERT_EQUALS(s_div, SpectrumCoef_d(1.5,-0.2,1.5));
      }

    void test_SpectrumCoef_DivAssign()
      {
      SpectrumCoef_d s1(1.5,2.0, 3.0);
      SpectrumCoef_d s2(1.0,10.0,2.0);
      SpectrumCoef_d s_div=s1;
      s_div/=s2;
      TS_ASSERT_EQUALS(s_div, SpectrumCoef_d(1.5,0.2,1.5));
      }

    void test_SpectrumCoef_MultScalar()
      {
      SpectrumCoef_d s1(1.0,2.0,3.0);
      SpectrumCoef_d s_scalar=s1*1.5;
      TS_ASSERT_EQUALS(s_scalar, SpectrumCoef_d(1.5,3.0,4.5));
      }

    void test_SpectrumCoef_PreMultScalar()
      {
      SpectrumCoef_d s1(1.0,2.0,3.0);
      SpectrumCoef_d s_scalar=1.5*s1;
      TS_ASSERT_EQUALS(s_scalar, SpectrumCoef_d(1.5,3.0,4.5));
      }

    void test_SpectrumCoef_MultScalarAssign()
      {
      SpectrumCoef_d s1(1.0,2.0,3.0);
      SpectrumCoef_d s_scalar=s1;
      s_scalar*=1.5;
      TS_ASSERT_EQUALS(s_scalar, SpectrumCoef_d(1.5,3.0,4.5));
      }

    void test_SpectrumCoef_DivScalar()
      {
      SpectrumCoef_d s1(1.0,2.0,3.0);
      SpectrumCoef_d s_div=s1/2.0;
      TS_ASSERT_EQUALS(s_div, SpectrumCoef_d(0.5,1.0,1.5));
      }

    void test_SpectrumCoef_DivScalarAssign()
      {
      SpectrumCoef_d s1(1.0,2.0,3.0);
      SpectrumCoef_d s_div=s1;
      s_div/=2.0;
      TS_ASSERT_EQUALS(s_div, SpectrumCoef_d(0.5,1.0,1.5));
      }

    void test_SpectrumCoef_Equal()
      {
      SpectrumCoef_d s1(1.0,2.0,3.0);
      SpectrumCoef_d s2(1.0,2.0,3.0);
      SpectrumCoef_d p3(-1.0,-2.0,-3.0);
      TS_ASSERT((s1==s2) && !(s1==p3));
      }

    void test_SpectrumCoef_NotEqual()
      {
      SpectrumCoef_d s1(1.0,2.0,3.0);
      SpectrumCoef_d s2(1.0,2.0,3.0);
      SpectrumCoef_d p3(-1.0,-2.0,-3.0);
      TS_ASSERT((s1!=p3) && !(s1!=s2));
      }

    void test_SpectrumCoef_IndexOperator()
      {
      SpectrumCoef_d s1(1.0,2.0,3.0);
      TS_ASSERT(s1[0]==1.0 && s1[1]==2.0 && s1[2]==3.0);
      }

    void test_SpectrumCoef_Clamp()
      {
      SpectrumCoef_d s1(1.0,2.0,3.0);
      s1.Clamp(1.5,2.5);
      TS_ASSERT_EQUALS(s1, SpectrumCoef_d(1.5,2.0,2.5));
      }

    void test_SpectrumCoef_AddWeighted()
      {
      SpectrumCoef_d s1(1.5,2.0,3.0);
      SpectrumCoef_d s2(1.0,0.0,2.0);
      s1.AddWeighted(s2,1.5);
      TS_ASSERT_EQUALS(s1, SpectrumCoef_d(3.0,2.0,6.0));
      }

    void test_SpectrumCoef_IsBlack()
      {
      SpectrumCoef_d black(0.0,0.0,0.0);
      SpectrumCoef_d non_black(0.0,0.0,-1.0);
      TS_ASSERT(black.IsBlack());
      TS_ASSERT(non_black.IsBlack()==false);
      }

    void test_SpectrumCoef_OutputStream()
      {
      SpectrumCoef_d s1(1.1,-2.2,3.0);
      std::stringstream sstream;
      sstream << s1;
      TS_ASSERT(sstream.str() == "1.1 -2.2 3");
      }

    void test_SpectrumCoef_InputStream()
      {
      std::stringstream sstream;
      sstream << "1.1 -2.2 3.0";
      SpectrumCoef_d s_read;
      sstream >> s_read;
      TS_ASSERT_EQUALS(s_read, SpectrumCoef_d(1.1,-2.2,3.0));
      }

    void test_SpectrumCoef_Conversion()
      {
      SpectrumCoef_d s1(1.8,-2.3,3.0);
      SpectrumCoef<float> s_float=Convert<float>(s1);
      TS_ASSERT_EQUALS(s_float, SpectrumCoef<float>(1.8f,-2.3f,3.0f));
      }

    void test_SpectrumCoef_InRange()
      {
      SpectrumCoef_d s1(0.0,0.2,1.0);
      SpectrumCoef_d s2(-1.0,5.2,0.0);

      TS_ASSERT(InRange(s1,0.0,1.0));
      TS_ASSERT(InRange(s2,0.0,10.0)==false);
      TS_ASSERT(InRange(s2,-10.0,4.0)==false);
      }

    void test_SpectrumCoef_IsNaN()
      {
      SpectrumCoef_d s;

      // We can't just initialize SpectrumCoef with NaN values because asserts will come up.
      // We use potentially unsafe trick with the reinterpret_cast.
      double *p_r=reinterpret_cast<double *>(&s);
      *p_r=std::numeric_limits<double>::quiet_NaN();

      TS_ASSERT(IsNaN(s));
      }

    void test_SpectrumCoef_IsInf1()
      {
      SpectrumCoef_d s(0,1,2);

      TS_ASSERT(IsInf(s)==false);
      }

    void test_SpectrumCoef_IsInf2()
      {
      SpectrumCoef_d s(0,DBL_INF,2);

      TS_ASSERT(IsInf(s));
      }
  };

#endif // SPECTRUM_TEST_H