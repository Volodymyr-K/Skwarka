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

#include "ConsecutiveImagePixelsOrder.h"
#include <Common/Common.h>

ConsecutiveImagePixelsOrder::ConsecutiveImagePixelsOrder(): ImagePixelsOrder()
  {
  }

void ConsecutiveImagePixelsOrder::SetImageSize(const Point2D_i &i_image_begin, const Point2D_i &i_image_end)
  {
  ASSERT(i_image_end[0]>=i_image_begin[0]);
  ASSERT(i_image_end[1]>=i_image_begin[1]);

  m_image_begin=i_image_begin;
  m_image_end=i_image_end;

  m_next_pixel=m_image_begin;
  }

size_t ConsecutiveImagePixelsOrder::GetTotalPixelsNum() const
  {
  return (m_image_end[0]-m_image_begin[0])*(m_image_end[1]-m_image_begin[1]);
  }

void ConsecutiveImagePixelsOrder::Reset()
  {
  m_next_pixel=m_image_begin;
  }

bool ConsecutiveImagePixelsOrder::GetNextPixel(Point2D_i &o_image_pixel)
  {
  if (m_next_pixel[0]>=m_image_end[0] || m_next_pixel[1]>=m_image_end[1])
    return false;

  o_image_pixel=m_next_pixel;
  if (++m_next_pixel[0]>=m_image_end[0])
    {
    m_next_pixel[0]=m_image_begin[0];
    ++m_next_pixel[1];
    }

  return true;
  }