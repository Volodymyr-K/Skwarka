#ifndef RANDOM_GENERATOR_TEST_H
#define RANDOM_GENERATOR_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/RandomGenerator.h>
#include "SamplingTestHelper.h"
#include <vector>

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

    void test_RandomRange1()
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

    void test_RandomRange2()
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
    void test_RandomDistribution()
      {
      std::vector<double> values;
      for(size_t i=0;i<1000;++i)
        values.push_back( (*mp_gen)(1.0,2.0) );

      bool uniform=SamplingTestHelper::TestUniformDistribution1D(values, 1.0, 2.0);
      TS_ASSERT(uniform);
      }

    // Test that two different instances of the same generator produce the same sequence of random values.
    void test_Correlation()
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

    void test_Seed()
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