#ifndef RANDOM_SAMPLER_TEST_H
#define RANDOM_SAMPLER_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/Sampler.h>
#include <Raytracer/Samplers/RandomSampler.h>
#include <Math/RandomGenerator.h>
#include "SamplingTestHelper.h"
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
      intrusive_ptr<Sampler> p_sampler(new RandomSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 20) );

      bool out_of_range=false;
      std::vector<Point2D_d> points;

      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();

      while(intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng))
        while(p_sub_sampler->GetNextSample(p_sample))
          {
          Point2D_d point = p_sample->GetImagePoint();
          points.push_back(point);
          if (point[0]<0.0 || point[0]>=image_size_x || point[1]<0.0 || point[1]>=image_size_y) out_of_range=true;
          }

      bool uniform = SamplingTestHelper::TestUniformDistribution2D(points, Point2D_d(0.0,0.0), Point2D_d(image_size_x,image_size_y));
      TS_ASSERT(uniform);
      TS_ASSERT(out_of_range==false);
      }

    void test_RandomSampler_LensUVs()
      {
      const int image_size_x=10, image_size_y=10;
      intrusive_ptr<Sampler> p_sampler(new RandomSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 20) );

      bool out_of_range=false;
      bool uniform=true;

      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();

      std::vector<Point2D_d> UVs;
      while(intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng))
        while(p_sub_sampler->GetNextSample(p_sample))
          {
          Point2D_d point = p_sample->GetLensUV();
          UVs.push_back(point);
          if (point[0]<0.0 || point[0]>1.0 || point[1]<0.0 || point[1]>1.0) out_of_range=true;

          // Here we exploit the fact that the sampler generates samples by pixels,
          // so that first group of samples is for the first pixel, the next group is all for the other pixel and so on.
          if (UVs.size()==20)
            {
            uniform = uniform && SamplingTestHelper::TestUniformDistribution2D(UVs, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0));
            UVs.clear();
            }
          }

      TS_ASSERT(uniform);
      TS_ASSERT(out_of_range==false);
      }

    void test_RandomSampler_1DSequences()
      {
      const int image_size_x=5, image_size_y=5;
      intrusive_ptr<Sampler> p_sampler(new RandomSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 2) );

      size_t actual_size;
      size_t index=p_sampler->AddSamplesSequence1D(5000, &actual_size);
      TS_ASSERT(actual_size == 5000); // Random sampler does not change the size.

      bool out_of_range=false;
      bool uniform=true;

      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();

      while(intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng))
        while(p_sub_sampler->GetNextSample(p_sample))
          {
          std::vector<double> values;

          SamplesSequence1D sequence = p_sample->GetSamplesSequence1D(index);
          for(SamplesSequence1D::Iterator it = sequence.m_begin;it!=sequence.m_end;++it)
            {
            double value=*it;
            if (value<0.0 || value>1.0) out_of_range=true;
            values.push_back(value);
            }
          
          uniform = uniform && SamplingTestHelper::TestUniformDistribution1D(values, 0.0, 1.0);
          if (uniform==false) break;
          }

      TS_ASSERT(uniform);
      TS_ASSERT(out_of_range==false);
      }

    void test_RandomSampler_2DSequences()
      {
      const int image_size_x=5, image_size_y=5;
      intrusive_ptr<Sampler> p_sampler(new RandomSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 2) );

      size_t actual_size;
      size_t index=p_sampler->AddSamplesSequence2D(5000, &actual_size);
      TS_ASSERT(actual_size == 5000); // Random sampler does not change the size.

      bool out_of_range=false;
      bool uniform=true;

      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();

      while(intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng))
        while(p_sub_sampler->GetNextSample(p_sample))
          {
          std::vector<Point2D_d> values;

          SamplesSequence2D sequence = p_sample->GetSamplesSequence2D(index);
          for(SamplesSequence2D::Iterator it = sequence.m_begin;it!=sequence.m_end;++it)
            {
            Point2D_d value=*it;
            if (value[0]<0.0 || value[0]>1.0 || value[1]<0.0 || value[1]>1.0) out_of_range=true;
            values.push_back(value);
            }

          uniform = uniform && SamplingTestHelper::TestUniformDistribution2D(values, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0));
          if (uniform==false) break;
          }

      TS_ASSERT(uniform);
      TS_ASSERT(out_of_range==false);
      }

private:
  RandomGenerator<double> m_rng;
  };

#endif // RANDOM_SAMPLER_TEST_H
