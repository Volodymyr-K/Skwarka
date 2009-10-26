#ifndef SAMPLING_ROUTINES_TEST_H
#define SAMPLING_ROUTINES_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/SamplingRoutines.h>
#include <Math/ThreadSafeRandom.h>
#include "SamplingTestHelper.h"
#include <cmath>
#include <vector>
#include <algorithm>

class SamplingRoutinesTestSuite : public CxxTest::TestSuite
  {
  public:

    // Test that ConcentricDiskSampling() generates points within the unit radius disk.
    void test_ConcentricDiskSampling_RadiusRange()
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
    void test_ConcentricDiskSampling_Covering()
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
        
        double min_dist=DBL_INF;
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
      TS_ASSERT(max_dist<0.06); // Empirical threshold for the given number of samples.
      }

    void test_UniformHemisphereSampling_Range()
      {
      const size_t num_samples = 1000;

      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d point(RandomDouble(1.0),RandomDouble(1.0));
        Vector3D_d sampled = SamplingRoutines::UniformHemisphereSampling(point);
        double radius = sqrt(sampled[0]*sampled[0]+sampled[1]*sampled[1]+sampled[2]*sampled[2]);
        if (fabs(radius-1.0)>DBL_EPS)
          {
          TS_FAIL("Direction vector is not normalized.");
          break;
          }
        if (sampled[2]<0.0)
          {
          TS_FAIL("Z coordinate is negative.");
          break;
          }
        }
      }

    void test_UniformHemispherePDF()
      {
      double pdf = SamplingRoutines::UniformHemispherePDF();
      TS_ASSERT_EQUALS(pdf, INV_2PI);
      }

    void test_UniformSphereSampling_Range()
      {
      const size_t num_samples = 1000;

      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d point(RandomDouble(1.0),RandomDouble(1.0));
        Vector3D_d sampled = SamplingRoutines::UniformSphereSampling(point);
        double radius = sqrt(sampled[0]*sampled[0]+sampled[1]*sampled[1]+sampled[2]*sampled[2]);
        if (fabs(radius-1.0)>DBL_EPS)
          {
          TS_FAIL("Direction vector is not normalized.");
          break;
          }
        }
      }

    void test_UniformSpherePDF()
      {
      double pdf = SamplingRoutines::UniformSpherePDF();
      TS_ASSERT_EQUALS(pdf, 1.0/(4.0*M_PI));
      }

    void test_CosineHemisphereSampling_Range()
      {
      const size_t num_samples = 1000;

      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d point(RandomDouble(1.0),RandomDouble(1.0));
        Vector3D_d sampled = SamplingRoutines::CosineHemisphereSampling(point);
        double radius = sqrt(sampled[0]*sampled[0]+sampled[1]*sampled[1]+sampled[2]*sampled[2]);
        if (fabs(radius-1.0)>DBL_EPS)
          {
          TS_FAIL("Direction vector is not normalized.");
          break;
          }
        if (sampled[2]<0.0)
          {
          TS_FAIL("Z coordinate is negative.");
          break;
          }
        }
      }

    void test_CosineHemispherePDF()
      {
      const size_t num_samples = 1000;

      for(size_t i=0;i<num_samples;++i)
        {
        double cos_theta=cos(RandomDouble(M_PI_2));
        double pdf = SamplingRoutines::CosineHemispherePDF(cos_theta);
        if (fabs(pdf-cos_theta*INV_PI)>DBL_EPS)
          {
          TS_FAIL("PDF is incorrect.");
          break;
          }
        }
      }

    void test_UniformTriangleSampling_Range()
      {
      const size_t num_samples = 1000;

      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d point(RandomDouble(1.0),RandomDouble(1.0));
        double b1,b2;
        SamplingRoutines::UniformTriangleSampling(point,b1,b2);
        if (b1<0.0 || b2<0.0 || b1+b2>1.0)
          {
          TS_FAIL("Barycentric coordinates are out of range.");
          break;
          }
        }
      }

    // Test that StratifiedSampling1D() generates samples in the whole [0;1] range.
    void test_Stratified1D_Range()
      {
      const size_t num_samples = 5000;

      double mn=DBL_INF,mx=-DBL_INF;
      std::vector<double> samples(num_samples);
      SamplingRoutines::StratifiedSampling1D(samples.begin(),num_samples,true);
      for(size_t i=0;i<samples.size();++i)
        {
        if (samples[i]>mx) mx=samples[i];
        if (samples[i]<mn) mn=samples[i];
        }
      TS_ASSERT(mn>=0.0 && mn<=1.0/num_samples && mx<=1.0 && mx>=1.0-1.0/num_samples);
      }

    // Test that StratifiedSampling1D() does not generate clumping samples.
    void test_Stratified1D_Clumping()
      {
      const size_t num_samples = 5000;
      std::vector<double> samples(num_samples);
      SamplingRoutines::StratifiedSampling1D(samples.begin(),num_samples,true);

      bool not_clumped = SamplingTestHelper::TestSamplesClumping1D(samples, 0.0, 1.0);
      TS_ASSERT(not_clumped);
      }

    // Test that StratifiedSampling2D() generates samples in the whole [0;1]^2 range.
    void test_Stratified2D_Range()
      {
      const int x_samples = 60, y_samples=70;

      Point2D_d mn=Point2D_d(DBL_INF,DBL_INF),mx=Point2D_d(-DBL_INF,-DBL_INF);
      std::vector<Point2D_d> samples(x_samples*y_samples);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),x_samples,y_samples,true);
      for(size_t i=0;i<samples.size();++i)
        {
        if (samples[i][0]>mx[0]) mx[0]=samples[i][0];
        if (samples[i][1]>mx[1]) mx[1]=samples[i][1];

        if (samples[i][0]<mn[0]) mn[0]=samples[i][0];
        if (samples[i][1]<mn[1]) mn[1]=samples[i][1];
        }

      bool whole_range_covered = mn[0]>=0.0 && mn[0]<=1.0/x_samples && mn[1]>=0.0 && mn[1]<=1.0/y_samples;
      whole_range_covered = whole_range_covered && mx[0]<=1.0 && mx[0]>=1.0-1.0/x_samples && mx[1]<=1.0 && mx[1]>=1.0-1.0/y_samples;
      TS_ASSERT(whole_range_covered);
      }

    // Test that StratifiedSampling2D() does not generate clumping samples.
    void test_Stratified2DClumping()
      {
      const int x_samples = 60, y_samples=70;
      std::vector<Point2D_d> samples(x_samples*y_samples);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),x_samples,y_samples,true);

      bool not_clumped = SamplingTestHelper::TestSamplesClumping2D(samples, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0), x_samples, y_samples);
      TS_ASSERT(not_clumped);
      }

    // Test that LatinHypercubeSampling2D() generates samples in the whole [0;1]^2 range.
    void test_LatinHypercube2D_Range()
      {
      const size_t num_samples = 5000;

      Point2D_d mn=Point2D_d(DBL_INF,DBL_INF),mx=Point2D_d(-DBL_INF,-DBL_INF);
      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);
      for(size_t i=0;i<samples.size();++i)
        {
        if (samples[i][0]>mx[0]) mx[0]=samples[i][0];
        if (samples[i][1]>mx[1]) mx[1]=samples[i][1];

        if (samples[i][0]<mn[0]) mn[0]=samples[i][0];
        if (samples[i][1]<mn[1]) mn[1]=samples[i][1];
        }

      bool whole_range_covered = mn[0]>=0.0 && mn[0]<=1.0/num_samples && mn[1]>=0.0 && mn[1]<=1.0/num_samples;
      whole_range_covered = whole_range_covered && mx[0]<=1.0 && mx[0]>=1.0-1.0/num_samples && mx[1]<=1.0 && mx[1]>=1.0-1.0/num_samples;
      TS_ASSERT(whole_range_covered);
      }

    // Test that LatinHypercubeSampling2D() does not generate clumping samples.
    void test_LatinHypercube2D_Clumping()
      {
      const size_t num_samples = 5000;

      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);

      bool not_clumped = SamplingTestHelper::TestLatinHypercubeDistribution2D(samples, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0));
      TS_ASSERT(not_clumped);
      }

    void test_Shuffle()
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

    void test_BalanceHeuristic()
      {
      TS_ASSERT_EQUALS(SamplingRoutines::BalanceHeuristic(10,0.2,0,0.8), 1.0);
      TS_ASSERT_EQUALS(SamplingRoutines::BalanceHeuristic(0,0.2,10,0.8), 0.0);
      TS_ASSERT_EQUALS(SamplingRoutines::BalanceHeuristic(10,0.2,10,0.8), 0.2);
      }

    void test_PowerHeuristic()
      {
      TS_ASSERT_EQUALS(SamplingRoutines::PowerHeuristic(10,0.2,0,0.8), 1.0);
      TS_ASSERT_EQUALS(SamplingRoutines::PowerHeuristic(0,0.2,10,0.8), 0.0);
      TS_ASSERT_EQUALS(SamplingRoutines::PowerHeuristic(10,0.2,10,0.8), 0.2*0.2/(0.2*0.2+0.8*0.8));
      }
  };

#endif // SAMPLING_ROUTINES_TEST_H