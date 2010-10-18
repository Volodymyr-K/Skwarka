#ifndef MIX_MATERIAL_TEST_H
#define MIX_MATERIAL_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Materials/MixMaterial.h>
#include "Mocks/MaterialMock.h"

class MixMaterialTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_MixMaterial_TwoMaterials()
      {
      SpectrumCoef_d refl1(0.1,0.2,0.3), refl2(0.4,0.8,0.1), scale(0.3,0.5,0.7);
      intrusive_ptr<Material> p_material1( new MaterialMock(refl1) );
      intrusive_ptr<Material> p_material2( new MaterialMock(refl2) );
      intrusive_ptr<Texture<SpectrumCoef_d> > p_scale( new ConstantTexture<SpectrumCoef_d>(scale) );

      intrusive_ptr<Material> p_material3(new MixMaterial(p_material1, p_material2, p_scale) );

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      
      const BSDF *p_bsdf1 = p_material1->GetBSDF(dg, 0, pool);
      const BSDF *p_bsdf2 = p_material2->GetBSDF(dg, 0, pool);
      const BSDF *p_bsdf3 = p_material3->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf3 != NULL);
      TS_ASSERT_EQUALS(p_bsdf3->GetComponentsNum(), p_bsdf1->GetComponentsNum()+p_bsdf2->GetComponentsNum());

      SpectrumCoef_d val1 = p_bsdf1->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));
      SpectrumCoef_d val2 = p_bsdf2->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));
      SpectrumCoef_d val3 = p_bsdf3->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));

      CustomAssertDelta(val3, val1*scale+val2*(SpectrumCoef_d(1.0)-scale), (1e-9));
      }

    void test_MixMaterial_OneMaterial()
      {
      SpectrumCoef_d refl1(0.1,0.2,0.3), scale(0.3,0.5,0.7);
      intrusive_ptr<Material> p_material1( new MaterialMock(refl1) );
      intrusive_ptr<Texture<SpectrumCoef_d> > p_scale( new ConstantTexture<SpectrumCoef_d>(scale) );

      intrusive_ptr<Material> p_material3(new MixMaterial(p_material1, NULL, p_scale) );

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);

      const BSDF *p_bsdf1 = p_material1->GetBSDF(dg, 0, pool);
      const BSDF *p_bsdf3 = p_material3->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf3 != NULL);
      TS_ASSERT_EQUALS(p_bsdf3->GetComponentsNum(), p_bsdf1->GetComponentsNum());

      SpectrumCoef_d val1 = p_bsdf1->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));
      SpectrumCoef_d val3 = p_bsdf3->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));

      CustomAssertDelta(val3, val1*scale, (1e-9));
      }
  };

#endif // MIX_MATERIAL_TEST_H
