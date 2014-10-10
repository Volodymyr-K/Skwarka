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

#include "UberMaterial.h"
#include <Raytracer/BxDFs/SpecularReflection.h>
#include <Raytracer/BxDFs/SpecularTransmission.h>
#include <Raytracer/Core/Fresnel.h>
#include <Raytracer/BxDFs/Microfacet.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/BxDFs/Lambertian.h>

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_IMPLEMENT(UberMaterial);

UberMaterial::UberMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Kd,
                           intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Ks,
                           intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Kr,
                           intrusive_ptr<const Texture<double>> ip_roughness,
                           intrusive_ptr<const Texture<SpectrumCoef_d>> ip_opacity,
                           double i_refractive_index):
Material(), mp_Kd(ip_Kd), mp_Ks(ip_Ks), mp_Kr(ip_Kr), mp_roughness(ip_roughness), mp_opacity(ip_opacity), m_refractive_index(i_refractive_index)
  {
  ASSERT(ip_Kd);
  ASSERT(ip_Ks);
  ASSERT(ip_Kr);
  ASSERT(ip_roughness);
  ASSERT(ip_opacity);

  ASSERT(i_refractive_index>0.0);
  if (i_refractive_index<0.0)
    m_refractive_index=0.0;
  }

const BSDF *UberMaterial::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  SpectrumCoef_d op = mp_opacity->Evaluate(i_dg, i_triangle_index);
  op.Clamp(0.0, 1.0);
  if (InRange(op, 1.0-(1e-2), 1.0+(1e-2))==false)
    {
    BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(SpecularTransmission)) ) SpecularTransmission(SpectrumCoef_d(1.0) - op, m_refractive_index, 1.0);
    p_bsdf->AddBxDF(p_bxdf);
    }

  SpectrumCoef_d kd = op * mp_Kd->Evaluate(i_dg, i_triangle_index);
  kd.Clamp(0.0, 1.0);
  if (kd.IsBlack()==false)
    {
    BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(Lambertian)) ) Lambertian(kd);
    p_bsdf->AddBxDF(p_bxdf);
    }

  FresnelDielectric fresnel(m_refractive_index, 1.0);
  SpectrumCoef_d ks = op * mp_Ks->Evaluate(i_dg, i_triangle_index);
  ks.Clamp(0.0, 1.0);
  if (ks.IsBlack()==false)
    {
    typedef Microfacet<FresnelDielectric,BlinnDistribution> BlinnMicrofacetDielectric;

    double roughness = mp_roughness->Evaluate(i_dg, i_triangle_index);
    BlinnDistribution blinn(1.0/roughness);

    BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(BlinnMicrofacetDielectric)) ) BlinnMicrofacetDielectric(ks, fresnel, blinn);
    p_bsdf->AddBxDF(p_bxdf);
    }

  SpectrumCoef_d kr = mp_Kr->Evaluate(i_dg, i_triangle_index);
  kr.Clamp(0.0, 1.0);
  if (kr.IsBlack()==false)
    {
    BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(SpecularReflection<FresnelDielectric>)) ) SpecularReflection<FresnelDielectric>(kr, fresnel);
    p_bsdf->AddBxDF(p_bxdf);
    }

  ASSERT(InRange(kd+ks+kr, 0.0, 1.0));
  return p_bsdf;
  }