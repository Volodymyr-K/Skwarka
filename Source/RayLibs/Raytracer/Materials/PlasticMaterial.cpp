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

#include "PlasticMaterial.h"

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/BxDFs/Lambertian.h>
#include <Raytracer/BxDFs/Microfacet.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/MathRoutines.h>

PlasticMaterial::PlasticMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_diffuse_reflectance, intrusive_ptr<const Texture<SpectrumCoef_d>> ip_glossy_reflectance, 
                                 intrusive_ptr<const Texture<double>> ip_roughness):
Material(), mp_glossy_reflectance(ip_glossy_reflectance), mp_diffuse_reflectance(ip_diffuse_reflectance), mp_roughness(ip_roughness)
  {
  ASSERT(ip_diffuse_reflectance);
  ASSERT(ip_glossy_reflectance);
  ASSERT(ip_roughness);
  }

const BSDF *PlasticMaterial::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  SpectrumCoef_d diffuse = mp_diffuse_reflectance->Evaluate(i_dg, i_triangle_index);
  SpectrumCoef_d glossy = mp_glossy_reflectance->Evaluate(i_dg, i_triangle_index);
  diffuse.Clamp(0.0, 1.0);
  glossy.Clamp(0.0, 1.0);

  BxDF *p_lambertian_bxdf = new ( i_pool.Alloc(sizeof(Lambertian)) ) Lambertian(diffuse);

  double roughness = MathRoutines::Clamp(mp_roughness->Evaluate(i_dg, i_triangle_index), 0.001, 1.0);
  BlinnDistribution blinn(1.0/roughness);

  typedef Microfacet<FresnelDielectric,BlinnDistribution> BlinnMicrofacetDielectric;
  BxDF *p_microfacet_bxdf = new ( i_pool.Alloc(sizeof(BlinnMicrofacetDielectric)) ) BlinnMicrofacetDielectric(glossy, FresnelDielectric(1.5, 1.0), blinn);

  p_bsdf->AddBxDF(p_lambertian_bxdf);
  p_bsdf->AddBxDF(p_microfacet_bxdf);
  return p_bsdf;
  }
