#include "Sampler.h"
#include <Raytracer/Samplers/ConsecutiveImagePixelsOrder.h>

////////////////////////////////////////////// Sampler ///////////////////////////////////////////////////

Sampler::Sampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel):
m_samples_per_pixel(i_samples_per_pixel)
  {
  ASSERT(m_samples_per_pixel>=1);

  // ConsecutiveImagePixelsOrder implementation is used by default.
  mp_pixels_order.reset(new ConsecutiveImagePixelsOrder());
  mp_pixels_order->SetImageSize(i_image_begin, i_image_end);
  }

Sampler::Sampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel, intrusive_ptr<ImagePixelsOrder> ip_pixels_order):
m_samples_per_pixel(i_samples_per_pixel), mp_pixels_order(ip_pixels_order)
  {
  ASSERT(m_samples_per_pixel>=1);
  ASSERT(mp_pixels_order != NULL);

  mp_pixels_order->SetImageSize(i_image_begin, i_image_end);
  }

size_t Sampler::AddSamplesSequence1D(size_t i_size, size_t *op_actual_size)
  {
  size_t actual_size = _RoundSamplesNumber(i_size);
  m_sequences_1D_size.push_back(actual_size);

  if (op_actual_size) *op_actual_size = actual_size;

  return m_sequences_1D_size.size()-1;
  }

size_t Sampler::AddSamplesSequence2D(size_t i_size, size_t *op_actual_size)
  {
  size_t actual_size = _RoundSamplesNumber(i_size);
  m_sequences_2D_size.push_back(actual_size);

  if (op_actual_size) *op_actual_size = actual_size;

  return m_sequences_2D_size.size()-1;
  }

void Sampler::ClearSamplesSequences()
  {
  m_sequences_1D_size.clear();
  m_sequences_2D_size.clear();
  }

intrusive_ptr<Sample> Sampler::CreateSample() const
  {
  return intrusive_ptr<Sample>( new Sample(m_sequences_1D_size, m_sequences_2D_size) );
  }

intrusive_ptr<SubSampler> Sampler::GetNextSubSampler(size_t i_pixels_num, RandomGenerator<double> *ip_rng)
  {
  ASSERT(ip_rng);
  ASSERT(i_pixels_num > 0);
  std::vector<Point2D_i> pixels;
  pixels.reserve(i_pixels_num);

  Point2D_i pixel;
  while(pixels.size()+1 <= i_pixels_num && mp_pixels_order->GetNextPixel(pixel))
    pixels.push_back(pixel);

  if (pixels.empty()==false)
    return _CreateSubSampler(pixels, m_samples_per_pixel, ip_rng);
  else
    return intrusive_ptr<SubSampler>(NULL);
  }

void Sampler::Reset()
  {
  mp_pixels_order->Reset();
  }

size_t Sampler::GetTotalSamplesNum() const
  {
  return mp_pixels_order->GetTotalPixelsNum()*m_samples_per_pixel;
  }

std::vector<size_t> Sampler::_GetSequences1DSize() const
  {
  return m_sequences_1D_size;
  }

std::vector<size_t> Sampler::_GetSequences2DSize() const
  {
  return m_sequences_2D_size;
  }

Sampler::~Sampler()
  {
  }

///////////////////////////////////////////// SubSampler //////////////////////////////////////////////////

SubSampler::SubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng):
m_pixels(i_pixels), m_samples_per_pixel(i_samples_per_pixel), mp_rng(ip_rng), m_current_pixel_index(0)
  {
  ASSERT(ip_rng);
  ASSERT(i_samples_per_pixel>0);

  if (i_pixels.empty()==false)
    m_pixel_sample_index=0;
  else
    m_pixel_sample_index=m_samples_per_pixel;
  }

bool SubSampler::GetNextSample(intrusive_ptr<Sample> op_sample)
  {
  ASSERT(op_sample);

  if (m_pixel_sample_index==m_samples_per_pixel)
    {
    if (m_current_pixel_index+1 >= m_pixels.size())
      return false;

    ++m_current_pixel_index;
    m_pixel_sample_index=0;
    }

  if (m_pixel_sample_index==0)
    _PrecomputePixelSamples(m_pixels[m_current_pixel_index]);

  _GetSample(m_pixels[m_current_pixel_index], m_pixel_sample_index, op_sample);

  ++m_pixel_sample_index;
  return true;
  }

void SubSampler::Reset()
  {
  m_current_pixel_index=0;

  if (m_pixels.empty()==false)
    m_pixel_sample_index=0;
  else
    m_pixel_sample_index=m_samples_per_pixel;
  }

size_t SubSampler::GetTotalSamplesNum() const
  {
  return m_pixels.size()*m_samples_per_pixel;
  }

void SubSampler::_PrecomputePixelSamples(const Point2D_i &i_current_pixel)
  {
  // Default implementation does nothing.
  }

RandomGenerator<double> *SubSampler::_GetRandomGenerator() const
  {
  return mp_rng;
  }

SubSampler::~SubSampler()
  {
  }