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

#ifndef PBRT_FILM_FILTER_FACTORY
#define PBRT_FILM_FILTER_FACTORY

#include <Common/Common.h>
#include "../PbrtParamSet.h"
#include <Raytracer/FilmFilters/BoxFilter.h>
#include <Raytracer/FilmFilters/MitchellFilter.h>
#include "../PbrtUtils.h"

namespace PbrtImport
  {

  class FilmFilterFactory
    {
    public:
      FilmFilterFactory(intrusive_ptr<Log> ip_log): mp_log(ip_log) {}

      intrusive_ptr<const FilmFilter> CreateFilmFilter(const std::string &i_name, const ParamSet &i_params) const
        {
        if (i_name == "box")
          return _CreateBoxFilter(i_params);
        else if (i_name == "gaussian")
          return _CreateGaussianFilter(i_params);
        else if (i_name == "mitchell")
          return _CreateMitchellFilter(i_params);
        else if (i_name == "sinc")
          return _CreateSincFilter(i_params);
        else if (i_name == "triangle")
          return _CreateTriangleFilter(i_params);
        else
          PbrtImport::Utils::LogError(mp_log, std::string("Filter \"") + i_name + std::string("\" unknown."));

        return NULL;
        }

    private:
      intrusive_ptr<const FilmFilter> _CreateBoxFilter(const ParamSet &i_params) const
        {
        float xw = i_params.FindOneFloat("xwidth", 0.5f);
        float yw = i_params.FindOneFloat("ywidth", 0.5f);
        return new BoxFilter(xw, yw);
        }

      intrusive_ptr<const FilmFilter> _CreateMitchellFilter(const ParamSet &i_params) const
        {
        float xw = i_params.FindOneFloat("xwidth", 2.f);
        float yw = i_params.FindOneFloat("ywidth", 2.f);
        float B = i_params.FindOneFloat("B", 1.f/3.f);
        float C = i_params.FindOneFloat("C", 1.f/3.f);
        return new MitchellFilter(xw, yw, B, C);
        }

      intrusive_ptr<const FilmFilter> _CreateGaussianFilter(const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Gaussian film filter is not supported");
        return NULL;
        }

      intrusive_ptr<const FilmFilter> _CreateSincFilter(const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Sinc film filter is not supported");
        return NULL;
        }

      intrusive_ptr<const FilmFilter> _CreateTriangleFilter(const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Triangle film filter is not supported");
        return NULL;
        }

    private:
      intrusive_ptr<Log> mp_log;
    };

  };

#endif // PBRT_FILM_FILTER_FACTORY
