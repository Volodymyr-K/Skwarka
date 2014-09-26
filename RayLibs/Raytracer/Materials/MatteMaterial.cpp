#include "MatteMaterial.h"

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/BxDFs/Lambertian.h>
#include <Raytracer/BxDFs/OrenNayar.h>
#include <Math/MathRoutines.h>

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_IMPLEMENT(MatteMaterial);

MatteMaterial::MatteMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_reflectance, intrusive_ptr<const Texture<double>> ip_sigma):
Material(), mp_reflectance(ip_reflectance), mp_sigma(ip_sigma)
  {
  ASSERT(ip_reflectance);
  ASSERT(ip_sigma);
  }

const BSDF *MatteMaterial::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  SpectrumCoef_d r = mp_reflectance->Evaluate(i_dg, i_triangle_index);
  r.Clamp(0.0, 1.0);

  double sig = MathRoutines::Clamp(mp_sigma->Evaluate(i_dg, i_triangle_index), 0.0, 1.0);

  if (sig < DBL_EPS)
    {
    BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(Lambertian)) ) Lambertian(r);
    p_bsdf->AddBxDF(p_bxdf);
    }
  else
    {
    BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(OrenNayar)) ) OrenNayar(r, sig);
    p_bsdf->AddBxDF(p_bxdf);
    }

  return p_bsdf;
  }
