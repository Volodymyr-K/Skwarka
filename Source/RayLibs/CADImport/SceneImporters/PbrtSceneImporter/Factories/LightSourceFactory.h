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

#ifndef PBRT_LIGHT_SOURCE_FACTORY
#define PBRT_LIGHT_SOURCE_FACTORY

#include <Common/Common.h>
#include "../PbrtParamSet.h"
#include <Raytracer/LightSources/ImageEnvironmentalLight.h>
#include <Raytracer/LightSources/ParallelLight.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Raytracer/LightSources/SpotPointLight.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Math/MathRoutines.h>
#include "../PbrtUtils.h"

namespace PbrtImport
  {

  class LightSourceFactory
    {
    public:
      LightSourceFactory(intrusive_ptr<Log> ip_log): mp_log(ip_log) {}

      intrusive_ptr<const DeltaLightSource> CreateDeltaLight(const std::string &i_name, const Transform &i_light_to_world,  const ParamSet &i_params, BBox3D_d i_world_bounds) const
        {
        if (i_name == "point")
          return _CreatePointLight(i_light_to_world, i_params);
        else if (i_name == "spot")
          return _CreateSpotLight(i_light_to_world, i_params);
        else if (i_name == "goniometric")
          return _CreateGoniometricLight(i_light_to_world, i_params);
        else if (i_name == "projection")
          return _CreateProjectionLight(i_light_to_world, i_params);
        else if (i_name == "distant")
          return _CreateDistantLight(i_light_to_world, i_params, i_world_bounds);

        return NULL;
        }

      intrusive_ptr<const InfiniteLightSource> CreateInfiniteLight(const std::string &i_name, const Transform &i_light_to_world,  const ParamSet &i_params, BBox3D_d i_world_bounds) const
        {
        if (i_name == "infinite" || i_name == "exinfinite")
          return _CreateInfiniteLight(i_light_to_world, i_params, i_world_bounds);

        return NULL;
        }

      intrusive_ptr<const AreaLightSource> CreateAreaLight(const std::string &i_name, const Transform &i_light_to_world,  const ParamSet &i_params, intrusive_ptr<const TriangleMesh> ip_mesh) const
        {
        if (i_name == "area" || i_name == "diffuse")
          return _CreateDiffuseAreaLight(i_light_to_world, i_params, ip_mesh);

        PbrtImport::Utils::LogError(mp_log, std::string("Area light \"") + i_name + std::string("\" unknown."));
        return NULL;
        }

    private:
      intrusive_ptr<const DeltaLightSource> _CreatePointLight(const Transform &i_light_to_world,  const ParamSet &i_params) const
        {
        Spectrum_d I = i_params.FindOneSpectrum("I", Spectrum_d(1.0));
        SpectrumCoef_d sc = i_params.FindOneSpectrumCoef("scale", SpectrumCoef_d(1.0));
        Point3D_d P = i_params.FindOnePoint("from", Point3D_d(0,0,0));
        return new PointLight(i_light_to_world(P), I * sc);
        }

      intrusive_ptr<const DeltaLightSource> _CreateSpotLight(const Transform &i_light_to_world,  const ParamSet &i_params) const
        {
        Spectrum_d I = i_params.FindOneSpectrum("I", Spectrum_d(1.0));
        SpectrumCoef_d sc = i_params.FindOneSpectrumCoef("scale", SpectrumCoef_d(1.0));
        float coneangle = i_params.FindOneFloat("coneangle", 30.);
        float conedelta = i_params.FindOneFloat("conedeltaangle", 5.);

        // Compute spotlight world to light transformation.
        Point3D_d from = i_params.FindOnePoint("from", Point3D_d(0,0,0));
        Point3D_d to = i_params.FindOnePoint("to", Point3D_d(0,0,1));
        Vector3D_d dir = Vector3D_d(to - from).Normalized();

        return new SpotPointLight(i_light_to_world(from), i_light_to_world(dir), I * sc,
                                  MathRoutines::DegreesToRadians(coneangle-conedelta), MathRoutines::DegreesToRadians(coneangle));
        }

      intrusive_ptr<const DeltaLightSource> _CreateGoniometricLight(const Transform &i_light_to_world,  const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Goniometric light is not supported.");
        return NULL;
        }

      intrusive_ptr<const DeltaLightSource> _CreateProjectionLight(const Transform &i_light_to_world,  const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Projection light is not supported.");
        return NULL;
        }

      intrusive_ptr<const DeltaLightSource> _CreateDistantLight(const Transform &i_light_to_world,  const ParamSet &i_params, BBox3D_d i_world_bounds) const
        {
        Spectrum_d L = i_params.FindOneSpectrum("L", Spectrum_d(1.0));
        SpectrumCoef_d sc = i_params.FindOneSpectrumCoef("scale", SpectrumCoef_d(1.0));

        Point3D_d from = i_params.FindOnePoint("from", Point3D_d(0,0,0));
        Point3D_d to = i_params.FindOnePoint("to", Point3D_d(0,0,1));
        Vector3D_d dir = Vector3D_d(to - from).Normalized();

        return new ParallelLight(i_light_to_world(dir), L * sc, i_world_bounds);
        }

      intrusive_ptr<const InfiniteLightSource> _CreateInfiniteLight(const Transform &i_light_to_world,  const ParamSet &i_params, BBox3D_d i_world_bounds) const
        {
        Spectrum_d L = i_params.FindOneSpectrum("L", Spectrum_d(1.0));
        SpectrumCoef_d sc = i_params.FindOneSpectrumCoef("scale", SpectrumCoef_d(1.0));

        std::string texmap = i_params.FindOneFilename("mapname", "");

        if (texmap.empty()==false)
          {
          intrusive_ptr<const ImageSource<Spectrum_f>> p_image_source =
            PbrtImport::Utils::CreateImageSourceFromFile<Spectrum_f>(texmap, false, 1.0, mp_log);

          if (p_image_source && p_image_source->GetHeight() > 0)
            return new ImageEnvironmentalLight(i_world_bounds, i_light_to_world, p_image_source, sc);
          }

        std::vector<std::vector<Spectrum_f>> values(1, std::vector<Spectrum_f>(1, Convert<float>(L)));
        return new ImageEnvironmentalLight(i_world_bounds, i_light_to_world, values, sc);
        }

      intrusive_ptr<const AreaLightSource> _CreateDiffuseAreaLight(const Transform &i_light_to_world,  const ParamSet &i_params, intrusive_ptr<const TriangleMesh> ip_mesh) const
        {
        Spectrum_d L = i_params.FindOneSpectrum("L", Spectrum_d(1.0));
        SpectrumCoef_d sc = i_params.FindOneSpectrumCoef("scale", SpectrumCoef_d(1.0));

        return new DiffuseAreaLightSource(L*sc, ip_mesh);
        }

    private:
      intrusive_ptr<Log> mp_log;
    };

  };

#endif // PBRT_LIGHT_SOURCE_FACTORY