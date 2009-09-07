#ifndef THREAD_SAFE_RANDOM_TEST_H
#define THREAD_SAFE_RANDOM_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/ThreadSafeRandom.h>

// This class does not actually test the multi-threaded aspects of the generator, it just tests the random values and distribution in a single thread.
class ThreadSafeRandomTestSuite : public CxxTest::TestSuite
  {
  public:
    void testRandomDoubleRange1()
      {
      double mn=DBL_INF,mx=-DBL_INF;
      for(size_t i=0;i<1000;++i)
        {
        double r = RandomDouble(123.0);
        if (r<mn) mn=r;
        if (r>mx) mx=r;
        }
      TS_ASSERT(mn>=0.0 && mn<123.0 && mx>=0.0 && mx<123.0);
      }

    void testRandomDoubleRange2()
      {
      double mn=DBL_INF,mx=-DBL_INF;
      for(size_t i=0;i<1000;++i)
        {
        // We can use reversed range as well.
        double r = RandomDouble(123.0, -10.0);
        if (r<mn) mn=r;
        if (r>mx) mx=r;
        }
      TS_ASSERT(mn>=-10.0 && mn<123.0 && mx>=-10.0 && mx<123.0);
      }

    void testRandomIntRange1()
      {
      int mn=INT_MAX,mx=INT_MIN;
      for(size_t i=0;i<1000;++i)
        {
        int r = RandomInt(123);
        if (r<mn) mn=r;
        if (r>mx) mx=r;
        }
      TS_ASSERT(mn>=0 && mn<123 && mx>=0 && mx<123);
      }

    void testRandomIntRange2()
      {
      int mn=INT_MAX,mx=INT_MIN;
      for(size_t i=0;i<1000;++i)
        {
        // We can use reversed range as well but nevertheless only the fist parameter is included into the range (i.e. 123 can be generated while 10 should not).
        int r = RandomInt(123, -10);
        if (r<mn) mn=r;
        if (r>mx) mx=r;
        }
      TS_ASSERT(mn>-10 && mn<=123 && mx>-10 && mx<=123);
      }

    // Test for mean and variance values.
    void testRandomDistribution()
      {
      double values[1024], mean=0.0;
      for(size_t i=0;i<1024;++i)
        {
        values[i]=RandomDouble(1.0,2.0);
        mean+=values[i];
        }
      mean/=1024.0;

      double variance=0;
      for(size_t i=0;i<1024;++i)
        variance+=(values[i]-mean)*(values[i]-mean);
      variance/=1024.0;

      // Although the values of mean and variance are naturally random they can hardly break the following barrier for the given number of samples.
      TS_ASSERT_DELTA(mean, 1.5, 0.05);
      TS_ASSERT_DELTA(variance, 1.0/12.0, 0.02);
      }
  };

#endif // THREAD_SAFE_RANDOM_TEST_H