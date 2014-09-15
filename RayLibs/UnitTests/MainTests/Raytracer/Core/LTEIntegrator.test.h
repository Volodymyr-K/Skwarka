#ifndef LTE_INTEGRATOR_TEST_H
#define LTE_INTEGRATOR_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Math/RandomGenerator.h>
#include "Mocks/LTEIntegratorMock.h"
#include "Mocks/MaterialMock.h"
#include "Mocks/InfiniteLightSourceMock.h"
#include "Mocks/VolumeRegionMock.h"
#include <Raytracer/LightSources/PointLight.h>
#include <UnitTests/TestHelpers/TriangleMeshTestHelper.h>

class LTEIntegratorTestSuite : public CxxTest::TestSuite
  {
  public:

    // Primitives creation separated to the constructor (instead of setUp() method) to avoid performance overhead.
    LTEIntegratorTestSuite()
      {
      intrusive_ptr<TriangleMesh> p_sphere( TriangleMeshHelper::ConstructSphere(Point3D_d(0,0,0),1.0, 6) );
      intrusive_ptr<Material> p_material(new MaterialMock());
      intrusive_ptr<Primitive> p_sphere_primitive(new Primitive(p_sphere, Transform(), p_material));

      m_primitives.push_back(p_sphere_primitive);

      m_ts.mp_pool = &m_pool;
      m_ts.mp_random_generator = &m_rng;
      }

    void setUp()
      {
      ////// Lights ////////
      LightSources lights;
      intrusive_ptr<DeltaLightSource> p_delta_light( new PointLight(Point3D_d(0, 0.0, 0.0), Spectrum_d(100.0)) );
      intrusive_ptr<InfiniteLightSource> p_infinite_light( new InfiniteLightSourceMock(Spectrum_d(10.0), BBox3D_d(Point3D_d(-1,-1,-1),Point3D_d(1,1,1))) );
      lights.m_delta_light_sources.push_back(p_delta_light);
      lights.m_infinite_light_sources.push_back(p_infinite_light);
      //////////////////////

      intrusive_ptr<const VolumeRegion> p_volume( new VolumeRegionMock(BBox3D_d(Point3D_d(-1,-1,-1),Point3D_d(1,1,1)), Spectrum_d(0), SpectrumCoef_d(0.5), SpectrumCoef_d(0.5)) );

      mp_scene_no_media.reset( new Scene(m_primitives, NULL, lights) );
      mp_scene_with_media.reset( new Scene(m_primitives, p_volume, lights) );
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_LTEIntegrator_RadianceInsideSphere_NoMedia()
      {
      intrusive_ptr<LTEIntegrator> p_lte_int( new LTEIntegratorMock(mp_scene_no_media) );

      Spectrum_d radiance = p_lte_int->Radiance(RayDifferential(Ray(Point3D_d(0,0,0),Vector3D_d(1,0,0))), NULL, m_ts);

      // Since the mesh is a unit radius sphere with a point light in its center we can compute the radiance analytically.
      Spectrum_d analytical_radiance = Spectrum_d(100.0)*INV_PI;
      CustomAssertDelta(radiance, analytical_radiance, 0.05);
      }

    void test_LTEIntegrator_RadianceOutsideSphere_NoMedia()
      {
      intrusive_ptr<LTEIntegrator> p_lte_int( new LTEIntegratorMock(mp_scene_no_media) );

      Spectrum_d radiance = p_lte_int->Radiance(RayDifferential(Ray(Point3D_d(0,0,-20),Vector3D_d(0,0,1))), NULL, m_ts);

      // Since the sphere is lighted by the infinity light mock we can compute the radiance analytically.
      Spectrum_d analytical_radiance = (Spectrum_d(10.0)*M_PI) * INV_PI;
      CustomAssertDelta(radiance, analytical_radiance, 0.001);
      }

    void test_LTEIntegrator_RadianceInsideSphere_WithMedia()
      {
      intrusive_ptr<LTEIntegrator> p_lte_int( new LTEIntegratorMock(mp_scene_with_media) );

      RayDifferential ray(Ray(Point3D_d(0,0,0),Vector3D_d(1,0,0)));
      Spectrum_d radiance = p_lte_int->Radiance(ray, NULL, m_ts);

      // Since the mesh is a unit radius sphere with a point light in its center we can compute the radiance analytically.
      Spectrum_d analytical_radiance = Spectrum_d(100.0)*INV_PI * exp(-1.0) * exp(-1.0); // exp is multiplied twice to account for light attenuation and camera ray attenuation.
      CustomAssertDelta(radiance, analytical_radiance, 0.02);
      }

  private:
    std::vector<intrusive_ptr<const Primitive> > m_primitives;

    intrusive_ptr<Scene> mp_scene_no_media, mp_scene_with_media;

    MemoryPool m_pool;
    RandomGenerator<double> m_rng;
    ThreadSpecifics m_ts;
  };

#endif // LTE_INTEGRATOR_TEST_H