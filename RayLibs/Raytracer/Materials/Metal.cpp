#include "Metal.h"

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/BxDFs/Microfacet.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/MathRoutines.h>

Metal::Metal(intrusive_ptr<const Texture<Spectrum_d> > ip_refractive_index, intrusive_ptr<const Texture<Spectrum_d> > ip_absoprtion,
             intrusive_ptr<const Texture<double> > ip_roughness):
Material(), mp_refractive_index(ip_refractive_index), mp_absoprtion(ip_absoprtion), mp_roughness(ip_roughness)
  {
  ASSERT(ip_refractive_index);
  ASSERT(ip_absoprtion);
  ASSERT(ip_roughness);
  }

Metal::Metal(intrusive_ptr<const Texture<Spectrum_d> > ip_reflectance, intrusive_ptr<const Texture<double> > ip_roughness):
Material(), mp_reflectance(ip_reflectance), mp_roughness(ip_roughness)
  {
  ASSERT(ip_reflectance);
  ASSERT(ip_roughness);
  }

intrusive_ptr<const Texture<Spectrum_d> > Metal::GetRefractiveIndexTexture() const
  {
  return mp_refractive_index;
  }

intrusive_ptr<const Texture<Spectrum_d> > Metal::GetAbsoprtionTexture() const
  {
  return mp_absoprtion;
  }

intrusive_ptr<const Texture<Spectrum_d> > Metal::GetReflectanceTexture() const
  {
  return mp_reflectance;
  }

intrusive_ptr<const Texture<double> > Metal::GetRoughnessTexture() const
  {
  return mp_roughness;
  }

const BSDF *Metal::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  typedef Microfacet<FresnelConductor,BlinnDistribution> BlinnMicrofacetMetal;

  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  double roughness = MathRoutines::Clamp(mp_roughness->Evaluate(i_dg, i_triangle_index), 0.001, 1.0);
  BlinnDistribution blinn(1.0/roughness);

  Spectrum_d r, a;
  if (mp_refractive_index && mp_absoprtion)
    {
    r = mp_refractive_index->Evaluate(i_dg, i_triangle_index);
    a = mp_absoprtion->Evaluate(i_dg, i_triangle_index);
    }
  else
    {
    ASSERT(mp_reflectance);
    Spectrum_d reflectance = mp_reflectance->Evaluate(i_dg, i_triangle_index);
    reflectance.Clamp(0.0, 1.0);

    ApproximateFresnelParameters(reflectance, r, a);
    }

  FresnelConductor fresnel(r, a);
  BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(BlinnMicrofacetMetal)) ) BlinnMicrofacetMetal(Spectrum_d(1.0), fresnel, blinn);
  p_bsdf->AddBxDF(p_bxdf);

  return p_bsdf;
  }
