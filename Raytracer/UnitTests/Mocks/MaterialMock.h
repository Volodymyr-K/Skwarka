#ifndef MATERIAL_MOCK_H
#define MATERIAL_MOCK_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>
#include "BxDFMock.h"

/*
Material mock implementation.
Returns a BSDF with a single diffuse reflection component everywhere.
*/
class MaterialMock: public Material
  {
  public:
    virtual BSDF *GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

BSDF *MaterialMock::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  BSDF *p_bsdf = new ( i_pool.Alloc(sizeof(BSDF)) ) BSDF(i_dg);

  BxDF *p_bxdf = new ( i_pool.Alloc(sizeof(BxDFMock)) ) BxDFMock(BxDFType(BSDF_REFLECTION|BSDF_DIFFUSE));
  p_bsdf->AddBxDF(p_bxdf);

  return p_bsdf;
  }


#endif // MATERIAL_MOCK_H