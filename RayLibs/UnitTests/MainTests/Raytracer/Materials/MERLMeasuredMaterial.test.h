#ifndef MERL_MEASURED_MATERIAL_TEST_H
#define MERL_MEASURED_MATERIAL_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/MERLMeasuredMaterial.h>
#include <Raytracer/BxDFs/MERLMeasured.h>
#include <fstream>

class MERLMeasuredMaterialTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_MERLMeasuredMaterial()
      {
      intrusive_ptr<MERLMeasuredData> p_tungsten_carbide_data = _LoadMERLDataFromFile("TestData/MERLMeasured/tungsten-carbide.binary");
      intrusive_ptr<Material> p_material(new MERLMeasuredMaterial(p_tungsten_carbide_data));

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      const BSDF *p_bsdf = p_material->GetBSDF(dg, 0, pool);

      TS_ASSERT(p_bsdf != NULL);
      TS_ASSERT_EQUALS(p_bsdf->GetComponentsNum(), 1);

      SpectrumCoef_d val = p_bsdf->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      MERLMeasured bxdf(p_tungsten_carbide_data.get());
      SpectrumCoef_d val2 = bxdf.Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());

      CustomAssertDelta(val, val2, (1e-6));
      }

  private:
    intrusive_ptr<MERLMeasuredData> _LoadMERLDataFromFile(std::string i_filename) const
      {
      std::ifstream file(i_filename.c_str(), std::ios::in | std::ios::binary);
      return intrusive_ptr<MERLMeasuredData>( new MERLMeasuredData(file) );
      }

  };

#endif // MERL_MEASURED_MATERIAL_TEST_H
