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

#include "MatteMaterial.h"

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/BxDFs/Lambertian.h>
#include <Raytracer/BxDFs/OrenNayar.h>
#include <Math/MathRoutines.h>

MatteMaterial::MatteMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_reflectance, intrusive_ptr<const Texture<double>> ip_sigma):
Material(), mp_reflectance(ip_reflectance), mp_sigma(ip_sigma)
  {
  ASSERT(ip_reflectance);
  ASSERT(ip_sigma);
  }

const BSDF *MatteMaterial::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  SpectrumCoef_d r = mp_reflectance->Evaluate(i_dg, i_triangle_index);
  r.Clamp(0.0, 1.0);

  double sig = MathRoutines::Clamp(mp_sigma->Evaluate(i_dg, i_triangle_index), 0.0, 1.0);

  if (sig < DBL_EPS)
    {
    BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(Lambertian)) ) Lambertian(r);
    p_bsdf->AddBxDF(p_bxdf);
    }
  else
    {
    BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(OrenNayar)) ) OrenNayar(r, sig);
    p_bsdf->AddBxDF(p_bxdf);
    }

  return p_bsdf;
  }
