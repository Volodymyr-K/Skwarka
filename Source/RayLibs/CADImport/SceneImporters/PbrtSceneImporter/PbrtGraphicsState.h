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

#ifndef PBRT_GRAPHICS_STATE_H
#define PBRT_GRAPHICS_STATE_H

#include "PbrtParamSet.h"
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/Spectrum.h>
#include <string>
#include <map>

namespace PbrtImport
  {

  ////////////////////////////////////////// GraphicsState ///////////////////////////////////////////////

  struct GraphicsState
    {
    GraphicsState(intrusive_ptr<Log> ip_log = NULL);

    intrusive_ptr<const Material> CreateMaterial(const ParamSet &i_params) const;

    std::map<std::string, intrusive_ptr<const Texture<double>>> floatTextures;
    std::map<std::string, intrusive_ptr<const Texture<SpectrumCoef_d>>> spectrumTextures;
    ParamSet materialParams;
    std::string material;
    mutable std::map<std::string, intrusive_ptr<const Material>> namedMaterials;
    std::string currentNamedMaterial;
    ParamSet areaLightParams;
    std::string areaLight;
    bool reverseOrientation;

    intrusive_ptr<Log> mp_log;
    mutable std::map<const Material *, intrusive_ptr<const Texture<double>>> m_material_to_bump_map;
    };

  };

#endif // PBRT_GRAPHICS_STATE_H
