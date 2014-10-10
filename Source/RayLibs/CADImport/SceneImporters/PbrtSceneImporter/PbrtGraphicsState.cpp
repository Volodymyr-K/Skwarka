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

#include "PbrtGraphicsState.h"
#include "Factories/MaterialFactory.h"
#include "PbrtUtils.h"

namespace PbrtImport
  {

  GraphicsState::GraphicsState(intrusive_ptr<Log> ip_log): mp_log(ip_log)
    {
    material = "matte";
    reverseOrientation = false;
    }

  intrusive_ptr<const Material> GraphicsState::CreateMaterial(const ParamSet &i_params) const
    {
    TextureParams mp(i_params, materialParams, floatTextures, spectrumTextures);
    PbrtImport::MaterialFactory material_factory(m_material_to_bump_map, mp_log);

    intrusive_ptr<const Material> mtl;
    if (currentNamedMaterial != "" && namedMaterials.find(currentNamedMaterial) != namedMaterials.end())
      mtl = namedMaterials[currentNamedMaterial];

    if (!mtl)
      mtl = material_factory.CreateMaterial(material, mp, *this);

    if (!mtl)
      mtl = material_factory.CreateMaterial("matte", mp, *this);

    if (!mtl)
      PbrtImport::Utils::LogError(mp_log, "Unable to create \"matte\" material?!");

    return mtl;
    }

  };