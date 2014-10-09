#ifndef SPECTRUM_TEST_H
#define SPECTRUM_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/Spectrum.h>
#include <limits>

class SpectrumTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Spectrum_DefaultConstr()
      {
      Spectrum_d s1;
      TS_ASSERT_EQUALS(s1, Spectrum_d(0.0,0.0,0.0));
      }

    void test_Spectrum_ConstrWithValue()
      {
      Spectrum_d s1(1.1);
      TS_ASSERT_EQUALS(s1, Spectrum_d(1.1,1.1,1.1));
      }

    void test_Spectrum_ConstrWithCoords()
      {
      Spectrum_d s1(1.1,2.1,-3.1);
      TS_ASSERT_EQUALS(s1, Spectrum_d(1.1,2.1,-3.1));
      }

    void test_Spectrum_Add()
      {
      Spectrum_d s1(1.5,2.0,3.0);
      Spectrum_d s2(1.0,0.0,2.0);
      Spectrum_d s_sum=s1+s2;
      TS_ASSERT_EQUALS(s_sum, Spectrum_d(2.5,2.0,5.0));
      }

    void test_Spectrum_AddAssign()
      {
      Spectrum_d s1(1.5,2.0,3.0);
      Spectrum_d s2(1.0,0.0,2.0);
      Spectrum_d s_sum=s1;
      s_sum+=s2;
      TS_ASSERT_EQUALS(s_sum, Spectrum_d(2.5,2.0,5.0));
      }

    void test_Spectrum_Sub()
      {
      Spectrum_d s1(1.5,2.0, 3.0);
      Spectrum_d s2(1.0,10.0,2.0);
      Spectrum_d s_sub=s1-s2;
      TS_ASSERT_EQUALS(s_sub, Spectrum_d(0.5,-8.0,1.0));
      }

    void test_Spectrum_SubAssign()
      {
      Spectrum_d s1(1.5,2.0, 3.0);
      Spectrum_d s2(1.0,10.0,2.0);
      Spectrum_d s_sub=s1;
      s_sub-=s2;
      TS_ASSERT_EQUALS(s_sub, Spectrum_d(0.5,-8.0,1.0));
      }

    void test_Spectrum_Mult()
      {
      Spectrum_d s1(1.5,2.0, 3.0);
      SpectrumCoef_d s2(1.0,-10.0,2.0);
      Spectrum_d s_mult=s1*s2;
      TS_ASSERT_EQUALS(s_mult, Spectrum_d(1.5,-20.0,6.0));
      }

    void test_Spectrum_MultAssign()
      {
      Spectrum_d s1(1.5,2.0, 3.0);
      SpectrumCoef_d s2(1.0,10.0,2.0);
      Spectrum_d s_mult=s1;
      s_mult*=s2;
      TS_ASSERT_EQUALS(s_mult, Spectrum_d(1.5,20.0,6.0));
      }

    void test_Spectrum_Div()
      {
      Spectrum_d s1(1.5,2.0, 3.0);
      SpectrumCoef_d s2(1.0,-10.0,2.0);
      Spectrum_d s_div=s1/s2;
      TS_ASSERT_EQUALS(s_div, Spectrum_d(1.5,-0.2,1.5));
      }

    void test_Spectrum_DivAssign()
      {
      Spectrum_d s1(1.5,2.0, 3.0);
      SpectrumCoef_d s2(1.0,10.0,2.0);
      Spectrum_d s_div=s1;
      s_div/=s2;
      TS_ASSERT_EQUALS(s_div, Spectrum_d(1.5,0.2,1.5));
      }

    void test_Spectrum_MultScalar()
      {
      Spectrum_d s1(1.0,2.0,3.0);
      Spectrum_d s_scalar=s1*1.5;
      TS_ASSERT_EQUALS(s_scalar, Spectrum_d(1.5,3.0,4.5));
      }

    void test_Spectrum_PreMultScalar()
      {
      Spectrum_d s1(1.0,2.0,3.0);
      Spectrum_d s_scalar=1.5*s1;
      TS_ASSERT_EQUALS(s_scalar, Spectrum_d(1.5,3.0,4.5));
      }

    void test_Spectrum_MultScalarAssign()
      {
      Spectrum_d s1(1.0,2.0,3.0);
      Spectrum_d s_scalar=s1;
      s_scalar*=1.5;
      TS_ASSERT_EQUALS(s_scalar, Spectrum_d(1.5,3.0,4.5));
      }

    void test_Spectrum_DivScalar()
      {
      Spectrum_d s1(1.0,2.0,3.0);
      Spectrum_d s_div=s1/2.0;
      TS_ASSERT_EQUALS(s_div, Spectrum_d(0.5,1.0,1.5));
      }

    void test_Spectrum_DivScalarAssign()
      {
      Spectrum_d s1(1.0,2.0,3.0);
      Spectrum_d s_div=s1;
      s_div/=2.0;
      TS_ASSERT_EQUALS(s_div, Spectrum_d(0.5,1.0,1.5));
      }

    void test_Spectrum_Equal()
      {
      Spectrum_d s1(1.0,2.0,3.0);
      Spectrum_d s2(1.0,2.0,3.0);
      Spectrum_d p3(-1.0,-2.0,-3.0);
      TS_ASSERT((s1==s2) && !(s1==p3));
      }

    void test_Spectrum_NotEqual()
      {
      Spectrum_d s1(1.0,2.0,3.0);
      Spectrum_d s2(1.0,2.0,3.0);
      Spectrum_d p3(-1.0,-2.0,-3.0);
      TS_ASSERT((s1!=p3) && !(s1!=s2));
      }

    void test_Spectrum_IndexOperator()
      {
      Spectrum_d s1(1.0,2.0,3.0);
      TS_ASSERT(s1[0]==1.0 && s1[1]==2.0 && s1[2]==3.0);
      }

    void test_Spectrum_Clamp()
      {
      Spectrum_d s1(1.0,2.0,3.0);
      s1.Clamp(1.5,2.5);
      TS_ASSERT_EQUALS(s1, Spectrum_d(1.5,2.0,2.5));
      }

    void test_Spectrum_AddWeighted()
      {
      Spectrum_d s1(1.5,2.0,3.0);
      Spectrum_d s2(1.0,0.0,2.0);
      s1.AddWeighted(s2,1.5);
      TS_ASSERT_EQUALS(s1, Spectrum_d(3.0,2.0,6.0));
      }

    void test_Spectrum_IsBlack()
      {
      Spectrum_d black(0.0,0.0,0.0);
      Spectrum_d non_black(0.0,0.0,-1.0);
      TS_ASSERT(black.IsBlack());
      TS_ASSERT(non_black.IsBlack()==false);
      }

    void test_Spectrum_OutputStream()
      {
      Spectrum_d s1(1.1,-2.2,3.0);
      std::stringstream sstream;
      sstream << s1;
      std::string s(sstream.str().c_str());
      TS_ASSERT(s=="1.1 -2.2 3");
      }

    void test_Spectrum_InputStream()
      {
      std::stringstream sstream;
      sstream << "1.1 -2.2 3.0";
      Spectrum_d s_read;
      sstream >> s_read;
      TS_ASSERT_EQUALS(s_read, Spectrum_d(1.1,-2.2,3.0));
      }

    void test_Spectrum_Conversion()
      {
      Spectrum_d s1(1.8,-2.3,3.0);
      Spectrum<float> s_float=Convert<float>(s1);
      TS_ASSERT_EQUALS(s_float, Spectrum<float>(1.8f,-2.3f,3.0f));
      }

    void test_Spectrum_InRange()
      {
      Spectrum_d s1(0.0,0.2,1.0);
      Spectrum_d s2(-1.0,5.2,0.0);

      TS_ASSERT(InRange(s1,0.0,1.0));
      TS_ASSERT(InRange(s2,0.0,10.0)==false);
      TS_ASSERT(InRange(s2,-10.0,4.0)==false);
      }

    void test_Spectrum_IsNaN()
      {
      Spectrum_d s;

      // We can't just initialize Spectrum with NaN values because asserts will come up.
      // We use potentially unsafe trick with the reinterpret_cast.
      double *p_r=reinterpret_cast<double *>(&s);
      *p_r=std::numeric_limits<double>::quiet_NaN();

      TS_ASSERT(IsNaN(s));
      }

    void test_Spectrum_IsInf1()
      {
      Spectrum_d s(0,1,2);

      TS_ASSERT(IsInf(s)==false);
      }

    void test_Spectrum_IsInf2()
      {
      Spectrum_d s(0,DBL_INF,2);

      TS_ASSERT(IsInf(s));
      }
  };

#endif // SPECTRUM_TEST_H