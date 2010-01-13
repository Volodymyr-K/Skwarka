#ifndef SAMPLER_TEST_H
#define SAMPLER_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/Sampler.h>
#include <UnitTests/Mocks/SamplerMock.h>
#include <vector>

class SamplerTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Sampler_Constr()
      {
      SamplerMock sampler(Point2D_i(0,0),Point2D_i(100,100),10);

      TS_ASSERT_EQUALS(sampler.GetTotalSamplesNum(), 100*100*10);
      }

    void test_Sampler_CreateSample()
      {
      SamplerMock sampler(Point2D_i(0,0),Point2D_i(100,100),10);

      size_t indices[3];
      indices[0]=sampler.AddSamplesSequence1D(15);
      indices[1]=sampler.AddSamplesSequence2D(25);
      indices[2]=sampler.AddSamplesSequence1D(35);

      intrusive_ptr<Sample> sample = sampler.CreateSample();

      TS_ASSERT_EQUALS(sample->GetNumberOfSamplesSequences1D(),2);
      TS_ASSERT_EQUALS(sample->GetNumberOfSamplesSequences2D(),1);

      size_t lengths[3];
      lengths[0]=std::distance(sample->GetSamplesSequence1D(indices[0]).m_begin, sample->GetSamplesSequence1D(indices[0]).m_end);
      lengths[1]=std::distance(sample->GetSamplesSequence2D(indices[1]).m_begin, sample->GetSamplesSequence2D(indices[1]).m_end);
      lengths[2]=std::distance(sample->GetSamplesSequence1D(indices[2]).m_begin, sample->GetSamplesSequence1D(indices[2]).m_end);

      TS_ASSERT_EQUALS(lengths[0],15);
      TS_ASSERT_EQUALS(lengths[1],25);
      TS_ASSERT_EQUALS(lengths[2],35);
      }

    void test_Sampler_ClearSamplesSequences()
      {
      SamplerMock sampler(Point2D_i(0,0),Point2D_i(100,100),10);

      size_t indices[3];
      indices[0]=sampler.AddSamplesSequence1D(15);
      indices[1]=sampler.AddSamplesSequence2D(25);

      // Clear the sequences.
      sampler.ClearSamplesSequences();

      indices[2]=sampler.AddSamplesSequence1D(35);

      intrusive_ptr<Sample> sample = sampler.CreateSample();

      TS_ASSERT_EQUALS(sample->GetNumberOfSamplesSequences1D(),1);
      TS_ASSERT_EQUALS(sample->GetNumberOfSamplesSequences2D(),0);

      size_t lengths[3];
      lengths[0]=std::distance(sample->GetSamplesSequence1D(indices[0]).m_begin, sample->GetSamplesSequence1D(indices[0]).m_end);

      TS_ASSERT_EQUALS(lengths[0],35);
      }

    void test_SubSampler_TotalSampelsNum()
      {
      intrusive_ptr<Sampler> p_sampler(new SamplerMock(Point2D_i(0,0),Point2D_i(100,100),10) );
      intrusive_ptr<Sample> sample = p_sampler->CreateSample();

      intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng);

      TS_ASSERT_EQUALS(p_sub_sampler->GetTotalSamplesNum(), 16*10);
      }

    void test_SubSampler_GeneratedSamplesCount1()
      {
      intrusive_ptr<Sampler> p_sampler(new SamplerMock(Point2D_i(0,0),Point2D_i(100,100),10) );
      intrusive_ptr<Sample> sample = p_sampler->CreateSample();

      size_t count=0;
      intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng);
      while(p_sub_sampler->GetNextSample(sample))
        ++count;

      TS_ASSERT_EQUALS(count, p_sub_sampler->GetTotalSamplesNum());
      }

    // Tests when there are less total pixels than the sub sampler requests.
    void test_SubSampler_GeneratedSamplesCount2()
      {
      intrusive_ptr<Sampler> p_sampler(new SamplerMock(Point2D_i(0,0),Point2D_i(100,100),10) );
      intrusive_ptr<Sample> sample = p_sampler->CreateSample();

      size_t count=0;
      intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(1000000, &m_rng);
      while(p_sub_sampler->GetNextSample(sample))
        ++count;

      TS_ASSERT_EQUALS(count, p_sub_sampler->GetTotalSamplesNum());
      }

    void test_Sampler_GeneratedSamplesCount()
      {
      intrusive_ptr<Sampler> p_sampler(new SamplerMock(Point2D_i(0,0),Point2D_i(100,100),10) );
      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();

      size_t count=0;
      while(intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng))
        while(p_sub_sampler->GetNextSample(p_sample))
          ++count;

      TS_ASSERT_EQUALS(count, p_sampler->GetTotalSamplesNum());
      }

    void test_SubSampler_Reset()
      {
      intrusive_ptr<Sampler> p_sampler(new SamplerMock(Point2D_i(0,0),Point2D_i(100,100),10) );
      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();

      intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng);
      for(size_t i=0;i<1003;++i) p_sub_sampler->GetNextSample(p_sample);

      //Reset the sub sampler.
      p_sub_sampler->Reset();

      size_t count=0;
      while(p_sub_sampler->GetNextSample(p_sample))
        ++count;

      TS_ASSERT_EQUALS(count, p_sub_sampler->GetTotalSamplesNum());
      }

    void test_Sampler_Reset()
      {
      intrusive_ptr<Sampler> p_sampler(new SamplerMock(Point2D_i(0,0),Point2D_i(100,100),10) );
      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();

      intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng);
      for(size_t i=0;i<1003;++i) p_sub_sampler->GetNextSample(p_sample);
      
      //Reset the sampler.
      p_sampler->Reset();

      size_t count=0;
      while(intrusive_ptr<SubSampler> p_sub_sampler = p_sampler->GetNextSubSampler(16, &m_rng))
        while(p_sub_sampler->GetNextSample(p_sample))
          ++count;

      TS_ASSERT_EQUALS(count, p_sampler->GetTotalSamplesNum());
      }

private:
  RandomGenerator<double> m_rng;
  };

#endif // SAMPLER_TEST_H
