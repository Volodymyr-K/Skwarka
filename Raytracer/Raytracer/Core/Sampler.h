#ifndef SAMPLER_H
#define SAMPLER_H

#include <Common\Common.h>
#include <Math\Geometry.h>
#include "Sample.h"

class Sampler
  {
  public:
    Sampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel);

    size_t AddSamplesSequence1D(size_t i_size);
    size_t AddSamplesSequence2D(size_t i_size);

    // This method just creates an empty sample instance with no real data. GetNextSample() method should be called to populate it with the data.
    shared_ptr<Sample> CreateSample() const;

    bool GetNextSample(shared_ptr<Sample> op_sample);

    size_t GetTotalSamplesNum() const;

    virtual ~Sampler();

  protected:
    virtual size_t _RoundSamplesNumber(size_t i_samples_number) const = 0;
    virtual void _GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, shared_ptr<Sample> op_sample) = 0;
    virtual void _PrecomputeSamplesForPixel(const Point2D_i &i_current_pixel);

  private:
    // not implemented
    Sampler();
    Sampler(const Sampler&);
    Sampler &operator=(const Sampler&);

  private:
    Point2D_i m_image_begin, m_image_end, m_current_pixel;
    size_t m_samples_per_pixel, m_pixel_sample_index;

    std::vector<size_t> m_sequences_1D_size, m_sequences_2D_size;
  };

#endif // SAMPLER_H