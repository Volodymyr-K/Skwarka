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

    void test_Sampler_GeneratedSamplesCount()
      {
      SamplerMock sampler(Point2D_i(0,0),Point2D_i(100,100),10);
      intrusive_ptr<Sample> sample = sampler.CreateSample();

      size_t count=0;
      while(sampler.GetNextSample(sample))
        ++count;

      TS_ASSERT_EQUALS(count, sampler.GetTotalSamplesNum());
      }

    void test_Sampler_Reset()
      {
      SamplerMock sampler(Point2D_i(0,0),Point2D_i(100,100),10);
      intrusive_ptr<Sample> sample = sampler.CreateSample();

      for(size_t i=0;i<1003;++i) sampler.GetNextSample(sample);
      
      //Reset the sampler.
      sampler.Reset();

      size_t count=0;
      while(sampler.GetNextSample(sample))
        ++count;

      TS_ASSERT_EQUALS(count, sampler.GetTotalSamplesNum());
      }
  };

#endif // SAMPLER_TEST_H
