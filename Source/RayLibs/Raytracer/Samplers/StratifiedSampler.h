/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef STRATIFIED_SAMPLER_H
#define STRATIFIED_SAMPLER_H

#include <Common/Common.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Core/Sampler.h>
#include <Math/Point2D.h>
#include <vector>

/**
* Sampler implementation that creates StratifiedSubSampler instances that produce stratified samples.
* Image and lens samples are stratified with respect to other samples inside the same pixel.
* Integrator 2D samples are produced by the StratifiedSampling2D algorithm.
* The sampler can produce samples sequences with a length that is a square integer.
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
    * @param i_x_samples_per_pixel Number of image samples in X direction.
    * @param i_y_samples_per_pixel Number of image samples in Y direction.
    */
    StratifiedSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_x_samples_per_pixel, size_t i_y_samples_per_pixel);

    /**
    * Creates StratifiedSampler instance.
    * @param i_image_begin Left lower corner of the sampling image.
    * @param i_image_end Right upper corner of the sampling image (exclusive).
    * @param i_x_samples_per_pixel Number of image samples per pixel in X dimension.
    * @param i_y_samples_per_pixel Number of image samples per pixel in Y dimension.
    * @param ip_pixels_order ImagePixelsOrder implementation defining the order the image pixels are sampled in. Should not be NULL.
    */
    StratifiedSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_x_samples_per_pixel,
      size_t i_y_samples_per_pixel, intrusive_ptr<ImagePixelsOrder> ip_pixels_order);

  protected:
    /**
    * Returns the nearest number of integrator samples higher or equal than the specified one that the sampler can produce.
    * The method returns the next square integer to be able to generate the same number of samples in both dimensions for 2D samples sequences.
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
  // Only corresponding Sampler implementation can create the sub-sampler.
  friend StratifiedSampler;

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
    /**
    * Creates StratifiedSubSampler instance for the specified pixels.
    * @param i_pixels Pixels the sub-sampler should create samples for. Should not be empty.
    * @param i_x_samples_per_pixel Number of image samples per pixel in X dimension.
    * @param i_y_samples_per_pixel Number of image samples per pixel in Y dimension.
    * @param ip_rng Random number generator to be used by the sub-sampler for generating samples. Should not be NULL.
    */
    StratifiedSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_x_samples_per_pixel, size_t i_y_samples_per_pixel, RandomGenerator<double> *ip_rng);

  private:
    size_t m_x_samples_per_pixel, m_y_samples_per_pixel;
    double m_inv_x_samples_per_pixel, m_inv_y_samples_per_pixel;

    std::vector<Point2D_d> m_image_points;
    std::vector<Point2D_d> m_lens_UVs;
  };


#endif // RANDOM_SAMPLER_H