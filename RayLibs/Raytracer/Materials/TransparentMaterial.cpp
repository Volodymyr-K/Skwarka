#include "TransparentMaterial.h"

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/BxDFs/SpecularReflection.h>
#include <Raytracer/BxDFs/SpecularTransmission.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/MathRoutines.h>

TransparentMaterial::TransparentMaterial(intrusive_ptr<const Texture<SpectrumCoef_d> > ip_reflectance, intrusive_ptr<const Texture<SpectrumCoef_d> > ip_transmittance, double i_refractive_index):
Material(), mp_reflectance(ip_reflectance), mp_transmittance(ip_transmittance), m_refractive_index(i_refractive_index)
  {
  ASSERT(ip_reflectance);
  ASSERT(ip_transmittance);

  ASSERT(i_refractive_index>0.0);
  if (i_refractive_index<0.0)
    m_refractive_index=0.0;
  }

intrusive_ptr<const Texture<SpectrumCoef_d> > TransparentMaterial::GetReflectanceTexture() const
  {
  return mp_reflectance;
  }

intrusive_ptr<const Texture<SpectrumCoef_d> > TransparentMaterial::GetTransmittanceTexture() const
  {
  return mp_transmittance;
  }

double TransparentMaterial::GetRefractiveIndex() const
  {
  return m_refractive_index;
  }

const BSDF *TransparentMaterial::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg, m_refractive_index);

  SpectrumCoef_d r = mp_reflectance->Evaluate(i_dg, i_triangle_index);
  SpectrumCoef_d t = mp_transmittance->Evaluate(i_dg, i_triangle_index);
  r.Clamp(0.0, 1.0);
  t.Clamp(0.0, 1.0);

  if (r.IsBlack()==false)
    {
    FresnelDielectric fresnel(m_refractive_index, 1.0);
    BxDF *p_reflection = new ( i_pool.Alloc(sizeof(SpecularReflection<FresnelDielectric>)) ) SpecularReflection<FresnelDielectric>(r, fresnel);
    p_bsdf->AddBxDF(p_reflection);
    }

  if (t.IsBlack()==false)
    {
    BxDF *p_transmission = new ( i_pool.Alloc(sizeof(SpecularTransmission)) ) SpecularTransmission(t, m_refractive_index, 1.0);
    p_bsdf->AddBxDF(p_transmission);
    }

  return p_bsdf;
  }
