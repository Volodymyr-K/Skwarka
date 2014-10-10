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

#ifndef PBRT_FILM_FACTORY
#define PBRT_FILM_FACTORY

#include <Common/Common.h>
#include "../PbrtParamSet.h"
#include <Raytracer/Films/ImageFilm.h>
#include <Math/MathRoutines.h>
#include "../PbrtUtils.h"
#include <cmath>

namespace PbrtImport
  {

  class FilmFactory
    {
    public:
      FilmFactory(intrusive_ptr<Log> ip_log): mp_log(ip_log) {}

      intrusive_ptr<Film> CreateFilm(const std::string &i_name, const ParamSet &i_params, intrusive_ptr<const FilmFilter> ip_filter) const
        {
        intrusive_ptr<Film> p_film;
        if (i_name == "image")
          return _CreateImageFilm(i_params, ip_filter);
        else
          PbrtImport::Utils::LogError(mp_log, std::string("Film \"") + i_name + std::string("\" unknown."));

        return NULL;
        }

    private:
      intrusive_ptr<Film> _CreateImageFilm(const ParamSet &i_params, intrusive_ptr<const FilmFilter> ip_filter) const
        {
        int xres = i_params.FindOneInt("xresolution", 640);
        int yres = i_params.FindOneInt("yresolution", 480);
        float crop[4] = { 0, 1, 0, 1 };
        int cwi;
        const float *cr = i_params.FindFloat("cropwindow", &cwi);
        if (cr && cwi == 4)
          {
          crop[0] = MathRoutines::Clamp(std::min(cr[0], cr[1]), 0.f, 1.f);
          crop[1] = MathRoutines::Clamp(std::max(cr[0], cr[1]), 0.f, 1.f);
          crop[2] = MathRoutines::Clamp(std::min(cr[2], cr[3]), 0.f, 1.f);
          crop[3] = MathRoutines::Clamp(std::max(cr[2], cr[3]), 0.f, 1.f);
          }

        int xPixelStart = (int)std::ceil(xres * crop[0]);
        int xPixelCount = std::max(1, (int)std::ceil(xres * crop[1]) - xPixelStart);
        int yPixelStart = (int)std::ceil(yres * crop[2]);
        int yPixelCount = std::max(1, (int)std::ceil(yres * crop[3]) - yPixelStart);

        intrusive_ptr<ImageFilm> p_film( new ImageFilm(xres, yres, ip_filter) );
        p_film->SetCropWindow(Point2D_i(xPixelStart, yPixelStart), Point2D_i(xPixelStart+xPixelCount, yPixelStart+yPixelCount));
        return p_film;
        }

    private:
      intrusive_ptr<Log> mp_log;
    };

  };

#endif // PBRT_FILM_FACTORY
