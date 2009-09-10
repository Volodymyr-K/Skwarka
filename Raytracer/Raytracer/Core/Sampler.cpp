#include "Sampler.h"
#include <Raytracer/Samplers/ConsecutiveImagePixelsOrder.h>

Sampler::Sampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel):
m_samples_per_pixel(i_samples_per_pixel)
  {
  ASSERT(m_samples_per_pixel>=1);

  // ConsecutiveImagePixelsOrder implementation is used by default.
  mp_pixels_order.reset(new ConsecutiveImagePixelsOrder());
  mp_pixels_order->SetImageSize(i_image_begin, i_image_end);

  if (mp_pixels_order->GetNextPixel(m_current_pixel))
    m_pixel_sample_index=0;
  else
    m_pixel_sample_index=m_samples_per_pixel;
  }

Sampler::Sampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel, shared_ptr<ImagePixelsOrder> ip_pixels_order):
m_samples_per_pixel(i_samples_per_pixel), mp_pixels_order(ip_pixels_order)
  {
  ASSERT(m_samples_per_pixel>=1);
  ASSERT(mp_pixels_order != NULL);

  mp_pixels_order->SetImageSize(i_image_begin, i_image_end);

  if (mp_pixels_order->GetNextPixel(m_current_pixel))
    m_pixel_sample_index=0;
  else
    m_pixel_sample_index=m_samples_per_pixel;
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
  ASSERT(op_sample);
  ASSERT(m_sequences_1D_size.size() == op_sample->GetNumberOfSamplesSequences1D() && "Sample has wrong number of 1D samples sequences.");
  ASSERT(m_sequences_2D_size.size() == op_sample->GetNumberOfSamplesSequences2D() && "Sample has wrong number of 2D samples sequences.");

  if (m_pixel_sample_index==m_samples_per_pixel)
    {
    if (mp_pixels_order->GetNextPixel(m_current_pixel) == false)
      return false;

    m_pixel_sample_index=0;
    }

  if (m_pixel_sample_index==0)
    _PrecomputeSamplesForPixel(m_current_pixel);

  _GetSample(m_current_pixel, m_pixel_sample_index, op_sample);

  ++m_pixel_sample_index;
  return true;
  }

size_t Sampler::GetTotalSamplesNum() const
  {
  return mp_pixels_order->GetTotalPixelsNum()*m_samples_per_pixel;
  }

void Sampler::_PrecomputeSamplesForPixel(const Point2D_i &i_current_pixel)
  {
  // Default implementation does nothing.
  }

Sampler::~Sampler()
  {
  }