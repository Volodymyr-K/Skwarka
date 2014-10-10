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

#ifndef FILM_FILTER_MOCK_H
#define FILM_FILTER_MOCK_H

#include <Raytracer/Core/FilmFilter.h>

/*
FilmFilter mock implementation.
Always generates the same value (just like box filter).
*/
class FilmFilterMock: public FilmFilter
  {
  public:
    FilmFilterMock(double i_x_width, double i_y_width) : FilmFilter(i_x_width, i_y_width)
      {
      }

    double Evaluate(double i_x, double i_y) const
      {
      if (i_x<-GetXWidth() || i_x>GetXWidth() || i_y<-GetYWidth() || i_y>GetYWidth())
        return 0.0;

      return 1.0;
      }
  };

#endif // FILM_FILTER_MOCK_H