#include "Transparent.h"

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/BxDFs/SpecularReflection.h>
#include <Raytracer/BxDFs/SpecularTransmission.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/Util.h>

Transparent::Transparent(intrusive_ptr<Texture<Spectrum_d> > ip_reflectance, intrusive_ptr<Texture<Spectrum_d> > ip_transmittance, double i_refractive_index):
Material(), mp_reflectance(ip_reflectance), mp_transmittance(ip_transmittance), m_refractive_index(i_refractive_index)
  {
  ASSERT(ip_reflectance);
  ASSERT(ip_transmittance);

  ASSERT(i_refractive_index>0.0);
  if (i_refractive_index<0.0)
    m_refractive_index=0.0;
  }

BSDF *Transparent::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg, m_refractive_index);

  Spectrum_d r = mp_reflectance->Evaluate(i_dg, i_triangle_index);
  Spectrum_d t = mp_transmittance->Evaluate(i_dg, i_triangle_index);
  r.Clamp(0.0, 1.0);
  t.Clamp(0.0, 1.0);

  FresnelDielectric fresnel(m_refractive_index, 1.0);
  BxDF *p_reflection = new ( i_pool.Alloc(sizeof(SpecularReflection<FresnelDielectric>)) ) SpecularReflection<FresnelDielectric>(r, fresnel);
  p_bsdf->AddBxDF(p_reflection);

  BxDF *p_transmission = new ( i_pool.Alloc(sizeof(SpecularTransmission)) ) SpecularTransmission(r, m_refractive_index, 1.0);
  p_bsdf->AddBxDF(p_transmission);

  return p_bsdf;
  }
