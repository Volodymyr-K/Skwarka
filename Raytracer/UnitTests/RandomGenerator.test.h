#ifndef RANDOM_GENERATOR_TEST_H
#define RANDOM_GENERATOR_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/RandomGenerator.h>

class RandomGeneratorTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      mp_gen = shared_ptr<RandomGenerator<double> > ( new RandomGenerator<double>() );
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void testRandomRange1()
      {
      double mn=DBL_INF,mx=-DBL_INF;
      for(size_t i=0;i<1000;++i)
        {
        double r = (*mp_gen)(123.0);
        if (r<mn) mn=r;
        if (r>mx) mx=r;
        }
      TS_ASSERT(mn>=0.0 && mn<123.0 && mx>=0.0 && mx<123.0);
      }

    void testRandomRange2()
      {
      double mn=DBL_INF,mx=-DBL_INF;
      for(size_t i=0;i<1000;++i)
        {
        // We can use reversed range as well.
        double r = (*mp_gen)(123.0, -10.0);
        if (r<mn) mn=r;
        if (r>mx) mx=r;
        }
      TS_ASSERT(mn>=-10.0 && mn<123.0 && mx>=-10.0 && mx<123.0);
      }

    // Test for mean and variance values.
    void testRandomDistribution()
      {
      double values[1024], mean=0.0;
      for(size_t i=0;i<1024;++i)
        {
        values[i]=(*mp_gen)(1.0,2.0);
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

    // Test that two different instances of the same generator produce the same sequence of random values.
    void testCorrelation()
      {
      shared_ptr<RandomGenerator<double> > mp_gen2 = shared_ptr<RandomGenerator<double> > ( new RandomGenerator<double>() );

      bool match=true;
      for(size_t i=0;i<1000;++i)
        if ( (*mp_gen)(1.0) != (*mp_gen2)(1.0) )
          {
          match=false;
          break;
          }
      TS_ASSERT(match);
      }

    void testSeed()
      {
      shared_ptr<RandomGenerator<double> > mp_gen2 = shared_ptr<RandomGenerator<double> > ( new RandomGenerator<double>() );
      mp_gen2->SetSeed(123);

      bool match=true;
      for(size_t i=0;i<1000;++i)
        if ( (*mp_gen)(1.0) != (*mp_gen2)(1.0) )
          {
          match=false;
          break;
          }
      TS_ASSERT(match==false);
      }

  private:
    shared_ptr<RandomGenerator<double> > mp_gen;
  };

#endif // RANDOM_GENERATOR_TEST_H