#ifndef UBER_MATERIAL_TEST_H
#define UBER_MATERIAL_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/UberMaterial.h>
#include "Mocks/TextureMock.h"

class UberMaterialTestSuite : public CxxTest::TestSuite
  {
  public:

    // Just evaluate the BSDF with some random parameters, no special check here.
    void test_UberMaterial()
      {
      intrusive_ptr<Texture<SpectrumCoef_d> > p_Kd( new TextureMock<SpectrumCoef_d>(SpectrumCoef_d(0.1,0.2,0.3)) );
      intrusive_ptr<Texture<SpectrumCoef_d> > p_Ks( new TextureMock<SpectrumCoef_d>(SpectrumCoef_d(0.3,0.1,0.05)) );
      intrusive_ptr<Texture<SpectrumCoef_d> > p_Kr( new TextureMock<SpectrumCoef_d>(SpectrumCoef_d(0.15,0.12,0.25)) );
      intrusive_ptr<Texture<SpectrumCoef_d> > p_opacity( new TextureMock<SpectrumCoef_d>(SpectrumCoef_d(0.2,0.2,0.2)) );
      intrusive_ptr<Texture<double> > p_roughness( new TextureMock<double>(0.1) );

      intrusive_ptr<Material> p_material(new UberMaterial(p_Kd, p_Ks, p_Kr, p_roughness, p_opacity, 1.5));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      const BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);
      TS_ASSERT(p_bsdf != NULL);
      }

  };

#endif // UBER_MATERIAL_TEST_H
