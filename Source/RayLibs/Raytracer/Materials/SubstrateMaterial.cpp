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

#include "SubstrateMaterial.h"

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/BxDFs/FresnelBlend.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/MicrofacetDistributions/AnisotropicDistribution.h>
#include <Math/MathRoutines.h>

SubstrateMaterial::SubstrateMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_diffuse_reflectance, intrusive_ptr<const Texture<SpectrumCoef_d>> ip_specular_reflectance, 
                     intrusive_ptr<const Texture<double>> ip_roughness):
Material(), mp_specular_reflectance(ip_specular_reflectance), mp_diffuse_reflectance(ip_diffuse_reflectance), mp_roughness(ip_roughness)
  {
  ASSERT(ip_diffuse_reflectance);
  ASSERT(ip_specular_reflectance);
  ASSERT(ip_roughness);
  }

SubstrateMaterial::SubstrateMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_diffuse_reflectance, intrusive_ptr<const Texture<SpectrumCoef_d>> ip_specular_reflectance, 
                     intrusive_ptr<const Texture<double>> ip_u_roughness, intrusive_ptr<const Texture<double>> ip_v_roughness):
Material(), mp_specular_reflectance(ip_specular_reflectance), mp_diffuse_reflectance(ip_diffuse_reflectance), mp_u_roughness(ip_u_roughness), mp_v_roughness(ip_v_roughness)
  {
  ASSERT(ip_diffuse_reflectance);
  ASSERT(ip_specular_reflectance);
  ASSERT(ip_u_roughness);
  ASSERT(ip_v_roughness);
  }

const BSDF *SubstrateMaterial::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  SpectrumCoef_d diffuse = mp_diffuse_reflectance->Evaluate(i_dg, i_triangle_index);
  SpectrumCoef_d specular = mp_specular_reflectance->Evaluate(i_dg, i_triangle_index);
  diffuse.Clamp(0.0, 1.0);
  specular.Clamp(0.0, 1.0);

  BxDF *p_bxdf;
  if (mp_roughness)
    {
    typedef FresnelBlend<BlinnDistribution> BlinnFresnelBlend;
    double roughness = MathRoutines::Clamp(mp_roughness->Evaluate(i_dg, i_triangle_index), 0.001, 1.0);
    BlinnDistribution blinn(1.0/roughness);

    p_bxdf = new ( i_pool.Alloc(sizeof(BlinnFresnelBlend)) ) BlinnFresnelBlend(diffuse, specular, blinn);
    }
  else
    {
    typedef FresnelBlend<AnisotropicDistribution> AnisotropicFresnelBlend;
    double u_roughness = MathRoutines::Clamp(mp_u_roughness->Evaluate(i_dg, i_triangle_index), 0.001, 1.0);
    double v_roughness = MathRoutines::Clamp(mp_v_roughness->Evaluate(i_dg, i_triangle_index), 0.001, 1.0);
    AnisotropicDistribution anisotropic(1.0/u_roughness, 1.0/v_roughness);

    p_bxdf = new ( i_pool.Alloc(sizeof(AnisotropicFresnelBlend)) ) AnisotropicFresnelBlend(diffuse, specular, anisotropic);
    }

  p_bsdf->AddBxDF(p_bxdf);
  return p_bsdf;
  }
