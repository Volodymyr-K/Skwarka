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

#ifndef FILM_FILTER_H
#define FILM_FILTER_H

#include <Common/Common.h>

/**
* An abstract class defining the contract for image film filters.
* A film filter is a scalar function defined on 2D. The values of the function define the weights of the samples for a given pixel.
* Film filter has an X and Y widths, defining the region of the function domain where it can take non-zero values.
* @sa ImageFilm
*/
class FilmFilter: public ReferenceCounted
  {
  public:  
    /**
    * Returns the filter value for the specified X and Y coordinates.
    */
    virtual double Evaluate(double i_x, double i_y) const = 0;

    /**
    * Returns the maximum X coordinate for which the function is not zero.
    */
    double GetXWidth() const;

    /**
    * Returns the maximum Y coordinate for which the function is not zero.
    */
    double GetYWidth() const;

    virtual ~FilmFilter() {}

  protected:
    FilmFilter(double i_x_width, double i_y_width);

  private:
    // Not implemented, not a value type.
    FilmFilter(const FilmFilter&);
    FilmFilter &operator=(const FilmFilter&);

  private:
    double m_x_width, m_y_width;
  };

#endif // FILM_FILTER_H