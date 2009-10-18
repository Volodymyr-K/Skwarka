#ifndef STRATIFIED_SAMPLER_TEST_H
#define STRATIFIED_SAMPLER_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/Sampler.h>
#include <Raytracer/Samplers/StratifiedSampler.h>
#include "SamplingTestHelper.h"
#include <vector>

class StratifiedSamplerTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_StratifiedSampler_Constr()
      {
      StratifiedSampler sampler(Point2D_i(0,0), Point2D_i(100,100), 3, 4);
      TS_ASSERT_EQUALS(sampler.GetTotalSamplesNum(), 100*100*3*4);
      }

    void test_StratifiedSampler_ImagePoints()
      {
      const int image_size_x=3, image_size_y=3;
      StratifiedSampler sampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 11, 15);

      bool out_of_range=false;
      std::vector<Point2D_d> points;

      intrusive_ptr<Sample> sample = sampler.CreateSample();
      while(sampler.GetNextSample(sample))
        {
        Point2D_d point = sample->GetImagePoint();
        points.push_back(point);
        if (point[0]<0.0 || point[0]>=image_size_x || point[1]<0.0 || point[1]>=image_size_y) out_of_range=true;
        }

      // Stratified points should be uniformly distributed and should not clump at the same time.
      bool uniform = SamplingTestHelper::TestUniformDistribution2D(points, Point2D_d(0.0,0.0), Point2D_d(image_size_x,image_size_y));
      bool not_clumped = SamplingTestHelper::TestSamplesClumping2D(points, Point2D_d(0.0,0.0), Point2D_d(image_size_x,image_size_y), image_size_x*11, image_size_y*15);
      TS_ASSERT(uniform);
      TS_ASSERT(not_clumped);
      TS_ASSERT(out_of_range==false);
      }

    void test_StratifiedSampler_LensUVs()
      {
      const int image_size_x=3, image_size_y=3;
      StratifiedSampler sampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 11, 15);

      bool out_of_range=false;
      bool not_clumped=true, uniform=true;

      intrusive_ptr<Sample> sample = sampler.CreateSample();

      std::vector<Point2D_d> UVs;
      while(sampler.GetNextSample(sample))
        {
        Point2D_d point = sample->GetLensUV();
        UVs.push_back(point);
        if (point[0]<0.0 || point[0]>1.0 || point[1]<0.0 || point[1]>1.0) out_of_range=true;

        // Here we exploit the fact that the sampler generates samples by pixels,
        // so that first group of samples is for the first pixel, the next group is all for the other pixel and so on.
        if (UVs.size()==11*15)
          {
          uniform = uniform && SamplingTestHelper::TestUniformDistribution2D(UVs, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0));
          not_clumped = not_clumped && SamplingTestHelper::TestSamplesClumping2D(UVs, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0), 11, 15);
          UVs.clear();
          }
        }

      // Stratified points should be uniformly distributed and should not clump at the same time.
      TS_ASSERT(uniform);
      TS_ASSERT(not_clumped);
      TS_ASSERT(out_of_range==false);
      }

    void test_StratifiedSampler_1DSequences()
      {
      const int image_size_x=2, image_size_y=2;
      StratifiedSampler sampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 2, 3);

      size_t index=sampler.AddSamplesSequence1D(1000);

      bool out_of_range=false;
      bool not_clumped=true, uniform=true;

      intrusive_ptr<Sample> sample = sampler.CreateSample();
      while(sampler.GetNextSample(sample))
        {
        std::vector<double> values;

        SamplesSequence1D sequence = sample->GetSamplesSequence1D(index);
        for(SamplesSequence1D::IteratorType it = sequence.m_begin;it!=sequence.m_end;++it)
          {
          double value=*it;
          if (value<0.0 || value>1.0) out_of_range=true;
          values.push_back(value);
          }

        uniform = uniform && SamplingTestHelper::TestUniformDistribution1D(values, 0.0, 1.0);
        not_clumped = not_clumped && SamplingTestHelper::TestSamplesClumping1D(values, 0.0, 1.0);
        if (uniform==false || not_clumped==false) break;
        }

      TS_ASSERT(uniform);
      TS_ASSERT(not_clumped);
      TS_ASSERT(out_of_range==false);
      }

    void test_StratifiedSampler_2DSequences()
      {
      const int image_size_x=2, image_size_y=2;
      StratifiedSampler sampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 2, 3);

      size_t index=sampler.AddSamplesSequence2D(1000);

      bool out_of_range=false;
      bool not_clumped=true, uniform=true;

      intrusive_ptr<Sample> sample = sampler.CreateSample();
      while(sampler.GetNextSample(sample))
        {
        std::vector<Point2D_d> values;

        SamplesSequence2D sequence = sample->GetSamplesSequence2D(index);
        for(SamplesSequence2D::IteratorType it = sequence.m_begin;it!=sequence.m_end;++it)
          {
          Point2D_d value=*it;
          if (value[0]<0.0 || value[0]>1.0 || value[1]<0.0 || value[1]>1.0) out_of_range=true;
          values.push_back(value);
          }

        uniform = uniform && SamplingTestHelper::TestUniformDistribution2D(values, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0));
        not_clumped = not_clumped && SamplingTestHelper::TestLatinHypercubeDistribution2D(values, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0));
        if (uniform==false || not_clumped==false) break;
        }

      TS_ASSERT(uniform);
      TS_ASSERT(not_clumped);
      TS_ASSERT(out_of_range==false);
      }

  };

#endif // STRATIFIED_SAMPLER_TEST_H
