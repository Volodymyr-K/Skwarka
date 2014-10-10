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

#ifndef SAMPLER_MOCK_H
#define SAMPLER_MOCK_H

#include <Common/Common.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Core/Sampler.h>
#include <Math/Geometry.h>
#include <Math/ThreadSafeRandom.h>

class SubSamplerMock;

/*
Sampler mock implementation.
Always generates random samples.
*/
class SamplerMock: public Sampler
  {
  public:
    SamplerMock(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel);

  protected:
    size_t _RoundSamplesNumber(size_t i_samples_number) const;

    virtual intrusive_ptr<SubSampler> _CreateSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng) const;
  };

class SubSamplerMock: public SubSampler
  {
  public:
    SubSamplerMock(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng);

  protected:
    void _GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, intrusive_ptr<Sample> op_sample);

  private:
    double m_inv_samples_per_pixel;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

SamplerMock::SamplerMock(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel):
  Sampler(i_image_begin, i_image_end, i_samples_per_pixel)
  {
  }

size_t SamplerMock::_RoundSamplesNumber(size_t i_samples_number) const
  {
  return i_samples_number;
  }

intrusive_ptr<SubSampler> SamplerMock::_CreateSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng) const
  {
  return intrusive_ptr<SubSampler>( new SubSamplerMock(i_pixels, i_samples_per_pixel, ip_rng) );
  }

SubSamplerMock::SubSamplerMock(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng):
SubSampler(i_pixels, i_samples_per_pixel,ip_rng)
  {
  ASSERT(i_samples_per_pixel > 0);
  m_inv_samples_per_pixel = 1.0 / i_samples_per_pixel;
  }

void SubSamplerMock::_GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, intrusive_ptr<Sample> op_sample)
  {
  op_sample->SetImagePoint( Convert<double>(i_current_pixel) + Point2D_d(RandomDouble(1.0), RandomDouble(1.0)) );
  op_sample->SetImageFilterWidth(m_inv_samples_per_pixel, m_inv_samples_per_pixel);
  op_sample->SetLensUV( Point2D_d(RandomDouble(1.0), RandomDouble(1.0)) );

  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences1D();++i)
    {
    SamplesSequence1D sequence = op_sample->GetSamplesSequence1D(i);
    for(SamplesSequence1D::Iterator it = sequence.m_begin; it != sequence.m_end; ++it)
      *it=RandomDouble(1.0);
    }

  for(size_t i=0;i<op_sample->GetNumberOfSamplesSequences2D();++i)
    {
    SamplesSequence2D sequence = op_sample->GetSamplesSequence2D(i);
    for(SamplesSequence2D::Iterator it = sequence.m_begin; it != sequence.m_end; ++it)
      *it=Point2D_d(RandomDouble(1.0), RandomDouble(1.0));
    }
  }

#endif // SAMPLER_MOCK_H