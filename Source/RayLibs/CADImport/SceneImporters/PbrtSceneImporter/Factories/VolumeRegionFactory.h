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

#ifndef PBRT_VOLUME_REGION_FACTORY
#define PBRT_VOLUME_REGION_FACTORY

#include <Common/Common.h>
#include "../PbrtParamSet.h"
#include <Raytracer/VolumeRegions/GridDensityVolumeRegion.h>
#include <Raytracer/VolumeRegions/HomogeneousVolumeRegion.h>
#include <Raytracer/PhaseFunctions/HGPhaseFunction.h>
#include "../PbrtUtils.h"

namespace PbrtImport
  {

  class VolumeRegionFactory
    {
    public:
      VolumeRegionFactory(intrusive_ptr<Log> ip_log): mp_log(ip_log) {}

      intrusive_ptr<const VolumeRegion> CreateVolumeRegion(const std::string &i_name, const Transform &i_volume_to_world, const ParamSet &i_params) const
        {
        if (i_name == "homogeneous")
          return _CreateHomogeneousVolumeDensityRegion(i_volume_to_world, i_params);
        else if (i_name == "volumegrid")
          return _CreateGridVolumeRegion(i_volume_to_world, i_params);
        else if (i_name == "exponential")
          return _CreateExponentialVolumeRegion(i_volume_to_world, i_params);
        else
          PbrtImport::Utils::LogError(mp_log, std::string("Volume region \"") + i_name + std::string("\" unknown."));

        return NULL;
        }
    private:

      intrusive_ptr<const VolumeRegion> _CreateHomogeneousVolumeDensityRegion( const Transform &i_volume_to_world, const ParamSet &i_params) const
        {
        // Initialize common volume region parameters
        SpectrumCoef_d sigma_a = i_params.FindOneSpectrumCoef("sigma_a", SpectrumCoef_d(0.0));
        SpectrumCoef_d sigma_s = i_params.FindOneSpectrumCoef("sigma_s", SpectrumCoef_d(0.0));
        float g = i_params.FindOneFloat("g", 0.f);
        Spectrum_d Le = i_params.FindOneSpectrum("Le", Spectrum_d(0.0));
        Point3D_d p0 = i_params.FindOnePoint("p0", Point3D_d(0,0,0));
        Point3D_d p1 = i_params.FindOnePoint("p1", Point3D_d(1,1,1));

        intrusive_ptr<PhaseFunction> p_phase_function(new HGPhaseFunction(g));
        return new HomogeneousVolumeRegion(BBox3D_d(i_volume_to_world(p0),i_volume_to_world(p1)), Le, sigma_a, sigma_s, p_phase_function);
        }

      intrusive_ptr<const VolumeRegion> _CreateGridVolumeRegion( const Transform &i_volume_to_world, const ParamSet &i_params) const
        {
        // Initialize common volume region parameters
        SpectrumCoef_d sigma_a = i_params.FindOneSpectrumCoef("sigma_a", SpectrumCoef_d(0.0));
        SpectrumCoef_d sigma_s = i_params.FindOneSpectrumCoef("sigma_s", SpectrumCoef_d(0.0));
        float g = i_params.FindOneFloat("g", 0.f);
        Spectrum_d Le = i_params.FindOneSpectrum("Le", Spectrum_d(0.0));
        Point3D_d p0 = i_params.FindOnePoint("p0", Point3D_d(0,0,0));
        Point3D_d p1 = i_params.FindOnePoint("p1", Point3D_d(1,1,1));

        int nitems;
        const float *data = i_params.FindFloat("density", &nitems);
        if (!data)
          {
          PbrtImport::Utils::LogError(mp_log, "No \"density\" values provided for volume grid?");
          return NULL;
          }

        int nx = i_params.FindOneInt("nx", 1);
        int ny = i_params.FindOneInt("ny", 1);
        int nz = i_params.FindOneInt("nz", 1);
        if (nitems != nx*ny*nz)
          {
          std::string message = std::string("VolumeGridDensity has ");
          message += nitems; message += " density values but nx*ny*nz = "; message += nx*ny*nz;
          PbrtImport::Utils::LogError(mp_log, message);
          return NULL;
          }

        size_t ind = 0;
        std::vector<std::vector<std::vector<float>>> densities(nx, std::vector<std::vector<float>>(ny, std::vector<float>(nz)));
        for(int z=0;z<nz;++z)
          for(int y=0;y<ny;++y)
            for(int x=0;x<nx;++x)
              densities[x][y][z] = data[ind++];

        intrusive_ptr<PhaseFunction> p_phase_function(new HGPhaseFunction(g));
        return new GridDensityVolumeRegion(BBox3D_d(i_volume_to_world(p0),i_volume_to_world(p1)), Le, sigma_a, sigma_s, p_phase_function, densities);
        }

      intrusive_ptr<const VolumeRegion> _CreateExponentialVolumeRegion( const Transform &i_volume_to_world, const ParamSet &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Exponential volume region is not supported");
        return NULL;
        }

    private:
      intrusive_ptr<Log> mp_log;
    };

  };

#endif // PBRT_VOLUME_REGION_FACTORY