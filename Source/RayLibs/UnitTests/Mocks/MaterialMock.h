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

#ifndef MATERIAL_MOCK_H
#define MATERIAL_MOCK_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>
#include "BxDFMock.h"

/*
Material mock implementation.
Returns a BSDF with a single diffuse reflection component everywhere.
*/
class MaterialMock: public Material
  {
  public:
    MaterialMock(SpectrumCoef_d i_refl = SpectrumCoef_d(1.0));

    virtual const BSDF *GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const;

  private:
    SpectrumCoef_d m_refl;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline MaterialMock::MaterialMock(SpectrumCoef_d i_refl): m_refl(i_refl)
  {
  }

inline const BSDF *MaterialMock::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(BxDFMock)) ) BxDFMock(BxDFType(BSDF_REFLECTION|BSDF_DIFFUSE), m_refl);
  p_bsdf->AddBxDF(p_bxdf);

  return p_bsdf;
  }


#endif // MATERIAL_MOCK_H