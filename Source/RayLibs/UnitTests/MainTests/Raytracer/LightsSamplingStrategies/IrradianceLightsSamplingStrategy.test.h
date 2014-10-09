#ifndef IRRADIANCE_LIGHTS_SAMPLING_TEST_H
#define IRRADIANCE_LIGHTS_SAMPLING_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Raytracer/LightsSamplingStrategies/IrradianceLightsSamplingStrategy.h>
#include <UnitTests/TestHelpers/TriangleMeshTestHelper.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Raytracer/LightSources/PointLight.h>
#include "Mocks/InfiniteLightSourceMock.h"

class IrradianceLightsSamplingStrategyTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      m_light_sources.m_delta_light_sources.clear();
      m_light_sources.m_infinite_light_sources.clear();
      m_light_sources.m_area_light_sources.clear();

      intrusive_ptr<DeltaLightSource> p_delta_light( new PointLight(Point3D_d(1.0,2.0,3.0), Spectrum_d(10.5,20.5,30.5)) );
      m_light_sources.m_delta_light_sources.push_back(p_delta_light);

      intrusive_ptr<InfiniteLightSource> p_infinity_light( new InfiniteLightSourceMock(Spectrum_d(0.001,0.005,0.002), BBox3D_d(Point3D_d(-1,-1,-1),Point3D_d(50,50,50))));
      m_light_sources.m_infinite_light_sources.push_back(p_infinity_light);

      for(size_t i=0;i<5;++i)
        {
        intrusive_ptr<TriangleMesh> p_mesh( TriangleMeshHelper::ConstructTetrahedron(Point3D_f(10.f*i,0.f,0.f)) );
        intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(Spectrum_d(1.0), p_mesh) );
        m_light_sources.m_area_light_sources.push_back(p_area_light);
        }

      mp_light_sampling.reset( new IrradianceLightsSamplingStrategy(m_light_sources) );
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_IrradianceLightsSamplingStrategy_WithoutNormal()
      {
      double cdf[100]; // 100 should be enough.

      size_t infinity_lights_num = m_light_sources.m_infinite_light_sources.size();
      size_t area_lights_num = m_light_sources.m_area_light_sources.size();
      size_t lights_num = infinity_lights_num+area_lights_num;

      mp_light_sampling->GetLightsCDF(Point3D_d(0,0,0), cdf);

      for(size_t i=0;i<lights_num;++i)
        {
        if (cdf[i]<0.0) {TS_FAIL("CDF value is less than 0."); break;}
        if (cdf[i]>1.0) {TS_FAIL("CDF value is greater than 1."); break;}
        if (i>0 && cdf[i]<cdf[i-1]) {TS_FAIL("CDF values aren't ordered non-descendingly."); break;}
        }

      // Check that PDFs for area lights are ordered decreasingly.
      // This is so because each next area light is located at a bigger distance from the point than the previous one (look at setUp() method).
      for(size_t i=1;i<area_lights_num;++i)
        {
        size_t j=i+infinity_lights_num;
        double pdf_prev = cdf[j-1]-cdf[j-2];
        double pdf = cdf[j]-cdf[j-1];
        if (pdf_prev <= pdf) {TS_FAIL("CDF values for area lights are incorrect."); break;}
        }

      TS_ASSERT_EQUALS(cdf[lights_num-1], 1.0);
      }

    // Tests a case when area lights are in the correct hemisphere.
    void test_IrradianceLightsSamplingStrategy_WithNormal1()
      {
      double cdf[100]; // 100 should be enough.

      size_t infinity_lights_num = m_light_sources.m_infinite_light_sources.size();
      size_t area_lights_num = m_light_sources.m_area_light_sources.size();
      size_t lights_num = infinity_lights_num+area_lights_num;

      mp_light_sampling->GetLightsCDF(Point3D_d(0,0,0), Vector3D_d(1,0,0), cdf);

      for(size_t i=0;i<lights_num;++i)
        {
        if (cdf[i]<0.0) {TS_FAIL("CDF value is less than 0."); break;}
        if (cdf[i]>1.0) {TS_FAIL("CDF value is greater than 1."); break;}
        if (i>0 && cdf[i]<cdf[i-1]) {TS_FAIL("CDF values aren't ordered non-descendingly."); break;}
        }

      // Check that PDFs for area lights are ordered decreasingly.
      // This is so because each next area light is located at a bigger distance from the point than the previous one (look at setUp() method).
      for(size_t i=1;i<area_lights_num;++i)
        {
        size_t j=i+infinity_lights_num;
        double pdf_prev = cdf[j-1]-cdf[j-2];
        double pdf = cdf[j]-cdf[j-1];
        if (pdf_prev <= pdf) {TS_FAIL("CDF values for area lights are incorrect."); break;}
        }

      TS_ASSERT_EQUALS(cdf[lights_num-1], 1.0);
      }

    // Tests a case when area lights are in the opposite hemisphere, they should have zero PDF in this case.
    void test_IrradianceLightsSamplingStrategy_WithNormal2()
      {
      double cdf[100]; // 100 should be enough.

      size_t infinity_lights_num = m_light_sources.m_infinite_light_sources.size();
      size_t area_lights_num = m_light_sources.m_area_light_sources.size();
      size_t lights_num = infinity_lights_num+area_lights_num;

      mp_light_sampling->GetLightsCDF(Point3D_d(0,0,0), Vector3D_d(-1,0,0), cdf);

      TS_ASSERT_EQUALS(cdf[infinity_lights_num],1.0);
      }

    void test_IrradianceLightsSamplingStrategy_NoLights()
      {
      m_light_sources.m_delta_light_sources.clear();
      m_light_sources.m_infinite_light_sources.clear();
      m_light_sources.m_area_light_sources.clear();
      mp_light_sampling.reset( new IrradianceLightsSamplingStrategy(m_light_sources) );

      double cdf[100]; // 100 should be enough.
      cdf[0]=-1.0;
      mp_light_sampling->GetLightsCDF(Point3D_d(0,0,0), cdf);

      // If there are no lights in the scene the cdf array should not be changed.
      TS_ASSERT_EQUALS(cdf[0], -1.0);
      }

  private:
    LightSources m_light_sources;
    intrusive_ptr<LightsSamplingStrategy> mp_light_sampling;
  };

#endif // IRRADIANCE_LIGHTS_SAMPLING_TEST_H