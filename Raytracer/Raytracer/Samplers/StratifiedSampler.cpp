#include "StratifiedSampler.h"
#include <Math\SamplingRoutines.h>

StratifiedSampler::StratifiedSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_x_samples_per_pixel, size_t i_y_samples_per_pixel, bool i_jitter_samples):
Sampler(i_image_begin, i_image_end, i_x_samples_per_pixel*i_y_samples_per_pixel),
m_x_samples_per_pixel(i_x_samples_per_pixel), m_y_samples_per_pixel(i_y_samples_per_pixel), m_jitter_samples(i_jitter_samples),
m_image_points(i_x_samples_per_pixel*i_y_samples_per_pixel), m_lens_UVs(i_x_samples_per_pixel*i_y_samples_per_pixel)
  {
  }

size_t StratifiedSampler::_RoundSamplesNumber(size_t i_samples_number) const
  {
  return i_samples_number;
  }

void StratifiedSampler::_PrecomputeSamplesForPixel(const Point2D_i &i_current_pixel)
  {
  SamplingRoutines::StratifiedSampling2D(m_image_points.begin(), m_x_samples_per_pixel, m_y_samples_per_pixel, m_jitter_samples);
  for (size_t i = 0; i < m_x_samples_per_pixel*m_y_samples_per_pixel; ++i)
    m_image_points[i]+=Convert<double>(i_current_pixel);

  SamplingRoutines::StratifiedSampling2D(m_lens_UVs.begin(), m_x_samples_per_pixel, m_y_samples_per_pixel, m_jitter_samples);
  SamplingRoutines::Shuffle(m_lens_UVs);
  }

void StratifiedSampler::_GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, shared_ptr<Sample> op_sample)
  {
  op_sample->SetImagePoint( m_image_points[i_pixel_sample_index] );
  op_sample->SetLensUV( m_lens_UVs[i_pixel_sample_index] );

  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences1D();++i)
    {
    SamplesSequence1D sequence = op_sample->GetSamplesSequence1D(i);
    SamplingRoutines::StratifiedSampling1D(sequence.m_begin, std::distance(sequence.m_begin, sequence.m_end), true);
    }

  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences2D();++i)
    {
    SamplesSequence2D sequence = op_sample->GetSamplesSequence2D(i);
    SamplingRoutines::LatinHypercubeSampling2D(sequence.m_begin, std::distance(sequence.m_begin, sequence.m_end), true);
    }
  }