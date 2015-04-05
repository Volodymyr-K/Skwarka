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

#include "TranslucentMaterial.h"
#include <Raytracer/Core/Fresnel.h>
#include <Raytracer/BxDFs/Microfacet.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/BxDFs/Lambertian.h>

TranslucentMaterial::TranslucentMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Kd,
                                         intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Ks,
                                         intrusive_ptr<const Texture<double>> ip_roughness,
                                         intrusive_ptr<const Texture<SpectrumCoef_d>> ip_reflectance,
                                         intrusive_ptr<const Texture<SpectrumCoef_d>> ip_transmittance,
                                         double i_refractive_index) :
Material(), mp_Kd(ip_Kd), mp_Ks(ip_Ks), mp_roughness(ip_roughness), mp_reflectance(ip_reflectance), mp_transmittance(ip_transmittance), m_refractive_index(i_refractive_index)
  {
  ASSERT(ip_Kd);
  ASSERT(ip_Ks);
  ASSERT(ip_roughness);
  ASSERT(ip_reflectance);
  ASSERT(ip_transmittance);

  ASSERT(i_refractive_index>0.0);
  if (i_refractive_index<0.0)
    m_refractive_index=0.0;
  }

const BSDF *TranslucentMaterial::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new (i_pool.Alloc(sizeof(BSDF))) BSDF(i_dg);

  SpectrumCoef_d reflectance = mp_reflectance->Evaluate(i_dg, i_triangle_index);
  SpectrumCoef_d transmittance = mp_transmittance->Evaluate(i_dg, i_triangle_index);
  reflectance.Clamp(0.0, 1.0);
  transmittance.Clamp(0.0, 1.0);

  if (reflectance.IsBlack() && transmittance.IsBlack()) return p_bsdf;

  SpectrumCoef_d kd = mp_Kd->Evaluate(i_dg, i_triangle_index);
  kd.Clamp(0.0, 1.0);

  if (kd.IsBlack()==false)
    {
    if (reflectance.IsBlack() == false)
      {
      BxDF *p_bxdf = new (i_pool.Alloc(sizeof(Lambertian))) Lambertian(reflectance*kd);
      p_bsdf->AddBxDF(p_bxdf);
      }
    if (transmittance.IsBlack() == false)
      {
      BxDF *p_bxdf = new (i_pool.Alloc(sizeof(Lambertian))) Lambertian(transmittance*kd);
      p_bsdf->AddBxDF(p_bxdf);
      }
    if (!t.IsBlack()) bsdf->Add(BSDF_ALLOC(arena, BRDFToBTDF)(BSDF_ALLOC(arena, Lambertian)(t * kd)));
    }

  Spectrum ks = Ks->Evaluate(dgs).Clamp();
  if (!ks.IsBlack()) {
    float rough = roughness->Evaluate(dgs);
    if (!r.IsBlack()) {
      Fresnel *fresnel = BSDF_ALLOC(arena, FresnelDielectric)(ior, 1.f);
      bsdf->Add(BSDF_ALLOC(arena, Microfacet)(r * ks, fresnel,
        BSDF_ALLOC(arena, Blinn)(1.f / rough)));
      }
    if (!t.IsBlack()) {
      Fresnel *fresnel = BSDF_ALLOC(arena, FresnelDielectric)(ior, 1.f);
      bsdf->Add(BSDF_ALLOC(arena, BRDFToBTDF)(BSDF_ALLOC(arena, Microfacet)(t * ks, fresnel,
        BSDF_ALLOC(arena, Blinn)(1.f / rough))));
      }
    }
  return bsdf;







  SpectrumCoef_d op = mp_opacity->Evaluate(i_dg, i_triangle_index);
  op.Clamp(0.0, 1.0);
  if (InRange(op, 1.0-(1e-2), 1.0+(1e-2))==false)
    {
    BxDF *p_bxdf = new (i_pool.Alloc(sizeof(SpecularTransmission))) SpecularTransmission(SpectrumCoef_d(1.0) - op, m_refractive_index, 1.0);
    p_bsdf->AddBxDF(p_bxdf);
    }

  SpectrumCoef_d kd = op * mp_Kd->Evaluate(i_dg, i_triangle_index);
  kd.Clamp(0.0, 1.0);
  if (kd.IsBlack()==false)
    {
    BxDF *p_bxdf = new (i_pool.Alloc(sizeof(Lambertian))) Lambertian(kd);
    p_bsdf->AddBxDF(p_bxdf);
    }

  FresnelDielectric fresnel(m_refractive_index, 1.0);
  SpectrumCoef_d ks = op * mp_Ks->Evaluate(i_dg, i_triangle_index);
  ks.Clamp(0.0, 1.0);
  if (ks.IsBlack()==false)
    {
    typedef Microfacet<FresnelDielectric, BlinnDistribution> BlinnMicrofacetDielectric;

    double roughness = mp_roughness->Evaluate(i_dg, i_triangle_index);
    BlinnDistribution blinn(1.0/roughness);

    BxDF *p_bxdf = new (i_pool.Alloc(sizeof(BlinnMicrofacetDielectric))) BlinnMicrofacetDielectric(ks, fresnel, blinn);
    p_bsdf->AddBxDF(p_bxdf);
    }

  SpectrumCoef_d kr = mp_Kr->Evaluate(i_dg, i_triangle_index);
  kr.Clamp(0.0, 1.0);
  if (kr.IsBlack()==false)
    {
    BxDF *p_bxdf = new (i_pool.Alloc(sizeof(SpecularReflection<FresnelDielectric>))) SpecularReflection<FresnelDielectric>(kr, fresnel);
    p_bsdf->AddBxDF(p_bxdf);
    }

  ASSERT(InRange(kd+ks+kr, 0.0, 1.0));
  return p_bsdf;
  }