#ifndef SAMPLER_H
#define SAMPLER_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "Sample.h"

/**
* The class is responsible for generating samples.
* This is a base abstract class. Different implementation may use different sampling patterns and algorithms for generating the samples.
* @sa Sample
*/
class Sampler
  {
  public:
    /**
    * Integrators should call this method to request a 1D samples sequence.
    * The method should only be called before GetNextSample() method is called.
    * @warning The actual size of samples sequence may be bigger than the one requested. This depends on the Sampler implementation.
    * @param i_size Size of the requested samples sequence.
    * @return An identifier the integrators should use to retrieve the samples sequence from the Sample.
    */
    size_t AddSamplesSequence1D(size_t i_size);

    /**
    * Integrators should call this method to request a 2D samples sequence.
    * The method should only be called before GetNextSample() method is called.
    * @warning The actual size of samples sequence may be bigger than the one requested. This depends on the Sampler implementation.
    * @param i_size Size of the requested samples sequence.
    * @return An identifier the integrators should use to retrieve the samples sequence from the Sample.
    */
    size_t AddSamplesSequence2D(size_t i_size);

    /**
    * Creates empty Sample instance with no real data but with all the required storages allocated.
    * The sample should be passed to GetNextSample() method to populate it with the data.
    */
    shared_ptr<Sample> CreateSample() const;

    /**
    * Populates the specified Sample with the samples data.
    * param[out] op_sample Sample instance to be populated with the data.
    * return true if sample was successfully populated and false if there's no more samples.
    */
    bool GetNextSample(shared_ptr<Sample> op_sample);

    /**
    * Returns total number of samples the Sampler produces.
    */
    size_t GetTotalSamplesNum() const;

    virtual ~Sampler();

  protected:
    /**
    * Creates Sampler instance.
    * @param i_image_begin Left lower corner of the sampling window.
    * @param i_image_end Right upper corner of the sampling window (exclusive).
    * @param i_samples_per_pixel Number of pixel samples per pixel.
    */
    Sampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel);

    /**
    * Returns the nearest number of integrator samples higher or equal than the specified one that the sampler can produce.
    */
    virtual size_t _RoundSamplesNumber(size_t i_samples_number) const = 0;

    /**
    * Populates the Sample with the samples data for the specified image pixel and specified sample's index inside that pixel.
    */
    virtual void _GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, shared_ptr<Sample> op_sample) = 0;

    /**
    * The Sampler calls this method for each new pixel before calling _GetSample() method for that pixel.
    * Implementations may precompute the samples values for the entire pixel here.
    */
    virtual void _PrecomputeSamplesForPixel(const Point2D_i &i_current_pixel);

  private:
    // Not implemented, Sampler is not a value type.
    Sampler();
    Sampler(const Sampler&);
    Sampler &operator=(const Sampler&);

  private:
    Point2D_i m_image_begin, m_image_end, m_current_pixel;
    size_t m_samples_per_pixel, m_pixel_sample_index;

    std::vector<size_t> m_sequences_1D_size, m_sequences_2D_size;
  };

#endif // SAMPLER_H