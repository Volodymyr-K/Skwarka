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

#include "MitchellFilter.h"

MitchellFilter::MitchellFilter(double i_x_width, double i_y_width, double i_b, double i_c):
FilmFilter(i_x_width, i_y_width), m_b(i_b), m_c(i_c)
  {
  m_inv_x_width = i_x_width>0 ? 1.0/i_x_width : 0.0;
  m_inv_y_width = i_y_width>0 ? 1.0/i_y_width : 0.0;
  }

MitchellFilter::MitchellFilter(double i_x_width, double i_y_width, double i_b):
FilmFilter(i_x_width, i_y_width), m_b(i_b), m_c(0.5-0.5*i_b)
  {
  m_inv_x_width = i_x_width>0 ? 1.0/i_x_width : 0.0;
  m_inv_y_width = i_y_width>0 ? 1.0/i_y_width : 0.0;
  }

double MitchellFilter::Evaluate(double i_x, double i_y) const
  {
  return _Mitchell1D(i_x * m_inv_x_width) * _Mitchell1D(i_y * m_inv_y_width);
  }

void MitchellFilter::GetParameters(double &o_b, double &o_c) const
  {
  o_b=m_b;
  o_c=m_c;
  }