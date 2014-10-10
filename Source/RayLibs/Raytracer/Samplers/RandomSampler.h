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

#ifndef RANDOM_SAMPLER_H
#define RANDOM_SAMPLER_H

#include <Common/Common.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Core/Sampler.h>
#include <Math/Point2D.h>

/**
* Sampler implementation that creates RandomSubSampler instances that produce completely random values for all sample values.
* All values are generated independently with no stratification etc.
*
* The class uses a pluggable ImagePixelsOrder strategy for the order the pixels are sampled in. By default, the pixels are sampled in a consecutive order.
* @sa RandomSubSampler
*/
class RandomSampler: public Sampler
  {
  public:
    /**
    * Creates RandomSampler instance.
    * ConsecutiveImagePixelsOrder implementation is used to define the order the image pixels are sampled in.
    * @param i_image_begin Left lower corner of the sampling image.
    * @param i_image_end Right upper corner of the sampling image (exclusive).
    * @param i_samples_per_pixel Number of image samples per pixel.
    */
    RandomSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel);

    /**
    * Creates RandomSampler instance.
    * @param i_image_begin Left lower corner of the sampling image.
    * @param i_image_end Right upper corner of the sampling image (exclusive).
    * @param i_samples_per_pixel Number of image samples per pixel.
    * @param ip_pixels_order ImagePixelsOrder implementation defining the order the image pixels are sampled in. Should not be NULL.
    */
    RandomSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel, intrusive_ptr<ImagePixelsOrder> ip_pixels_order);

  protected:
    /**
    * Returns the nearest number of integrator samples higher or equal than the specified one that the sampler can produce.
    * This implementation can produce any given number of samples so it just returns what is passed.
    */
    size_t _RoundSamplesNumber(size_t i_samples_number) const;

    /**
    * Creates RandomSubSampler for the specified image pixels.
    */
    virtual intrusive_ptr<SubSampler> _CreateSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng) const;
  };

/**
* SubSampler implementation that produces completely random values for all sample values.
* All values are generated independently with no stratification etc.
* @sa RandomSampler
*/
class RandomSubSampler: public SubSampler
  {
  // Only corresponding Sampler implementation can create the sub-sampler.
  friend RandomSampler;

  protected:
    /**
    * Populates the Sample with the samples data for the specified image pixel and specified sample's index inside that pixel.
    */
    virtual void _GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, intrusive_ptr<Sample> op_sample);

  private:
    /**
    * Creates RandomSubSampler instance for the specified pixels.
    * @param i_pixels Pixels the sub-sampler should create samples for. Should not be empty.
    * @param i_samples_per_pixel Number of image samples per pixel. Should be greater than zero.
    * @param ip_rng Random number generator to be used by the sub-sampler for generating samples. Should not be NULL.
    */
    RandomSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng);

  private:
    double m_inv_samples_per_pixel_sqrt;
  };

#endif // RANDOM_SAMPLER_H