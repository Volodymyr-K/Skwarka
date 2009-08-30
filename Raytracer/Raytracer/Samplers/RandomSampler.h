#ifndef RANDOM_SAMPLER_H
#define RANDOM_SAMPLER_H

#include <Common\Common.h>
#include <Raytracer\Core\Sample.h>
#include <Raytracer\Core\Sampler.h>
#include <Math\Geometry.h>
#include <Math\MultiThreadedRandom.h>

class RandomSampler: public Sampler
  {
  public:
    RandomSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel);

  protected:
    size_t _RoundSamplesNumber(size_t i_samples_number) const;
    void _GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, shared_ptr<Sample> op_sample);
  };

#endif // RANDOM_SAMPLER_H