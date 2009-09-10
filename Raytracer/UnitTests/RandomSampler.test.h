#ifndef RANDOM_SAMPLER_TEST_H
#define RANDOM_SAMPLER_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/Sampler.h>
#include <Raytracer/Samplers/RandomSampler.h>
#include "SamplingTestRoutines.h"
#include <vector>

class RandomSamplerTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_RandomSampler_Constr()
      {
      RandomSampler sampler(Point2D_i(0,0), Point2D_i(100,100), 10);
      TS_ASSERT_EQUALS(sampler.GetTotalSamplesNum(), 100*100*10);
      }

    void test_RandomSampler_ImagePoints()
      {
      const int image_size_x=10, image_size_y=10;
      RandomSampler sampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 20);

      bool out_of_range=false;
      std::vector<Point2D_d> points;

      shared_ptr<Sample> sample = sampler.CreateSample();
      while(sampler.GetNextSample(sample))
        {
        Point2D_d point = sample->GetImagePoint();
        points.push_back(point);
        if (point[0]<0.0 || point[0]>=image_size_x || point[1]<0.0 || point[1]>=image_size_y) out_of_range=true;
        }

      bool uniform = SamplingTestRoutines::TestUniformDistribution2D(points, Point2D_d(0.0,0.0), Point2D_d(image_size_x,image_size_y));
      TS_ASSERT(uniform);
      TS_ASSERT(out_of_range==false);
      }

    void test_RandomSampler_LensUVs()
      {
      const int image_size_x=10, image_size_y=10;
      RandomSampler sampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 20);

      bool out_of_range=false;
      bool uniform=true;

      shared_ptr<Sample> sample = sampler.CreateSample();

      std::vector<Point2D_d> UVs;
      while(sampler.GetNextSample(sample))
        {
        Point2D_d point = sample->GetLensUV();
        UVs.push_back(point);
        if (point[0]<0.0 || point[0]>1.0 || point[1]<0.0 || point[1]>1.0) out_of_range=true;

        // Here we exploit the fact that the sampler generates samples by pixels,
        // so that first group of samples is for the first pixel, the next group is all for the other pixel and so on.
        if (UVs.size()==20)
          {
          uniform = uniform && SamplingTestRoutines::TestUniformDistribution2D(UVs, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0));
          UVs.clear();
          }
        }

      TS_ASSERT(uniform);
      TS_ASSERT(out_of_range==false);
      }

    void test_RandomSampler_1DSequences()
      {
      const int image_size_x=5, image_size_y=5;
      RandomSampler sampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 2);

      size_t index=sampler.AddSamplesSequence1D(5000);

      bool out_of_range=false;
      bool uniform=true;

      shared_ptr<Sample> sample = sampler.CreateSample();
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
        
        uniform = uniform && SamplingTestRoutines::TestUniformDistribution1D(values, 0.0, 1.0);
        if (uniform==false) break;
        }

      TS_ASSERT(uniform);
      TS_ASSERT(out_of_range==false);
      }

    void test_RandomSampler_2DSequences()
      {
      const int image_size_x=5, image_size_y=5;
      RandomSampler sampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 2);

      size_t index=sampler.AddSamplesSequence2D(5000);

      bool out_of_range=false;
      bool uniform=true;

      shared_ptr<Sample> sample = sampler.CreateSample();
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

        uniform = uniform && SamplingTestRoutines::TestUniformDistribution2D(values, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0));
        if (uniform==false) break;
        }

      TS_ASSERT(uniform);
      TS_ASSERT(out_of_range==false);
      }

  };

#endif // RANDOM_SAMPLER_TEST_H
