#ifndef SAMPLER_H
#define SAMPLER_H

#include <Common/Common.h>
#include <Math/Point2D.h>
#include <Math/RandomGenerator.h>
#include "Sample.h"
#include <vector>

/**
* This is a pure abstract class defining the strategy contract for the order the image pixels are sampled in.
* This strategy is used by Sampler class to iterate through all pixels of an image.
* Different strategies are preferable when an image is displayed in a real-time and gets refined as more samples are added to the film.
* @sa Sampler
*/
class ImagePixelsOrder: public ReferenceCounted
  {
  public:
    /**
    * Sets image size.
    * @param i_image_begin Left lower corner of the sampling image.
    * @param i_image_end Right upper corner of the sampling image (exclusive).
    */
    virtual void SetImageSize(const Point2D_i &i_image_begin, const Point2D_i &i_image_end) = 0;

    /**
    * Returns total number of image pixels.
    */
    virtual size_t GetTotalPixelsNum() const = 0;

    /**
    * Resets the pixel order.
    * The next call to GetNextPixel() will get the first image pixel in the sense of the order defined by the strategy.
    */
    virtual void Reset() = 0;

    /**
    * Gets the next image pixel.
    * param[out] o_image_point Next image point.
    * return true if the next image pixel was successfully get and false if there's no more pixels.
    */
    virtual bool GetNextPixel(Point2D_i &o_image_pixel) = 0;

    virtual ~ImagePixelsOrder() {}

  protected:
    ImagePixelsOrder() {}

  private:
    // Not implemented, not a value type.
    ImagePixelsOrder(const ImagePixelsOrder&);
    ImagePixelsOrder &operator=(const ImagePixelsOrder&);
  };

class SubSampler;

/**
* The class is responsible for generating sub-samplers which eventually generate samples.
* This is a base abstract class. Different implementations create different sub-samplers that use different sampling strategies and algorithms for generating samples.
* The idea behind the sub-samplers is that multiple sub-samplers can be created and used concurrently without locking.
* The class uses a pluggable ImagePixelsOrder strategy for the order the pixels are sampled in. By default, the pixels are sampled in a consecutive order.
* @sa Sample, SubSampler, ImagePixelsOrder
*/
class Sampler: public ReferenceCounted
  {
  public:
    /**
    * Integrators should call this method to request a 1D samples sequence.
    * The method should only be called before CreateSample() method is called.
    * @warning The actual size of samples sequence may be larger than what was actually requested. This depends on the Sampler implementation.
    * @param i_size Size of the requested samples sequence.
    * @param[out] op_actual_size Pointer to where the actual samples sequence size will be written to. Can be NULL.
    * @return An identifier the integrators should use to retrieve the samples sequence from the Sample.
    */
    size_t AddSamplesSequence1D(size_t i_size, size_t *op_actual_size=NULL);

    /**
    * Integrators should call this method to request a 2D samples sequence.
    * The method should only be called before CreateSample() method is called.
    * @warning The actual size of samples sequence may be larger than what was actually requested. This depends on the Sampler implementation.
    * @param i_size Size of the requested samples sequence.
    * @param[out] op_actual_size Pointer to where the actual samples sequence size will be written to. Can be NULL.
    * @return An identifier the integrators should use to retrieve the samples sequence from the Sample.
    */
    size_t AddSamplesSequence2D(size_t i_size, size_t *op_actual_size=NULL);

    /**
    * Clears all 1D and 2D samples sequences added so far.
    */
    void ClearSamplesSequences();

    /**
    * Creates empty Sample instance with no real data but with all the required storages allocated.
    * The sample should be passed to sub-sampler's GetNextSample() method to populate it with the data.
    */
    intrusive_ptr<Sample> CreateSample() const;

    /**
    * Creates a sub-sampler for the specified number of pixels read from the ImagePixelsOrder implementation.
    * @param i_pixels_num Number of pixels to be read from the ImagePixelsOrder implementation. Should be greater than zero.
    * @param ip_rng Random number generator to be used by the sub-sampler for generating samples. Should not be NULL.
    * @return Created sub-sampler. Can be NULL if there are no more pixels in the ImagePixelsOrder implementation.
    */
    intrusive_ptr<SubSampler> GetNextSubSampler(size_t i_pixels_num, RandomGenerator<double> *ip_rng);

    /**
    * Resets the sampler.
    * The method resets the sampler's internal pixel cursor making the sampler generate the sub-samplers from the beginning.
    */
    void Reset();

    /**
    * Returns total number of samples the Sampler produces.
    */
    size_t GetTotalSamplesNum() const;

    virtual ~Sampler();

  protected:
    /**
    * Creates Sampler instance.
    * ConsecutiveImagePixelsOrder implementation is used to define the order the image pixels are sampled in.
    * @param i_image_begin Left lower corner of the sampling image.
    * @param i_image_end Right upper corner of the sampling image (exclusive).
    * @param i_samples_per_pixel Number of pixel samples per pixel. Should be greater than zero.
    */
    Sampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel);

    /**
    * Creates Sampler instance.
    * @param i_image_begin Left lower corner of the sampling image.
    * @param i_image_end Right upper corner of the sampling image (exclusive).
    * @param i_samples_per_pixel Number of pixel samples per pixel. Should be greater than zero.
    * @param ip_pixels_order ImagePixelsOrder implementation defining the order the image pixels are sampled in. Should not be NULL.
    */
    Sampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel, intrusive_ptr<ImagePixelsOrder> ip_pixels_order);

    /**
    * Returns the nearest number of integrator samples higher or equal than the specified one that the sampler can produce.
    */
    virtual size_t _RoundSamplesNumber(size_t i_samples_number) const = 0;

    /**
    * Creates sub-sampler for the specified image pixels.
    */
    virtual intrusive_ptr<SubSampler> _CreateSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng) const = 0;

    /**
    * The method can be called by derived classes to get the sizes of the requested 1D samples sequences.
    */
    std::vector<size_t> _GetSequences1DSize() const;

    /**
    * The method can be called by derived classes to get the sizes of the requested 1D samples sequences.
    */
    std::vector<size_t> _GetSequences2DSize() const;

  private:
    // Not implemented, not a value type.
    Sampler();
    Sampler(const Sampler&);
    Sampler &operator=(const Sampler&);

  private:
    intrusive_ptr<ImagePixelsOrder> mp_pixels_order;
    size_t m_samples_per_pixel;

    std::vector<size_t> m_sequences_1D_size, m_sequences_2D_size;
  };

/**
* The class is responsible for samples. Sub-samplers generates samples for the specified sub-region of the image plane. Sub-samplers are generated by Sampler class.
* This is a base abstract class. Different implementations use different sampling strategies and algorithms for generating samples.
* @sa Sample, Sampler
*/
class SubSampler: public ReferenceCounted
  {
  public:
    /**
    * Populates the specified Sample with the samples data.
    * param[out] op_sample Sample instance to be populated with the data.
    * return true if sample was successfully populated and false if there's no more samples.
    */
    bool GetNextSample(intrusive_ptr<Sample> op_sample);

    /**
    * Resets the sampler.
    * The method resets the sub-sampler's internal pixel cursor making the sampler generate the samples from the beginning.
    */
    void Reset();

    /**
    * Returns total number of samples the sub-sampler produces.
    */
    size_t GetTotalSamplesNum() const;

    virtual ~SubSampler();

  protected:
    /**
    * Creates SubSampler instance for the specified pixels.
    * @param i_pixels Pixels the sub-sampler should create samples for. Should not be empty.
    * @param i_samples_per_pixel Number of pixel samples per pixel. Should be greater than zero.
    * @param ip_rng Random number generator to be used by the sub-sampler for generating samples. Should not be NULL.
    */
    SubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng);

    /**
    * Populates the Sample with the samples data for the specified image pixel and specified sample's index inside that pixel.
    */
    virtual void _GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, intrusive_ptr<Sample> op_sample) = 0;

    /**
    * The SubSampler calls this method for each new pixel before calling _GetSample() method for that pixel.
    * Implementations may precompute the samples values for the entire pixel here.
    */
    virtual void _PrecomputePixelSamples(const Point2D_i &i_current_pixel);

    RandomGenerator<double> *_GetRandomGenerator() const;

  private:
    // Not implemented, not a value type.
    SubSampler();
    SubSampler(const SubSampler&);
    SubSampler &operator=(const SubSampler&);

  private:
    std::vector<Point2D_i> m_pixels;
    size_t m_current_pixel_index;
    size_t m_samples_per_pixel, m_pixel_sample_index;

    RandomGenerator<double> *mp_rng;
  };

#endif // SAMPLER_H