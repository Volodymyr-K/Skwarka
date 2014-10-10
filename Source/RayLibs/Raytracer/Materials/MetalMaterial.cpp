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

#include "MetalMaterial.h"

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/BxDFs/Microfacet.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/MicrofacetDistributions/AnisotropicDistribution.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/MathRoutines.h>

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_IMPLEMENT(MetalMaterial);

MetalMaterial::MetalMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_refractive_index, intrusive_ptr<const Texture<SpectrumCoef_d>> ip_absorption,
             intrusive_ptr<const Texture<double>> ip_roughness):
Material(), mp_refractive_index(ip_refractive_index), mp_absorption(ip_absorption), mp_roughness(ip_roughness)
  {
  ASSERT(ip_refractive_index);
  ASSERT(ip_absorption);
  ASSERT(ip_roughness);
  }

MetalMaterial::MetalMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_refractive_index, intrusive_ptr<const Texture<SpectrumCoef_d>> ip_absorption,
             intrusive_ptr<const Texture<double>> ip_u_roughness, intrusive_ptr<const Texture<double>> ip_v_roughness):
Material(), mp_refractive_index(ip_refractive_index), mp_absorption(ip_absorption), mp_u_roughness(ip_u_roughness), mp_v_roughness(ip_v_roughness)
  {
  ASSERT(ip_refractive_index);
  ASSERT(ip_absorption);
  ASSERT(ip_u_roughness);
  ASSERT(ip_v_roughness);
  }

MetalMaterial::MetalMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_reflectance, intrusive_ptr<const Texture<double>> ip_roughness):
  Material(), mp_reflectance(ip_reflectance), mp_roughness(ip_roughness)
    {
    ASSERT(ip_reflectance);
    ASSERT(ip_roughness);
    }

MetalMaterial::MetalMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_reflectance, intrusive_ptr<const Texture<double>> ip_u_roughness, intrusive_ptr<const Texture<double>> ip_v_roughness):
Material(), mp_reflectance(ip_reflectance), mp_u_roughness(ip_u_roughness), mp_v_roughness(ip_v_roughness)
  {
  ASSERT(ip_reflectance);
  ASSERT(ip_u_roughness);
  ASSERT(ip_v_roughness);
  }

const BSDF *MetalMaterial::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  SpectrumCoef_d r, a;
  if (mp_refractive_index && mp_absorption)
    {
    r = mp_refractive_index->Evaluate(i_dg, i_triangle_index);
    a = mp_absorption->Evaluate(i_dg, i_triangle_index);
    }
  else
    {
    ASSERT(mp_reflectance);
    SpectrumCoef_d reflectance = mp_reflectance->Evaluate(i_dg, i_triangle_index);
    reflectance.Clamp(0.0, 1.0);

    ApproximateFresnelParameters(reflectance, r, a);
    }
  FresnelConductor fresnel(r, a);

  BxDF *p_bxdf;
  if (mp_roughness)
    {
    typedef Microfacet<FresnelConductor,BlinnDistribution> BlinnMicrofacetMetal;
    double roughness = MathRoutines::Clamp(mp_roughness->Evaluate(i_dg, i_triangle_index), 0.001, 1.0);
    BlinnDistribution blinn(1.0/roughness);

    p_bxdf = new ( i_pool.Alloc(sizeof(BlinnMicrofacetMetal)) ) BlinnMicrofacetMetal(SpectrumCoef_d(1.0), fresnel, blinn);
    }
  else
    {
    ASSERT(mp_u_roughness && mp_v_roughness);

    typedef Microfacet<FresnelConductor,AnisotropicDistribution> AnisotropicMicrofacetMetal;
    double u_roughness = MathRoutines::Clamp(mp_u_roughness->Evaluate(i_dg, i_triangle_index), 0.001, 1.0);
    double v_roughness = MathRoutines::Clamp(mp_v_roughness->Evaluate(i_dg, i_triangle_index), 0.001, 1.0);
    AnisotropicDistribution anisotropic(1.0/u_roughness, 1.0/v_roughness);

    p_bxdf = new ( i_pool.Alloc(sizeof(AnisotropicMicrofacetMetal)) ) AnisotropicMicrofacetMetal(SpectrumCoef_d(1.0), fresnel, anisotropic);
    }

  p_bsdf->AddBxDF(p_bxdf);
  return p_bsdf;
  }
