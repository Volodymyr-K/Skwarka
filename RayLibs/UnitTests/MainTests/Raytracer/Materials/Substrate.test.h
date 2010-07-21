#ifndef SUBSTRATE_TEST_H
#define SUBSTRATE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/Substrate.h>
#include <Raytracer/BxDFs/FresnelBlend.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/Core/Fresnel.h>
#include "Mocks/TextureMock.h"

class SubstrateTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_Substrate()
      {
      Spectrum_d diffuse_reflectance(0.4,0.5,0.6), specular_reflectance(0.5);
      intrusive_ptr<Texture<Spectrum_d> > p_diffuse_reflectance( new TextureMock<Spectrum_d>(diffuse_reflectance) );
      intrusive_ptr<Texture<Spectrum_d> > p_specular_reflectance( new TextureMock<Spectrum_d>(specular_reflectance) );
      intrusive_ptr<Texture<double> > p_roughness( new TextureMock<double>(0.025) );

      intrusive_ptr<Material> p_material(new Substrate(p_diffuse_reflectance, p_specular_reflectance, p_roughness));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      const BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 1);

      Spectrum_d val = p_bsdf->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      typedef FresnelBlend<BlinnDistribution> BlinnFresnelBlend;
      BlinnFresnelBlend fresnel_blend(diffuse_reflectance, specular_reflectance, BlinnDistribution(1.0/0.025));
      Spectrum_d val2 = fresnel_blend.Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      CustomAssertDelta(val, val2, (1e-6));
      }

  };

#endif // SUBSTRATE_TEST_H
