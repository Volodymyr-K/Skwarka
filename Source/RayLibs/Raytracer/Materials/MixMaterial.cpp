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

#include "MixMaterial.h"

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/BxDFs/ScaledBxDF.h>
#include <Math/MathRoutines.h>

MixMaterial::MixMaterial(intrusive_ptr<const Material> ip_material1, intrusive_ptr<const Material> ip_material2,
                         intrusive_ptr<const Texture<SpectrumCoef_d>> ip_scale):
Material(), mp_material1(ip_material1), mp_material2(ip_material2), mp_scale(ip_scale)
  {
  ASSERT(ip_material1);
  ASSERT(ip_scale);
  }

const BSDF *MixMaterial::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  SpectrumCoef_d scale1 = mp_scale->Evaluate(i_dg, i_triangle_index);
  scale1.Clamp(0.0, 1.0);

  if (scale1.IsBlack()==false)
    {
    const BSDF *p_bsdf1 = mp_material1->GetBSDF(i_dg, i_triangle_index, i_pool);
    for(size_t i=0;i<p_bsdf1->GetComponentsNum();++i)
      {
      const BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(ScaledBxDF)) ) ScaledBxDF(p_bsdf1->GetComponent(i), scale1);
      p_bsdf->AddBxDF(p_bxdf);
      }
    }

  SpectrumCoef_d scale2 = SpectrumCoef_d(1.0)-scale1;
  if (scale2.IsBlack()==false && mp_material2!=NULL)
    {
    const BSDF *p_bsdf2 = mp_material2->GetBSDF(i_dg, i_triangle_index, i_pool);
    for(size_t i=0;i<p_bsdf2->GetComponentsNum();++i)
      {
      const BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(ScaledBxDF)) ) ScaledBxDF(p_bsdf2->GetComponent(i), scale2);
      p_bsdf->AddBxDF(p_bxdf);
      }
    }

  return p_bsdf;
  }