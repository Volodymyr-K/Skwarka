#ifndef METAL_MATERIAL_TEST_H
#define METAL_MATERIAL_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/MetalMaterial.h>
#include <Raytracer/BxDFs/Microfacet.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/MicrofacetDistributions/AnisotropicDistribution.h>
#include <Raytracer/Core/Fresnel.h>
#include "Mocks/TextureMock.h"

class MetalMaterialTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_MetalMaterial_Isotropic()
      {
      SpectrumCoef_d refreactive_index(0.41,1.15,1.18), absorption(4.2,2.66,2.5);
      intrusive_ptr<Texture<SpectrumCoef_d>> p_refreactive_index( new TextureMock<SpectrumCoef_d>(refreactive_index) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_absorption( new TextureMock<SpectrumCoef_d>(absorption) );
      intrusive_ptr<Texture<double>> p_roughness( new TextureMock<double>(1.0) );

      intrusive_ptr<Material> p_material(new MetalMaterial(p_refreactive_index, p_absorption, p_roughness));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      const BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 1);

      SpectrumCoef_d val = p_bsdf->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      typedef Microfacet<FresnelConductor,BlinnDistribution> BlinnMicrofacetMetal;
      BlinnMicrofacetMetal metal(SpectrumCoef_d(1.0), FresnelConductor(refreactive_index, absorption), BlinnDistribution(1.0));
      SpectrumCoef_d val2 = metal.Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      CustomAssertDelta(val, val2, (1e-6));
      }

    void test_MetalMaterial_Anisotropic()
      {
      SpectrumCoef_d refreactive_index(0.41,1.15,1.18), absorption(4.2,2.66,2.5);
      intrusive_ptr<Texture<SpectrumCoef_d>> p_refreactive_index( new TextureMock<SpectrumCoef_d>(refreactive_index) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_absorption( new TextureMock<SpectrumCoef_d>(absorption) );
      intrusive_ptr<Texture<double>> p_u_roughness( new TextureMock<double>(1.0) );
      intrusive_ptr<Texture<double>> p_v_roughness( new TextureMock<double>(10.0) );

      intrusive_ptr<Material> p_material(new MetalMaterial(p_refreactive_index, p_absorption, p_u_roughness, p_v_roughness));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      const BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 1);

      SpectrumCoef_d val = p_bsdf->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      typedef Microfacet<FresnelConductor,AnisotropicDistribution> AnisotropicMicrofacetMetal;
      AnisotropicMicrofacetMetal metal(SpectrumCoef_d(1.0), FresnelConductor(refreactive_index, absorption), AnisotropicDistribution(1.0,10.0));
      SpectrumCoef_d val2 = metal.Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      CustomAssertDelta(val, val2, (1e-6));
      }

  };

#endif // METAL_MATERIAL_TEST_H
