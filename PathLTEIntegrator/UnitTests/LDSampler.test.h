#ifndef LD_SAMPLER_TEST_H
#define LD_SAMPLER_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/Sampler.h>
#include <Raytracer/Samplers/LDSampler.h>
#include <Math/RandomGenerator.h>
#include "SamplingTestHelper.h"
#include <vector>

class LDSamplerTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_LDSampler_Constr()
      {
      LDSampler sampler(Point2D_i(0,0), Point2D_i(100,100), 7);

      // The LD sampler rounds up number of pixels samples to the nearest power of the 2.
      TS_ASSERT_EQUALS(sampler.GetTotalSamplesNum(), 100*100*8);
      }

    void test_LDSampler_ImagePoints()
      {
      const int image_size_x=3, image_size_y=3;
      intrusive_ptr<Sampler> p_sampler(new LDSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 64) );

      bool out_of_range=false, low_discrepancy=true;
      std::vector<Point2D_d> points;

      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();
      while(intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng))
        while(p_sub_sampler->GetNextSample(p_sample))
          {
          Point2D_d point = p_sample->GetImagePoint();
          points.push_back(point);
          if (point[0]<0.0 || point[0]>=image_size_x || point[1]<0.0 || point[1]>=image_size_y) out_of_range=true;

          // Here we exploit the fact that the sampler generates samples by pixels,
          // so that first group of samples is for the first pixel, the next group is all for the other pixel and so on.
          if (points.size()==64)
            {
            Point2D_d pixel((int)points[0][0], (int)points[0][1]);
            low_discrepancy = low_discrepancy && SamplingTestHelper::TestLD02Distribution2D(points, pixel, pixel+Point2D_d(1.0,1.0));
            points.clear();
            }
          }

      TS_ASSERT(low_discrepancy);
      TS_ASSERT(out_of_range==false);
      }

    void test_LDSampler_LensUVs()
      {
      const int image_size_x=3, image_size_y=3;
      intrusive_ptr<Sampler> p_sampler(new LDSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 64) );

      bool out_of_range=false, low_discrepancy=true;

      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();
      std::vector<Point2D_d> UVs;
      while(intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng))
        while(p_sub_sampler->GetNextSample(p_sample))
          {
          Point2D_d point = p_sample->GetLensUV();
          UVs.push_back(point);
          if (point[0]<0.0 || point[0]>=image_size_x || point[1]<0.0 || point[1]>=image_size_y) out_of_range=true;

          // Here we exploit the fact that the sampler generates samples by pixels,
          // so that first group of samples is for the first pixel, the next group is all for the other pixel and so on.
          if (UVs.size()==64)
            {
            Point2D_d pixel((int)UVs[0][0], (int)UVs[0][1]);
            low_discrepancy = low_discrepancy && SamplingTestHelper::TestLD02Distribution2D(UVs, pixel, pixel+Point2D_d(1.0,1.0));
            UVs.clear();
            }
          }

        TS_ASSERT(low_discrepancy);
        TS_ASSERT(out_of_range==false);
      }

    void test_LDSampler_1DSequences()
      {
      const int image_size_x=2, image_size_y=2;
      intrusive_ptr<Sampler> p_sampler(new LDSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 4) );

      size_t actual_size;
      size_t index=p_sampler->AddSamplesSequence1D(1000, &actual_size);
      TS_ASSERT(actual_size == 1024); // Low-discrepancy sampler rounds to the next power of 2.

      bool out_of_range=false;
      bool not_clumped=true;

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

          // Since the low-discrepancy samples are shuffled we can not test their stratification for sub-sequences so we can only test the stratification
          // for the entire samples sequence.
          not_clumped = not_clumped && SamplingTestHelper::TestSamplesClumping1D(values, 0.0, 1.0);
          if (not_clumped==false) break;
          }

      TS_ASSERT(not_clumped);
      TS_ASSERT(out_of_range==false);
      }

    void test_LDSampler_2DSequences()
      {
      const int image_size_x=2, image_size_y=2;
      intrusive_ptr<Sampler> p_sampler(new LDSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 4) );

      size_t actual_size;
      size_t index=p_sampler->AddSamplesSequence2D(1000, &actual_size);
      TS_ASSERT(actual_size == 1024); // Low-discrepancy sampler rounds to the next power of 2.

      bool out_of_range=false;
      bool low_discrepancy=true;

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

          low_discrepancy = low_discrepancy && SamplingTestHelper::TestLD02Distribution2D(values, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0));
          if (low_discrepancy==false) break;
          }

      TS_ASSERT(low_discrepancy);
      TS_ASSERT(out_of_range==false);
      }

  private:
    RandomGenerator<double> m_rng;
  };

#endif // LD_SAMPLER_TEST_H