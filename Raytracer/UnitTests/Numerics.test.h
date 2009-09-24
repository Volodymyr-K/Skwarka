#ifndef NUMERICS_TEST_H
#define NUMERICS_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/Numerics.h>
#include <Math/Constants.h>
#include <limits>

// Disable compiler warning since we divide by zero intentionally.
#pragma warning( push )
#pragma warning( disable : 4723 )

class NumericsTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_IsNaN1()
      {
      double x=0.1;
      TS_ASSERT(IsNaN(x)==false);
      }

    void test_IsNaN2()
      {
      double x=DBL_INF;
      TS_ASSERT(IsNaN(x)==false);
      }

    void test_IsNaN3()
      {
      double x=std::numeric_limits<double>::quiet_NaN();
      TS_ASSERT(IsNaN(x));
      }

    void test_IsNaN4()
      {
      double x=0.0;
      x/=0.0;
      TS_ASSERT(IsNaN(x));
      }

    void test_IsPositiveInf1()
      {
      double x=0.1;
      TS_ASSERT(IsPositiveInf(x)==false);
      }

    void test_IsPositiveInf2()
      {
      double x=std::numeric_limits<double>::quiet_NaN();
      TS_ASSERT(IsPositiveInf(x)==false);
      }

    void test_IsPositiveInf3()
      {
      double x=-0.2;
      x/=0.0;
      TS_ASSERT(IsPositiveInf(x)==false);
      }

    void test_IsPositiveInf4()
      {
      double x=0.2;
      x/=0.0;
      TS_ASSERT(IsPositiveInf(x));
      }

    void test_IsNegativeInf1()
      {
      double x=0.1;
      TS_ASSERT(IsNegativeInf(x)==false);
      }

    void test_IsNegativeInf2()
      {
      double x=std::numeric_limits<double>::quiet_NaN();
      TS_ASSERT(IsNegativeInf(x)==false);
      }

    void test_IsNegativeInf3()
      {
      double x=0.2;
      x/=0.0;
      TS_ASSERT(IsNegativeInf(x)==false);
      }

    void test_IsNegativeInf4()
      {
      double x=-0.2;
      x/=0.0;
      TS_ASSERT(IsNegativeInf(x));
      }

  };

#pragma warning( pop )

#endif // NUMERICS_TEST_H