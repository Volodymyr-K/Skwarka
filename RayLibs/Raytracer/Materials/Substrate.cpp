#include "substrate.h"

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/BxDFs/FresnelBlend.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/MicrofacetDistributions/AnisotropicDistribution.h>
#include <Math/MathRoutines.h>

Substrate::Substrate(intrusive_ptr<const Texture<Spectrum_d> > ip_diffuse_reflectance, intrusive_ptr<const Texture<Spectrum_d> > ip_specular_reflectance, 
                     intrusive_ptr<const Texture<double> > ip_roughness):
Material(), mp_specular_reflectance(ip_specular_reflectance), mp_diffuse_reflectance(ip_diffuse_reflectance), mp_roughness(ip_roughness)
  {
  ASSERT(ip_diffuse_reflectance);
  ASSERT(ip_specular_reflectance);
  ASSERT(ip_roughness);
  }

Substrate::Substrate(intrusive_ptr<const Texture<Spectrum_d> > ip_diffuse_reflectance, intrusive_ptr<const Texture<Spectrum_d> > ip_specular_reflectance, 
                     intrusive_ptr<const Texture<double> > ip_u_roughness, intrusive_ptr<const Texture<double> > ip_v_roughness):
Material(), mp_specular_reflectance(ip_specular_reflectance), mp_diffuse_reflectance(ip_diffuse_reflectance), mp_u_roughness(ip_u_roughness), mp_v_roughness(ip_v_roughness)
  {
  ASSERT(ip_diffuse_reflectance);
  ASSERT(ip_specular_reflectance);
  ASSERT(ip_u_roughness);
  ASSERT(ip_v_roughness);
  }

intrusive_ptr<const Texture<Spectrum_d> > Substrate::GetDiffuseReflectanceTexture() const
  {
  return mp_diffuse_reflectance;
  }

intrusive_ptr<const Texture<Spectrum_d> > Substrate::GetSpecularReflectanceTexture() const
  {
  return mp_specular_reflectance;
  }

intrusive_ptr<const Texture<double> > Substrate::GetRoughnessTexture() const
  {
  return mp_roughness;
  }

intrusive_ptr<const Texture<double> > Substrate::GetURoughnessTexture() const
  {
  return mp_u_roughness;
  }

intrusive_ptr<const Texture<double> > Substrate::GetVRoughnessTexture() const
  {
  return mp_v_roughness;
  }

const BSDF *Substrate::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  Spectrum_d diffuse = mp_diffuse_reflectance->Evaluate(i_dg, i_triangle_index);
  Spectrum_d specular = mp_specular_reflectance->Evaluate(i_dg, i_triangle_index);

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
