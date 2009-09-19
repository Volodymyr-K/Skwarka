#ifndef MATTE_TEST_H
#define MATTE_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/Matte.h>
#include <Raytracer/BxDFs/OrenNayar.h>
#include "Mocks/TextureMock.h"

class MatteTestSuite : public CxxTest::TestSuite
  {
  public:
    // Tests for the case when sigma is zero.
    void test_Matte_EvaluateZeroSigma()
      {
      intrusive_ptr<Texture<Spectrum_d> > p_reflectance( new TextureMock<Spectrum_d>(Spectrum_d(1.0,0.5,0.1)) );
      intrusive_ptr<TextureMock<double> > p_sigma( new TextureMock<double>(0.0) );
      
      intrusive_ptr<Material> p_material(new Matte(p_reflectance, p_sigma));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 1);
      Spectrum_d val = p_bsdf->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));
      TS_ASSERT_DELTA(val[0], 1.0*INV_PI, (1e-6));
      TS_ASSERT_DELTA(val[1], 0.5*INV_PI, (1e-6));
      TS_ASSERT_DELTA(val[2], 0.1*INV_PI, (1e-6));
      }

    // Tests for the case when sigma is not zero, the returned value should match OrenNayar BxDF.
    void test_Matte_EvaluateNotZeroSigma()
      {
      intrusive_ptr<Texture<Spectrum_d> > p_reflectance( new TextureMock<Spectrum_d>(Spectrum_d(1.0,0.5,0.1)) );
      intrusive_ptr<TextureMock<double> > p_sigma( new TextureMock<double>(0.1) );

      intrusive_ptr<Material> p_material(new Matte(p_reflectance, p_sigma));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 1);

      Spectrum_d val = p_bsdf->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));
      Spectrum_d val2 = OrenNayar(Spectrum_d(1.0,0.5,0.1), 0.1).Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));

      TS_ASSERT_DELTA(val[0], val2[0], (1e-6));
      TS_ASSERT_DELTA(val[1], val2[1], (1e-6));
      TS_ASSERT_DELTA(val[2], val2[2], (1e-6));
      }
  };

#endif // MATTE_TEST_H
