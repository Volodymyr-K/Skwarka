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

#include "FilmFilter.h"
#include <Common/Common.h>

FilmFilter::FilmFilter(double i_x_width, double i_y_width):
m_x_width(i_x_width), m_y_width(i_y_width)
  {
  ASSERT(i_x_width>0.0);
  ASSERT(i_y_width>0.0);
  }

double FilmFilter::GetXWidth() const
  {
  return m_x_width;
  }

double FilmFilter::GetYWidth() const
  {
  return m_y_width;
  }