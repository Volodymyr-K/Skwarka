/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef STRATIFIED_SAMPLER_TEST_H
#define STRATIFIED_SAMPLER_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/Sampler.h>
#include <Raytracer/Samplers/StratifiedSampler.h>
#include <Math/RandomGenerator.h>
#include <UnitTests/TestHelpers/SamplingTestHelper.h>
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
      intrusive_ptr<Sampler> p_sampler(new StratifiedSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 11, 15) );

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
      intrusive_ptr<Sampler> p_sampler(new StratifiedSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 11, 15) );

      bool out_of_range=false;
      bool not_clumped=true, uniform=true;

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
      intrusive_ptr<Sampler> p_sampler(new StratifiedSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 2, 3) );

      size_t actual_size;
      size_t index=p_sampler->AddSamplesSequence1D(1000, &actual_size);
      TS_ASSERT(actual_size == 1024); // Stratified sampler rounds to the next square integer.

      bool out_of_range=false;
      bool not_clumped=true, uniform=true;

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
      intrusive_ptr<Sampler> p_sampler(new StratifiedSampler(Point2D_i(0,0), Point2D_i(image_size_x,image_size_y), 2, 3) );

      size_t actual_size;
      size_t index=p_sampler->AddSamplesSequence2D(1000, &actual_size);
      TS_ASSERT(actual_size == 1024); // Stratified sampler rounds to the next square integer.

      bool out_of_range=false;
      bool not_clumped=true, uniform=true;

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
          not_clumped = not_clumped && SamplingTestHelper::TestSamplesClumping2D(values, Point2D_d(0.0,0.0), Point2D_d(1.0,1.0), 32, 32);
          if (uniform==false || not_clumped==false) break;
          }

      TS_ASSERT(uniform);
      TS_ASSERT(not_clumped);
      TS_ASSERT(out_of_range==false);
      }

private:
  RandomGenerator<double> m_rng;

  };

#endif // STRATIFIED_SAMPLER_TEST_H
