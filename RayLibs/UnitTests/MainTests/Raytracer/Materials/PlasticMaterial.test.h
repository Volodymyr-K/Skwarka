#ifndef PLASTIC_MATERIAL_TEST_H
#define PLASTIC_MATERIAL_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/PlasticMaterial.h>
#include <Raytracer/BxDFs/Microfacet.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/Core/Fresnel.h>
#include "Mocks/TextureMock.h"

class PlasticMaterialTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_PlasticMaterial()
      {
      SpectrumCoef_d diffuse_reflectance(0.4,0.5,0.6), glossy_reflectance(0.3);
      intrusive_ptr<Texture<SpectrumCoef_d>> p_diffuse_reflectance( new TextureMock<SpectrumCoef_d>(diffuse_reflectance) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_glossy_reflectance( new TextureMock<SpectrumCoef_d>(glossy_reflectance) );
      intrusive_ptr<Texture<double>> p_roughness( new TextureMock<double>(0.025) );

      intrusive_ptr<Material> p_material(new PlasticMaterial(p_diffuse_reflectance, p_glossy_reflectance, p_roughness));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      const BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 2);

      SpectrumCoef_d val = p_bsdf->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      SpectrumCoef_d val2 = diffuse_reflectance * INV_PI;

      BlinnDistribution blinn(1.0/0.025);
      typedef Microfacet<FresnelDielectric,BlinnDistribution> BlinnMicrofacetDielectric;
      BlinnMicrofacetDielectric glossy(glossy_reflectance, FresnelDielectric(1.5, 1.0), blinn);
      SpectrumCoef_d val3 = glossy.Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      CustomAssertDelta(val, val2+val3, (1e-10));
      }

  };

#endif // PLASTIC_MATERIAL_TEST_H
