#include "RandomSampler.h"

////////////////////////////////////////////// RandomSampler ///////////////////////////////////////////////////

RandomSampler::RandomSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel):
Sampler(i_image_begin, i_image_end, i_samples_per_pixel)
  {
  }

RandomSampler::RandomSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel, intrusive_ptr<ImagePixelsOrder> ip_pixels_order):
Sampler(i_image_begin, i_image_end, i_samples_per_pixel, ip_pixels_order)
  {
  }

size_t RandomSampler::_RoundSamplesNumber(size_t i_samples_number) const
  {
  return i_samples_number;
  }

intrusive_ptr<SubSampler> RandomSampler::_CreateSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng) const
  {
  return intrusive_ptr<SubSampler>( new RandomSubSampler(i_pixels, i_samples_per_pixel, ip_rng) );
  }

///////////////////////////////////////////// RandomSubSampler //////////////////////////////////////////////////

RandomSubSampler::RandomSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng): SubSampler(i_pixels, i_samples_per_pixel, ip_rng)
  {
  ASSERT(i_samples_per_pixel > 0);
  m_inv_samples_per_pixel_sqrt = 1.0 / sqrt((double)i_samples_per_pixel);
  }

void RandomSubSampler::_GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, intrusive_ptr<Sample> op_sample)
  {
  RandomGenerator<double> *p_rng = _GetRandomGenerator();
  ASSERT(p_rng);

  op_sample->SetImagePoint( Convert<double>(i_current_pixel) + Point2D_d( (*p_rng)(1.0), (*p_rng)(1.0) ) );
  op_sample->SetImageFilterWidth(m_inv_samples_per_pixel_sqrt, m_inv_samples_per_pixel_sqrt);
  op_sample->SetLensUV( Point2D_d( (*p_rng)(1.0), (*p_rng)(1.0) ) );

  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences1D();++i)
    {
    SamplesSequence1D sequence = op_sample->GetSamplesSequence1D(i);
    for(SamplesSequence1D::Iterator it = sequence.m_begin; it != sequence.m_end; ++it)
      *it = (*p_rng)(1.0);
    }

  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences2D();++i)
    {
    SamplesSequence2D sequence = op_sample->GetSamplesSequence2D(i);
    for(SamplesSequence2D::Iterator it = sequence.m_begin; it != sequence.m_end; ++it)
      *it = Point2D_d( (*p_rng)(1.0), (*p_rng)(1.0) );
    }
  }