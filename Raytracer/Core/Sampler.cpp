#include "Sampler.h"

/////////////////////////////////////////////// SAMPLER ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

Sampler::Sampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel):
m_image_begin(i_image_begin), m_image_end(i_image_end), m_samples_per_pixel(i_samples_per_pixel)
  {
  ASSERT(i_image_end[0]>=i_image_begin[0]);
  ASSERT(i_image_end[1]>=i_image_begin[1]);
  ASSERT(m_samples_per_pixel>=1);

  m_pixel_sample_index=0;
  m_current_pixel=i_image_begin;
  }

size_t Sampler::AddSamplesSequence1D(size_t i_size)
  {
  m_sequences_1D_size.push_back( _RoundSamplesNumber(i_size) );
  return m_sequences_1D_size.size()-1;
  }

size_t Sampler::AddSamplesSequence2D(size_t i_size)
  {
  m_sequences_2D_size.push_back( _RoundSamplesNumber(i_size) );
  return m_sequences_2D_size.size()-1;
  }

shared_ptr<Sample> Sampler::CreateSample() const
  {
  return shared_ptr<Sample>( new Sample(m_sequences_1D_size, m_sequences_2D_size) );
  }

bool Sampler::GetNextSample(shared_ptr<Sample> op_sample)
  {
  ASSERT(m_sequences_1D_size.size() == op_sample->GetNumberOfSamplesSequences1D() && "Sample has wrong number of 1D samples sequences.");
  ASSERT(m_sequences_2D_size.size() == op_sample->GetNumberOfSamplesSequences2D() && "Sample has wrong number of 2D samples sequences.");

  if (m_pixel_sample_index==m_samples_per_pixel)
    {
    if (++m_current_pixel[0]>=m_image_end[0])
      {
      m_current_pixel[0]=m_image_begin[0];
      if (++m_current_pixel[1]>=m_image_end[1])
        return false;
      }

    m_pixel_sample_index=0;
    }

  _GetSample(m_current_pixel, m_pixel_sample_index, op_sample);

  ++m_pixel_sample_index;
  return true;
  }

size_t Sampler::GetTotalSamplesNum() const
  {
  return (m_image_end[0]-m_image_begin[0])*(m_image_end[1]-m_image_begin[1])*m_samples_per_pixel;
  }

Sampler::~Sampler()
  {
  }

//////////////////////////////////////////// RANDOMSAMPLER ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

RandomSampler::RandomSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel):
  Sampler(i_image_begin, i_image_end, i_samples_per_pixel)
  {
  }

size_t RandomSampler::_RoundSamplesNumber(size_t i_samples_number)
  {
  return i_samples_number;
  }

void RandomSampler::_GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, shared_ptr<Sample> op_sample)
  {
  op_sample->SetImagePoint( Convert<double>(i_current_pixel) + Point2D_d(m_random_generator(1.0), m_random_generator(1.0)) );
  op_sample->SetLensUV( Point2D_d(m_random_generator(1.0), m_random_generator(1.0)) );
  
  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences1D();++i)
    {
    SamplesSequence1D sequence = op_sample->GetSamplesSequence1D(i);
    for(SamplesSequence1D::IteratorType it = sequence.m_begin; it != sequence.m_end; ++it)
      *it=m_random_generator(1.0);
    }

  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences2D();++i)
    {
    SamplesSequence2D sequence = op_sample->GetSamplesSequence2D(i);
    for(SamplesSequence2D::IteratorType it = sequence.m_begin; it != sequence.m_end; ++it)
      *it=std::make_pair(m_random_generator(1.0), m_random_generator(1.0));
    }
  }