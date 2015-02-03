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

#ifndef FILM_MOCK_H
#define FILM_MOCK_H

#include <Raytracer/Core/Film.h>
#include <Raytracer/Core/Spectrum.h>
#include <Math/Point2D.h>

/*
Film mock implementation.
Always generates the same Spectrum value for all pixels.
*/
class FilmMock: public Film
  {
  public:
    FilmMock(size_t i_x_resolution, size_t i_y_resolution): Film(i_x_resolution, i_y_resolution)
      {
      }

    virtual void AddSample(const Point2D_d &i_image_point, const Spectrum_d &i_spectrum)
      {
      }

    virtual void ClearFilm()
      {
      }

    virtual bool GetPixel(const Point2D_i &i_image_point, Spectrum_d &o_spectrum, bool i_clamp_values = true) const
      {
      o_spectrum=Spectrum_d(1.0,1.0,1.0);
      return true;
      }

    virtual void GetSamplingExtent(Point2D_i &o_begin, Point2D_i &o_end) const
      {
      o_begin=Point2D_i(0,0);
      o_end=Point2D_i((int)GetXResolution(), (int)GetYResolution());
      }
  };

#endif // FILM_MOCK_H