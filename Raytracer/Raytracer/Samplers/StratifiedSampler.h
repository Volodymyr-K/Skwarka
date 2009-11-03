#ifndef STRATIFIED_SAMPLER_H
#define STRATIFIED_SAMPLER_H

#include <Common/Common.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Core/Sampler.h>
#include <Math/Point2D.h>
#include <vector>

/**
* Sampler implementation that creates StratifiedSubSampler instances that produces stratified samples.
* Image and lens samples are stratified with respect to other samples inside the same pixel.
* Integrator 2D samples are produced by the LatinHypercube algorithm.
* Integrator samples are stratified within each pixel.
* Integrator samples are <b>not</b> stratified with respect to other pixels.
*
* The class uses a pluggable ImagePixelsOrder strategy for the order the pixels are sampled in. By default, the pixels are sampled in a consecutive order.
* @sa StratifiedSubSampler
*/
class StratifiedSampler: public Sampler
  {
  public:
    /**
    * Creates StratifiedSampler instance.
    * ConsecutiveImagePixelsOrder implementation is used to define the order the image pixels are sampled in.
    * @param i_image_begin Left lower corner of the sampling image.
    * @param i_image_end Right upper corner of the sampling image (exclusive).
    * @param i_x_samples_per_pixel Number of pixel samples in X direction.
    * @param i_y_samples_per_pixel Number of pixel samples in Y direction.
    */
    StratifiedSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_x_samples_per_pixel, size_t i_y_samples_per_pixel);

    /**
    * Creates StratifiedSampler instance.
    * @param i_image_begin Left lower corner of the sampling image.
    * @param i_image_end Right upper corner of the sampling image (exclusive).
    * @param i_x_samples_per_pixel Number of pixel samples in X direction.
    * @param i_y_samples_per_pixel Number of pixel samples in Y direction.
    * @param ip_pixels_order ImagePixelsOrder implementation defining the order the image pixels are sampled in. Should not be NULL.
    */
    StratifiedSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_x_samples_per_pixel,
      size_t i_y_samples_per_pixel, intrusive_ptr<ImagePixelsOrder> ip_pixels_order);

  protected:
    /**
    * Returns the nearest number of integrator samples higher or equal than the specified one that the sampler can produce.
    * Since LatinHypercube can produce any given number of samples, this method just returns what is passed.
    */
    size_t _RoundSamplesNumber(size_t i_samples_number) const;

    /**
    * Creates StratifiedSubSampler for the specified image pixels.
    */
    virtual intrusive_ptr<SubSampler> _CreateSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng) const;

  private:
    size_t m_x_samples_per_pixel, m_y_samples_per_pixel;
  };

/**
* SubSampler implementation that produces stratified samples.
* Image and lens samples are stratified with respect to other samples inside the same pixel.
* Integrator 2D samples are produced by the LatinHypercube algorithm.
* Integrator samples are stratified within each pixel.
* Integrator samples are <b>not</b> stratified with respect to other pixels.
* @sa StratifiedSampler
*/
class StratifiedSubSampler: public SubSampler
  {
  public:
    /**
    * Creates StratifiedSubSampler instance for the specified pixels.
    * @param i_pixels Pixels the sub-sampler should create samples for. Should not be empty.
    * @param i_x_samples_per_pixel Number of pixel samples in X direction.
    * @param i_y_samples_per_pixel Number of pixel samples in Y direction.
    * @param ip_rng Random number generator to be used by the sub-sampler for generating samples. Should not be NULL.
    */
    StratifiedSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_x_samples_per_pixel, size_t i_y_samples_per_pixel, RandomGenerator<double> *ip_rng);

  protected:
    /**
    * Populates the Sample with the samples data for the specified image pixel and specified sample's index inside that pixel.
    */
    virtual void _GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, intrusive_ptr<Sample> op_sample);

    /**
    * Precomputes image and lens samples for the specified pixel.
    */
    void _PrecomputePixelSamples(const Point2D_i &i_current_pixel);

  private:
    size_t m_x_samples_per_pixel, m_y_samples_per_pixel;

    std::vector<Point2D_d> m_image_points;
    std::vector<Point2D_d> m_lens_UVs;
  };


#endif // RANDOM_SAMPLER_H