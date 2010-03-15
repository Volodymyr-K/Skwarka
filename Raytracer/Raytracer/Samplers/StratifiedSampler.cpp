#include "StratifiedSampler.h"
#include <Math/SamplingRoutines.h>

//////////////////////////////////////////// StratifiedSampler /////////////////////////////////////////////////

StratifiedSampler::StratifiedSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_x_samples_per_pixel, size_t i_y_samples_per_pixel):
Sampler(i_image_begin, i_image_end, i_x_samples_per_pixel*i_y_samples_per_pixel),
m_x_samples_per_pixel(i_x_samples_per_pixel), m_y_samples_per_pixel(i_y_samples_per_pixel)
  {
  }

StratifiedSampler::StratifiedSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_x_samples_per_pixel, size_t i_y_samples_per_pixel,
                                     intrusive_ptr<ImagePixelsOrder> ip_pixels_order):
Sampler(i_image_begin, i_image_end, i_x_samples_per_pixel*i_y_samples_per_pixel, ip_pixels_order),
m_x_samples_per_pixel(i_x_samples_per_pixel), m_y_samples_per_pixel(i_y_samples_per_pixel)
  {
  }

size_t StratifiedSampler::_RoundSamplesNumber(size_t i_samples_number) const
  {
  size_t sqrt_ceil = (size_t) ceil(sqrt((double)i_samples_number));
  return sqrt_ceil*sqrt_ceil;
  }

intrusive_ptr<SubSampler> StratifiedSampler::_CreateSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng) const
  {
  return intrusive_ptr<SubSampler>( new StratifiedSubSampler(i_pixels, m_x_samples_per_pixel, m_y_samples_per_pixel, ip_rng) );
  }

/////////////////////////////////////////// StratifiedSubSampler ////////////////////////////////////////////////

StratifiedSubSampler::StratifiedSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_x_samples_per_pixel,
                                           size_t i_y_samples_per_pixel, RandomGenerator<double> *ip_rng):
SubSampler(i_pixels, i_x_samples_per_pixel*i_y_samples_per_pixel, ip_rng), m_x_samples_per_pixel(i_x_samples_per_pixel), m_y_samples_per_pixel(i_y_samples_per_pixel), 
m_image_points(i_x_samples_per_pixel*i_y_samples_per_pixel), m_lens_UVs(i_x_samples_per_pixel*i_y_samples_per_pixel)
  {
  ASSERT(i_x_samples_per_pixel>0 && i_y_samples_per_pixel>0);

  m_inv_x_samples_per_pixel = 1.0/i_x_samples_per_pixel;
  m_inv_y_samples_per_pixel = 1.0/i_y_samples_per_pixel;
  }

void StratifiedSubSampler::_GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, intrusive_ptr<Sample> op_sample)
  {
  RandomGenerator<double> *p_rng = _GetRandomGenerator();
  ASSERT(p_rng);

  op_sample->SetImagePoint( m_image_points[i_pixel_sample_index] );
  op_sample->SetImageFilterWidth(m_inv_x_samples_per_pixel, m_inv_y_samples_per_pixel);
  op_sample->SetLensUV( m_lens_UVs[i_pixel_sample_index] );

  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences1D();++i)
    {
    SamplesSequence1D sequence = op_sample->GetSamplesSequence1D(i);
    SamplingRoutines::StratifiedSampling1D(sequence.m_begin, std::distance(sequence.m_begin, sequence.m_end), true, p_rng);

    // Shuffle samples to de-correlate them from other sample sequences.
    SamplingRoutines::Shuffle(sequence.m_begin, std::distance(sequence.m_begin, sequence.m_end), p_rng);
    }

  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences2D();++i)
    {
    SamplesSequence2D sequence = op_sample->GetSamplesSequence2D(i);

    size_t samples_num_sqrt = (size_t) sqrt((double)std::distance(sequence.m_begin, sequence.m_end));
    ASSERT(samples_num_sqrt*samples_num_sqrt == std::distance(sequence.m_begin, sequence.m_end));

    SamplingRoutines::StratifiedSampling2D(sequence.m_begin, samples_num_sqrt, samples_num_sqrt, true, p_rng);

    // Shuffle samples to de-correlate them from other sample sequences.
    SamplingRoutines::Shuffle(sequence.m_begin, std::distance(sequence.m_begin, sequence.m_end), p_rng);
    }
  }

void StratifiedSubSampler::_PrecomputePixelSamples(const Point2D_i &i_current_pixel)
  {
  RandomGenerator<double> *p_rng = _GetRandomGenerator();

  SamplingRoutines::StratifiedSampling2D(m_image_points.begin(), m_x_samples_per_pixel, m_y_samples_per_pixel, true, p_rng);

  // Translate the image samples.
  for (size_t i = 0; i < m_x_samples_per_pixel*m_y_samples_per_pixel; ++i)
    m_image_points[i] += Convert<double>(i_current_pixel);

  SamplingRoutines::StratifiedSampling2D(m_lens_UVs.begin(), m_x_samples_per_pixel, m_y_samples_per_pixel, true, p_rng);

  // De-correlate image and lens samples.
  SamplingRoutines::Shuffle(m_lens_UVs.begin(), m_lens_UVs.size(), p_rng);
  }