#ifndef MATTE_MATERIAL_TEST_H
#define MATTE_MATERIAL_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/MatteMaterial.h>
#include <Raytracer/BxDFs/OrenNayar.h>
#include "Mocks/TextureMock.h"

class MatteMaterialTestSuite : public CxxTest::TestSuite
  {
  public:
    // Tests for the case when sigma is zero.
    void test_MatteMaterial_EvaluateZeroSigma()
      {
      intrusive_ptr<Texture<SpectrumCoef_d> > p_reflectance( new TextureMock<SpectrumCoef_d>(SpectrumCoef_d(1.0,0.5,0.1)) );
      intrusive_ptr<TextureMock<double> > p_sigma( new TextureMock<double>(0.0) );
      
      intrusive_ptr<Material> p_material(new MatteMaterial(p_reflectance, p_sigma));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      const BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 1);

      SpectrumCoef_d val = p_bsdf->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));
      CustomAssertDelta(val, SpectrumCoef_d(1.0,0.5,0.1)*INV_PI, (1e-6));
      }

    // Tests for the case when sigma is not zero, the returned value should match OrenNayar BxDF.
    void test_MatteMaterial_EvaluateNotZeroSigma()
      {
      intrusive_ptr<Texture<SpectrumCoef_d> > p_reflectance( new TextureMock<SpectrumCoef_d>(SpectrumCoef_d(1.0,0.5,0.1)) );
      intrusive_ptr<TextureMock<double> > p_sigma( new TextureMock<double>(0.1) );

      intrusive_ptr<Material> p_material(new MatteMaterial(p_reflectance, p_sigma));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      const BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 1);

      SpectrumCoef_d val = p_bsdf->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));
      SpectrumCoef_d val2 = OrenNayar(SpectrumCoef_d(1.0,0.5,0.1), 0.1).Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));

      CustomAssertDelta(val, val2, (1e-6));
      }
  };

#endif // MATTE_MATERIAL_TEST_H