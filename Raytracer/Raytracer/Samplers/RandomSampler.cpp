#include "RandomSampler.h"
#include <Math/ThreadSafeRandom.h>

RandomSampler::RandomSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel):
Sampler(i_image_begin, i_image_end, i_samples_per_pixel)
  {
  }

RandomSampler::RandomSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel, shared_ptr<ImagePixelsOrder> ip_pixels_order):
Sampler(i_image_begin, i_image_end, i_samples_per_pixel, ip_pixels_order)
  {
  }

size_t RandomSampler::_RoundSamplesNumber(size_t i_samples_number) const
  {
  return i_samples_number;
  }

void RandomSampler::_GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, shared_ptr<Sample> op_sample)
  {
  op_sample->SetImagePoint( Convert<double>(i_current_pixel) + Point2D_d(RandomDouble(1.0), RandomDouble(1.0)) );
  op_sample->SetLensUV( Point2D_d(RandomDouble(1.0), RandomDouble(1.0)) );

  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences1D();++i)
    {
    SamplesSequence1D sequence = op_sample->GetSamplesSequence1D(i);
    for(SamplesSequence1D::IteratorType it = sequence.m_begin; it != sequence.m_end; ++it)
      *it=RandomDouble(1.0);
    }

  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences2D();++i)
    {
    SamplesSequence2D sequence = op_sample->GetSamplesSequence2D(i);
    for(SamplesSequence2D::IteratorType it = sequence.m_begin; it != sequence.m_end; ++it)
      *it=Point2D_d(RandomDouble(1.0), RandomDouble(1.0));
    }
  }