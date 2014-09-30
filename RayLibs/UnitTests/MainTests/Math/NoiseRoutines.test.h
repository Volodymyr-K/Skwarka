#ifndef NOISE_ROUTINES_TEST_H
#define NOISE_ROUTINES_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/ThreadSafeRandom.h>
#include <Math/NoiseRoutines.h>

class NoiseTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_PerlinNoise_Range()
      {
      double mn=DBL_INF, mx=-DBL_INF;
      for (size_t i=0; i<10000; ++i)
        {
        Point3D_d point(RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0);
        double val = NoiseRoutines::PerlinNoise(point);
        if (val>mx) mx=val;
        if (val<mn) mn=val;
        }

      ASSERT(mn>=-1.0 && mn<=1.0);
      ASSERT(mx>=-1.0 && mx<=1.0);
      }

    void test_FBm_Range()
      {
      double mn=DBL_INF, mx=-DBL_INF;
      for (size_t i=0; i<10000; ++i)
        {
        Point3D_d point(RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0);
        double val = NoiseRoutines::FBm(point, Vector3D_d(0.1, 0.1, 0.0), Vector3D_d(-0.1, 0.1, 0.0), 0.5, 10);
        if (val>mx) mx=val;
        if (val<mn) mn=val;
        }

      ASSERT(mn>=-2.0 && mn<=2.0); // theoretical limit for omega=0.5
      ASSERT(mx>=-2.0 && mx<=2.0);
      }

    void test_Turbulence_Range()
      {
      double mn=DBL_INF, mx=-DBL_INF;
      for (size_t i=0; i<10000; ++i)
        {
        Point3D_d point(RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0);
        double val = NoiseRoutines::Turbulence(point, Vector3D_d(0.1, 0.1, 0.0), Vector3D_d(-0.1, 0.1, 0.0), 0.5, 10);
        if (val>mx) mx=val;
        if (val<mn) mn=val;
        }

      ASSERT(mn>=0.0 && mn<=2.0); // theoretical limit for omega=0.5
      ASSERT(mx>=0.0 && mx<=2.0);
      }

  };

#endif // NOISE_ROUTINES_TEST_H