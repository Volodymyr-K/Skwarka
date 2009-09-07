#ifndef RANDOM_SAMPLER_H
#define RANDOM_SAMPLER_H

#include <Common/Common.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Core/Sampler.h>
#include <Math/Geometry.h>
#include <Math/ThreadSafeRandom.h>

/**
* Sampler implementation that produces completely random values for all sample values.
* All values are generated independently with no stratification etc.
*/
class RandomSampler: public Sampler
  {
  public:
    /**
    * Creates RandomSampler instance.
    * @param i_image_begin Left lower corner of the sampling window.
    * @param i_image_end Right upper corner of the sampling window (exclusive).
    * @param i_samples_per_pixel Number of pixel samples per pixel.
    */
    RandomSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel);

  protected:
    /**
    * Returns the nearest number of integrator samples higher or equal than the specified one that the sampler can produce.
    * This implementation can produce any given number of samples so it just returns what is passed.
    */
    size_t _RoundSamplesNumber(size_t i_samples_number) const;

    /**
    * Populates the Sample with the samples data for the specified image pixel and specified sample's index inside that pixel.
    */
    void _GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, shared_ptr<Sample> op_sample);
  };

#endif // RANDOM_SAMPLER_H