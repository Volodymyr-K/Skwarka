#ifndef LD_SAMPLER_H
#define LD_SAMPLER_H

#include <Common/Common.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Core/Sampler.h>
#include <Math/Point2D.h>
#include <vector>

/**
* Sampler implementation that creates LDSubSampler instances that produce low-discrepancy samples.
* For image samples, lens samples and 2D integrator samples sequences the sampler generates (0,2)-sequences using VanDerCorput and Sobol' sequences.
* For 1D integrator samples sequences the sampler generates low-discrepancy samples using VanDerCorput sequence.
* The special property of (0,2)-sequences is that any sequence of 2^n samples is well distributed (e.g. each 8 samples are well distirbuted, each 16 samples are well distirbuted etc.)
* The sampler can produce only a number of image samples, lens samples and integrator samples that is a power of 2 (otherwise they won't be well distributed).
* The sampler uses random scrambling technique to generate different samples for different pixels, without it all pixels would have the same sample values.
*
* The class uses a pluggable ImagePixelsOrder strategy for the order the pixels are sampled in. By default, the pixels are sampled in a consecutive order.
* @sa LDSubSampler
*/
class LDSampler: public Sampler
  {
  public:
    /**
    * Creates LDSampler instance.
    * ConsecutiveImagePixelsOrder implementation is used to define the order the image pixels are sampled in.
    * @param i_image_begin Left lower corner of the sampling image.
    * @param i_image_end Right upper corner of the sampling image (exclusive).
    * @param i_samples_per_pixel Number of image samples per pixel. Should be a power of 2, otherwise the value is rounded up to the nearest power of 2.
    */
    LDSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel);

    /**
    * Creates ConsecutiveImagePixelsOrder instance.
    * @param i_image_begin Left lower corner of the sampling image.
    * @param i_image_end Right upper corner of the sampling image (exclusive).
    * @param i_samples_per_pixel Number of image samples per pixel. Should be a power of 2, otherwise the value is rounded up to the nearest power of 2.
    * @param ip_pixels_order ImagePixelsOrder implementation defining the order the image pixels are sampled in. Should not be NULL.
    */
    LDSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel, intrusive_ptr<ImagePixelsOrder> ip_pixels_order);

  protected:
    /**
    * Returns the nearest number of integrator samples higher or equal than the specified one that the sampler can produce.
    * The method returns the next power of 2.
    */
    size_t _RoundSamplesNumber(size_t i_samples_number) const;

    /**
    * Creates LDSubSampler for the specified image pixels.
    */
    virtual intrusive_ptr<SubSampler> _CreateSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng) const;

  private:
    size_t m_samples_per_pixel;
  };

/**
* SubSampler implementation that produces low-discrepancy samples.
* For image samples, lens samples and 2D sample sequences the sampler generates (0,2)-sequences using VanDerCorput and Sobol' sequences.
* For 1D sample sequences the sampler generates low-discrepancy samples using VanDerCorput sequence.
* The special property of (0,2)-sequences is that any sequence of 2^n samples is well distributed (e.g. each 8 samples are well distirbuted, each 16 samples are well distirbuted etc.)
* The sampler can produce only a number of image samples, lens samples or integrator samples that is a power of 2.
* The sampler uses random scrambling technique to generate different samples for different pixels, without it all pixels would have the same samples.
*
* @sa LDSampler
*/
class LDSubSampler: public SubSampler
  {
  // Only corresponding Sampler implementation can create the sub-sampler.
  friend LDSampler;

  protected:
    /**
    * Populates the Sample with the samples data for the specified image pixel and specified sample's index inside that pixel.
    */
    virtual void _GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, intrusive_ptr<Sample> op_sample);

    /**
    * Precomputes image samples, lens samples and integrator samples sequences for the specified pixel.
    */
    void _PrecomputePixelSamples(const Point2D_i &i_current_pixel);

  private:
    /**
    * Creates LDSubSampler instance for the specified pixels.
    * @param i_pixels Pixels the sub-sampler should create samples for. Should not be empty.
    * @param i_samples_per_pixel Number of image samples per pixel. Should be a power of 2.
    * @param i_sequences_1D_size Vector of sizes of requested 1D samples sequences.
    * @param i_sequences_2D_size Vector of sizes of requested 2D samples sequences.
    * @param ip_rng Random number generator to be used by the sub-sampler for generating samples. Should not be NULL.
    */
    LDSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel,
      const std::vector<size_t> &i_sequences_1D_size, const std::vector<size_t> &i_sequences_2D_size, RandomGenerator<double> *ip_rng);

    /**
    * The private helper method that fills the specified range with 1D samples using VanDerCorput sequence.
    */
    void _LDShuffleScrambled1D(std::vector<double>::iterator i_begin, size_t i_samples_num) const;

    /**
    * The private helper method that fills the specified range with 2D samples using (0,2)-sequences (VanDerCorput and Sobol' sequences).
    */
    void _LDShuffleScrambled2D(std::vector<Point2D_d>::iterator i_begin, size_t i_samples_num) const;

  private:
    size_t m_samples_per_pixel;
    double m_inv_samples_per_pixel_sqrt;

    std::vector<size_t> m_sequences_1D_size, m_sequences_2D_size;

    std::vector<Point2D_d> m_image_points, m_lens_UVs;
    std::vector<double> m_buffer_1D;
    std::vector<Point2D_d> m_buffer_2D;
  };


#endif // LD_SAMPLER_H