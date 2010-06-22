#include "substrate.h"

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/BxDFs/FresnelBlend.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Math/MathRoutines.h>

Substrate::Substrate(intrusive_ptr<const Texture<Spectrum_d> > ip_diffuse_reflectance, intrusive_ptr<const Texture<Spectrum_d> > ip_specular_reflectance, 
                     intrusive_ptr<const Texture<double> > ip_roughness):
Material(), mp_specular_reflectance(ip_specular_reflectance), mp_diffuse_reflectance(ip_diffuse_reflectance), mp_roughness(ip_roughness)
  {
  ASSERT(ip_diffuse_reflectance);
  ASSERT(ip_specular_reflectance);
  ASSERT(ip_roughness);
  }

const BSDF *Substrate::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  typedef FresnelBlend<BlinnDistribution> BlinnFresnelBlend;

  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  double roughness = MathRoutines::Clamp(mp_roughness->Evaluate(i_dg, i_triangle_index), 0.001, 1.0);
  BlinnDistribution blinn(1.0/roughness);

  Spectrum_d diffuse = mp_diffuse_reflectance->Evaluate(i_dg, i_triangle_index);
  Spectrum_d specular = mp_specular_reflectance->Evaluate(i_dg, i_triangle_index);

  BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(BlinnFresnelBlend)) ) BlinnFresnelBlend(diffuse, specular, blinn);
  p_bsdf->AddBxDF(p_bxdf);

  return p_bsdf;
  }
