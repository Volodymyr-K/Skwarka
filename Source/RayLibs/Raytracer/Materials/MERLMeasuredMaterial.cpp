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

#include "MERLMeasuredMaterial.h"

#include <Raytracer/Core/BxDF.h>


MERLMeasuredMaterial::MERLMeasuredMaterial(intrusive_ptr<const MERLMeasuredData> ip_merl_measured_data): mp_merl_measured_data(ip_merl_measured_data)
  {
  ASSERT(ip_merl_measured_data);
  }

const BSDF *MERLMeasuredMaterial::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(MERLMeasured)) ) MERLMeasured(mp_merl_measured_data.get());
  p_bsdf->AddBxDF(p_bxdf);

  return p_bsdf;
  }