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

#ifndef BOX_FILTER_H
#define BOX_FILTER_H

#include <Raytracer/Core/FilmFilter.h>

/**
* Simplest implementation of the FilmFilter interface returning a constant value everywhere inside the filter's width.
*/
class BoxFilter: public FilmFilter
  {
  public:
    BoxFilter(double i_x_width, double i_y_width);

    /**
    * Returns value 1.0 everywhere inside the filter's width.
    */
    double Evaluate(double i_x, double i_y) const;
  };

#endif // BOX_FILTER_H