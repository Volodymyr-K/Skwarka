#include "MERLMeasuredMaterial.h"

#include <Raytracer/Core/BxDF.h>

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_IMPLEMENT(MERLMeasuredMaterial);

MERLMeasuredMaterial::MERLMeasuredMaterial(intrusive_ptr<const MERLMeasuredData> ip_merl_measured_data): mp_merl_measured_data(ip_merl_measured_data)
  {
  ASSERT(ip_merl_measured_data);
  }

const BSDF *MERLMeasuredMaterial::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(MERLMeasured)) ) MERLMeasured(mp_merl_measured_data.get());
  p_bsdf->AddBxDF(p_bxdf);

  return p_bsdf;
  }