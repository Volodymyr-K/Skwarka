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

#include "LDSampler.h"
#include <Math/MathRoutines.h>
#include <Math/SamplingRoutines.h>
#include <Math/ThreadSafeRandom.h>

//////////////////////////////////////////////// LDSampler /////////////////////////////////////////////////////

LDSampler::LDSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel):
Sampler(i_image_begin, i_image_end, MathRoutines::RoundUpPow2((unsigned int)i_samples_per_pixel)), m_samples_per_pixel(MathRoutines::RoundUpPow2((unsigned int)i_samples_per_pixel))
  {
  }

LDSampler::LDSampler(const Point2D_i &i_image_begin, const Point2D_i &i_image_end, size_t i_samples_per_pixel, intrusive_ptr<ImagePixelsOrder> ip_pixels_order):
Sampler(i_image_begin, i_image_end, MathRoutines::RoundUpPow2((unsigned int)i_samples_per_pixel), ip_pixels_order), m_samples_per_pixel(MathRoutines::RoundUpPow2((unsigned int)i_samples_per_pixel))
  {
  }

size_t LDSampler::_RoundSamplesNumber(size_t i_samples_number) const
  {
  return MathRoutines::RoundUpPow2((unsigned int)i_samples_number);
  }

intrusive_ptr<SubSampler> LDSampler::_CreateSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, RandomGenerator<double> *ip_rng) const
  {
  return intrusive_ptr<SubSampler>( new LDSubSampler(i_pixels, m_samples_per_pixel, _GetSequences1DSize(), _GetSequences2DSize(), ip_rng) );
  }

/////////////////////////////////////////////// LDSubSampler ////////////////////////////////////////////////////

LDSubSampler::LDSubSampler(const std::vector<Point2D_i> &i_pixels, size_t i_samples_per_pixel, 
                           const std::vector<size_t> &i_sequences_1D_size, const std::vector<size_t> &i_sequences_2D_size, RandomGenerator<double> *ip_rng):
SubSampler(i_pixels, i_samples_per_pixel, ip_rng), m_samples_per_pixel(i_samples_per_pixel),
m_sequences_1D_size(i_sequences_1D_size), m_sequences_2D_size(i_sequences_2D_size),
m_image_points(i_samples_per_pixel), m_lens_UVs(i_samples_per_pixel)
  {
  ASSERT(MathRoutines::IsPowerOf2((unsigned int)i_samples_per_pixel));
  m_inv_samples_per_pixel_sqrt = 1.0 / sqrt((double)i_samples_per_pixel);

  size_t count_1D=0, count_2D=0;
  for(size_t i=0;i<i_sequences_1D_size.size();++i) count_1D += i_sequences_1D_size[i];
  for(size_t i=0;i<i_sequences_2D_size.size();++i) count_2D += i_sequences_2D_size[i];

  m_buffer_1D.assign(count_1D*i_samples_per_pixel, 0.0);
  m_buffer_2D.assign(count_2D*i_samples_per_pixel, Point2D_d());
  }

void LDSubSampler::_GetSample(const Point2D_i &i_current_pixel, size_t i_pixel_sample_index, intrusive_ptr<Sample> op_sample)
  {
  ASSERT(i_pixel_sample_index<m_samples_per_pixel);

  RandomGenerator<double> *p_rng = _GetRandomGenerator();
  ASSERT(p_rng);

  op_sample->SetImagePoint( m_image_points[i_pixel_sample_index] );
  op_sample->SetImageFilterWidth(m_inv_samples_per_pixel_sqrt, m_inv_samples_per_pixel_sqrt);
  op_sample->SetLensUV( m_lens_UVs[i_pixel_sample_index] );

  // Disable compiler warning since we use std::copy with unchecked iterators.
#pragma warning( push )
#pragma warning( disable : 4996 )

  size_t count_1D = 0;
  ASSERT(op_sample->GetNumberOfSamplesSequences1D() == m_sequences_1D_size.size());
  for(size_t i=0;i<m_sequences_1D_size.size();++i)
    {
    SamplesSequence1D sequence = op_sample->GetSamplesSequence1D(i);
    ASSERT(std::distance(sequence.m_begin, sequence.m_end) == m_sequences_1D_size[i]);

    std::vector<double>::iterator source_begin = m_buffer_1D.begin()+count_1D*m_samples_per_pixel+m_sequences_1D_size[i]*i_pixel_sample_index;
    std::copy(source_begin, source_begin+m_sequences_1D_size[i], sequence.m_begin);

    count_1D += m_sequences_1D_size[i];
    }

  size_t count_2D = 0;
  ASSERT(op_sample->GetNumberOfSamplesSequences2D() == m_sequences_2D_size.size());
  for(size_t i=0;i<m_sequences_2D_size.size();++i)
    {
    SamplesSequence2D sequence = op_sample->GetSamplesSequence2D(i);
    ASSERT(std::distance(sequence.m_begin, sequence.m_end) == m_sequences_2D_size[i]);

    std::vector<Point2D_d>::iterator source_begin = m_buffer_2D.begin()+count_2D*m_samples_per_pixel+m_sequences_2D_size[i]*i_pixel_sample_index;
    std::copy(source_begin, source_begin+m_sequences_2D_size[i], sequence.m_begin);

    count_2D += m_sequences_2D_size[i];
    }

#pragma warning( pop )
  }

void LDSubSampler::_PrecomputePixelSamples(const Point2D_i &i_current_pixel)
  {
  RandomGenerator<double> *p_rng = _GetRandomGenerator();

  _LDShuffleScrambled2D(m_image_points.begin(), 1);

  // Translate the image samples.
  for (size_t i = 0; i < m_samples_per_pixel; ++i)
    m_image_points[i] += Convert<double>(i_current_pixel);

  _LDShuffleScrambled2D(m_lens_UVs.begin(), 1);

  size_t count_1D = 0;
  for (size_t i=0;i<m_sequences_1D_size.size();++i)
    {
    _LDShuffleScrambled1D(m_buffer_1D.begin()+count_1D*m_samples_per_pixel, m_sequences_1D_size[i]);
    count_1D += m_sequences_1D_size[i];
    }

  size_t count_2D = 0;
  for (size_t i=0;i<m_sequences_2D_size.size();++i)
    {
    _LDShuffleScrambled2D(m_buffer_2D.begin()+count_2D*m_samples_per_pixel, m_sequences_2D_size[i]);
    count_2D += m_sequences_2D_size[i];
    }
  }

void LDSubSampler::_LDShuffleScrambled1D(std::vector<double>::iterator i_begin, size_t i_samples_num) const
  {
  RandomGenerator<double> *p_rng = _GetRandomGenerator();
  ASSERT(p_rng);

  /**
  * The range will be filled with 1D samples for all pixel samples.
  * The total number of samples filled is equal to m_samples_per_pixel*i_samples_num.
  * The generated samples are grouped into m_samples_per_pixel groups with i_samples_num values each.
  * The groups are randomly shuffled with each other and the values in each group are also randomly shuffled with each other.
  * This is needed to reduce the visual artifcats due to samples correlation between different pixels (this is how pbrt explains that).
  */

  // Shuffle groups.
  std::vector<size_t> shuffled_order(m_samples_per_pixel);
  for (size_t i = 0; i < m_samples_per_pixel; ++i) shuffled_order[i]=i;
  SamplingRoutines::Shuffle(shuffled_order.begin(), m_samples_per_pixel, p_rng);

  unsigned int count=0;
  unsigned int scramble = RandomUInt();
  for (size_t i = 0; i < m_samples_per_pixel; ++i)
    {
    std::vector<double>::iterator it = i_begin + shuffled_order[i]*i_samples_num;
    for (size_t j = 0; j < i_samples_num; ++j)
      *(it+j) = SamplingRoutines::VanDerCorput(count++, scramble);
    }

  // Shuffle samples in each group.
  if (i_samples_num>1)
    for (size_t i = 0; i < m_samples_per_pixel; ++i)
      SamplingRoutines::Shuffle(i_begin + i * i_samples_num, i_samples_num, p_rng);
  }

void LDSubSampler::_LDShuffleScrambled2D(std::vector<Point2D_d>::iterator i_begin, size_t i_samples_num) const
  {
  RandomGenerator<double> *p_rng = _GetRandomGenerator();
  ASSERT(p_rng);

  /**
  * The range will be filled with 2D samples for all pixel samples.
  * The total number of samples filled is equal to m_samples_per_pixel*i_samples_num.
  * The generated samples are grouped into m_samples_per_pixel groups with i_samples_num values each.
  * The groups are randomly shuffled with each other and the values in each group are also randomly shuffled with each other.
  * This is needed to reduce the visual artifcats due to samples correlation between different pixels (this is how pbrt explains that).
  */

  // Shuffle groups.
  std::vector<size_t> shuffled_order(m_samples_per_pixel);
  for (size_t i = 0; i < m_samples_per_pixel; ++i) shuffled_order[i]=i;
  SamplingRoutines::Shuffle(shuffled_order.begin(), m_samples_per_pixel, p_rng);

  unsigned int count=0;
  unsigned int scramble1 = RandomUInt(), scramble2 = RandomUInt();
  for (size_t i = 0; i < m_samples_per_pixel; ++i)
    {
    std::vector<Point2D_d>::iterator it = i_begin + shuffled_order[i]*i_samples_num;
    for (size_t j = 0; j < i_samples_num; ++j)
      {
      *(it+j) = Point2D_d( SamplingRoutines::VanDerCorput(count, scramble1), SamplingRoutines::Sobol2(count, scramble2) );
      ++count;
      }
    }

  // Shuffle samples in each group.
  if (i_samples_num>1)
    for (size_t i = 0; i < m_samples_per_pixel; ++i)
      SamplingRoutines::Shuffle(i_begin + i * i_samples_num, i_samples_num, p_rng);
  }