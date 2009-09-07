#ifndef SMAPLING_ROUTINES_TEST_H
#define SMAPLING_ROUTINES_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/SamplingRoutines.h>
#include <Math/MultiThreadedRandom.h>
#include <cmath>
#include <vector>
#include <algorithm>

class SamplingRoutinesTestSuite : public CxxTest::TestSuite
  {
  public:

    // Test that ConcentricDiskSampling() generates points within the unit radius disk.
    void testConcentricDiskSamplingRadiusRange(void)
      {
      const size_t num_samples = 10000;

      double max_radius=0.0;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d point(RandomDouble(1.0),RandomDouble(1.0));
        Point2D_d sampled = SamplingRoutines::ConcentricDiskSampling(point);
        double radius = sqrt(sampled[0]*sampled[0]+sampled[1]*sampled[1]);
        if (radius>max_radius)
          max_radius=radius;
        }
      TS_ASSERT(max_radius<=1.0);
      }

    // Test that ConcentricDiskSampling() generates points within the whole unit radius disk.
    // The method randomly generates a number of points and maps them to the disk with ConcentricDiskSampling() method.
    // Then another set of points is generated randomly in the unit radius disk using another sampling algorithm.
    // For each of the point from the second set the minimum distance to the first set is computed. The maximum of these distances is then tested for a certain threshold.
    void testConcentricDiskSamplingCovering(void)
      {
      const size_t num_samples = 8000;

      std::vector<Point2D_d> samples;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d point(RandomDouble(1.0),RandomDouble(1.0));
        samples.push_back(SamplingRoutines::ConcentricDiskSampling(point));
        }

      double max_dist=0.0;
      for(size_t i=0;i<1000;++i)
        {
        Point2D_d point(RandomDouble(1.0),RandomDouble(1.0));
        double radius = sqrt(point[0]);
        double theta = 2.0*M_PI*point[1];
        Point2D_d test_point(radius*cos(theta), radius*sin(theta));
        
        double min_dist=DBL_MAX;
        for(size_t j=0;j<samples.size();++j)
          {
          double dist_sqr = Vector2D_d(test_point-samples[j]).LengthSqr();
          if (dist_sqr<min_dist)
            min_dist=dist_sqr;
          }
        if (min_dist>max_dist)
          max_dist=min_dist;
        }

      max_dist=sqrt(max_dist);
      TS_ASSERT(max_dist<0.05); // Empirical threshold for the given number of samples.
      }

    // Test that testStratified1DRange() generates samples in [0;1] range.
    void testStratified1DRange(void)
      {
      const size_t num_samples = 10000;

      double mn=DBL_MAX,mx=-DBL_MAX;
      std::vector<double> samples(num_samples);
      SamplingRoutines::StratifiedSampling1D(samples.begin(),num_samples,true);
      for(size_t i=0;i<samples.size();++i)
        {
        if (samples[i]>mx) mx=samples[i];
        if (samples[i]<mn) mn=samples[i];
        }
      TS_ASSERT(mn>=0.0 && mx<=1.0);
      }

    // Test that testStratified1DCovering() generates points within the whole [0;1] range.
    // The idea is similar to the one used in testConcentricDiskSamplingCovering().
    void testStratified1DCovering(void)
      {
      const size_t num_samples = 10000;

      std::vector<double> samples(num_samples);
      SamplingRoutines::StratifiedSampling1D(samples.begin(),num_samples,true);

      double max_dist=0.0;
      for(size_t i=0;i<1000;++i)
        {
        double sample = RandomDouble(1.0);

        double min_dist=DBL_MAX;
        for(size_t j=0;j<samples.size();++j)
          {
          double dist_sqr = fabs(sample-samples[j]);
          if (dist_sqr<min_dist)
            min_dist=dist_sqr;
          }
        if (min_dist>max_dist)
          max_dist=min_dist;
        }

      TS_ASSERT(max_dist<1.0/num_samples);
      }

    // Test that testStratified2DRange() generates samples in [0;1]^2 range.
    void testStratified2DRange(void)
      {
      const int x_samples = 90, y_samples=110;

      Point2D_d mn=Point2D_d(DBL_MAX,DBL_MAX),mx=Point2D_d(-DBL_MAX,-DBL_MAX);
      std::vector<Point2D_d> samples(x_samples*y_samples);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),x_samples,y_samples,true);
      for(size_t i=0;i<samples.size();++i)
        {
        if (samples[i][0]>mx[0]) mx[0]=samples[i][0];
        if (samples[i][1]>mx[1]) mx[1]=samples[i][1];

        if (samples[i][0]<mn[0]) mn[0]=samples[i][0];
        if (samples[i][1]<mn[1]) mn[1]=samples[i][1];
        }
      TS_ASSERT(mn[0]>=0.0 && mx[0]<=1.0 && mn[1]>=0.0 && mx[1]<=1.0);
      }

    // Test that testStratified2DCovering() generates points within the whole [0;1]^2 range.
    // The idea is similar to the one used in testConcentricDiskSamplingCovering().
    void testStratified2DCovering(void)
      {
      const int x_samples = 90, y_samples=110;

      std::vector<Point2D_d> samples(x_samples*y_samples);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),x_samples,y_samples,true);

      double max_dist=0.0;
      for(size_t i=0;i<1000;++i)
        {
        Point2D_d sample = Point2D_d(RandomDouble(1.0),RandomDouble(1.0));

        double min_dist=DBL_MAX;
        for(size_t j=0;j<samples.size();++j)
          {
          double dist_sqr = Vector2D_d(sample-samples[j]).LengthSqr();
          if (dist_sqr<min_dist)
            min_dist=dist_sqr;
          }
        if (min_dist>max_dist)
          max_dist=min_dist;
        }

      max_dist=sqrt(max_dist);
      double theoretical_max_dist=sqrt( (1.0/x_samples)*(1.0/x_samples) + (1.0/y_samples)*(1.0/y_samples) );
      TS_ASSERT(max_dist<theoretical_max_dist);
      }

    // Test that LatinHypercubeSampling2D() generates samples in [0;1]^2 range.
    void testLatinHypercube2DRange(void)
      {
      const size_t num_samples = 10000;

      Point2D_d mn=Point2D_d(DBL_MAX,DBL_MAX),mx=Point2D_d(-DBL_MAX,-DBL_MAX);
      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);
      for(size_t i=0;i<samples.size();++i)
        {
        if (samples[i][0]>mx[0]) mx[0]=samples[i][0];
        if (samples[i][1]>mx[1]) mx[1]=samples[i][1];

        if (samples[i][0]<mn[0]) mn[0]=samples[i][0];
        if (samples[i][1]<mn[1]) mn[1]=samples[i][1];
        }
      TS_ASSERT(mn[0]>=0.0 && mx[0]<=1.0 && mn[1]>=0.0 && mx[1]<=1.0);
      }

    // Test that LatinHypercubeSampling2D() generates points within the whole [0;1]^2 range.
    // The idea is similar to the one used in testConcentricDiskSamplingCovering().
    void testLatinHypercube2DCovering(void)
      {
      const size_t num_samples = 10000;

      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);

      double max_dist=0.0;
      for(size_t i=0;i<1000;++i)
        {
        Point2D_d sample = Point2D_d(RandomDouble(1.0),RandomDouble(1.0));

        double min_dist=DBL_MAX;
        for(size_t j=0;j<samples.size();++j)
          {
          double dist_sqr = Vector2D_d(sample-samples[j]).LengthSqr();
          if (dist_sqr<min_dist)
            min_dist=dist_sqr;
          }
        if (min_dist>max_dist)
          max_dist=min_dist;
        }

      max_dist=sqrt(max_dist);
      TS_ASSERT(max_dist<0.025); // Empirical threshold for the given number of samples.
      }

    void testShuffle(void)
      {
      const size_t num_samples = 10000;

      std::vector<int> samples;
      for(size_t i=0;i<num_samples;++i)
        samples.push_back(i);

      std::vector<int> shuffled(samples.begin(), samples.end());
      SamplingRoutines::Shuffle(shuffled);

      double distance=0;
      for(size_t i=0;i<num_samples;++i)
        distance+=fabs((double)i-shuffled[i]);

      double normalized_distance = distance/num_samples/num_samples;

      // Mean value for the distance is 1/3 (since the distribution is uniform).
      // We use empirical value 0.31 which is a good threshold for the given number of samples.
      TS_ASSERT(normalized_distance>0.31);

      std::sort(shuffled.begin(), shuffled.end());
      TS_ASSERT(samples==shuffled); // Make sure that shuffle does not change values, only permutes them.
      }
  };

#endif // SMAPLING_ROUTINES_TEST_H