#ifndef LTE_INTEGRATOR_TEST_H
#define LTE_INTEGRATOR_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/MemoryPool.h>
#include "Mocks/LTEIntegratorMock.h"
#include "Mocks/VolumeIntegratorMock.h"
#include "Mocks/MaterialMock.h"
#include "Mocks/InfiniteLightSourceMock.h"
#include <Raytracer/LightSources/PointLight.h>
#include "TriangleMeshTestHelper.h"

class LTEIntegratorTestSuite : public CxxTest::TestSuite
  {
  public:

    // Primitives creation separated to the constructor (instead of setUp() method) to avoid performance overhead.
    LTEIntegratorTestSuite()
      {
      intrusive_ptr<TriangleMesh> p_sphere( TriangleMeshHelper::ConstructSphere(Point3D_d(0,0,0),1.0, 6) );
      intrusive_ptr<Material> p_material(new MaterialMock());
      intrusive_ptr<Primitive> p_sphere_primitive(new Primitive(p_sphere, p_material));

      m_primitives.push_back(p_sphere_primitive);
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

      mp_scene.reset( new Scene(m_primitives, NULL, lights) );
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_LTEIntegratorTestSuite_RadianceInsideSphere_NoVolumeIntegrator()
      {
      intrusive_ptr<LTEIntegrator> p_lte_int( new LTEIntegratorMock(mp_scene, NULL) );

      MemoryPool pool;
      Spectrum_d radiance = p_lte_int->Radiance(RayDifferential(Ray(Point3D_d(0,0,0),Vector3D_d(1,0,0))), NULL, pool);

      // Since the mesh is a unit radius sphere with a point light in its center we can compute the radiance analytically.
      Spectrum_d analytical_radiance = Spectrum_d(100.0)*INV_PI;
      CustomAssertDelta(radiance, analytical_radiance, 0.05);
      }

    void test_LTEIntegrator_RadianceOutsideSphere_NoVolumeIntegrator()
      {
      intrusive_ptr<LTEIntegrator> p_lte_int( new LTEIntegratorMock(mp_scene, NULL) );

      MemoryPool pool;
      Spectrum_d radiance = p_lte_int->Radiance(RayDifferential(Ray(Point3D_d(0,0,-20),Vector3D_d(0,0,1))), NULL, pool);

      // Since the sphere is lighted by the infinity light mock we can compute the radiance analytically.
      Spectrum_d analytical_radiance = (Spectrum_d(10.0)*M_PI) * INV_PI;
      CustomAssertDelta(radiance, analytical_radiance, 0.001);
      }

    void test_LTEIntegrator_RadianceOutsideSphere_WithVolumeIntegrator()
      {
      intrusive_ptr<LTEIntegrator> p_lte_int( new LTEIntegratorMock(mp_scene, intrusive_ptr<VolumeIntegrator>( new VolumeIntegratorMock() )) );

      MemoryPool pool;
      Spectrum_d radiance = p_lte_int->Radiance(RayDifferential(Ray(Point3D_d(20,0,0),Vector3D_d(-1,0,0))), NULL, pool);

      // Since the participating media completely absorbs the infinite light the radiance is black.
      TS_ASSERT_EQUALS(radiance, Spectrum_d(0.0));
      }

    void test_LTEIntegrator_RadianceInsideSphere_WithVolumeIntegrators()
      {
      intrusive_ptr<LTEIntegrator> p_lte_int( new LTEIntegratorMock(mp_scene, intrusive_ptr<VolumeIntegrator>( new VolumeIntegratorMock() )) );

      MemoryPool pool;
      RayDifferential ray(Ray(Point3D_d(0,0,0),Vector3D_d(1,0,0)));
      Spectrum_d radiance = p_lte_int->Radiance(ray, NULL, pool);

      // Since the mesh is a unit radius sphere with a point light in its center we can compute the radiance analytically.
      Spectrum_d analytical_radiance = Spectrum_d(100.0)*INV_PI * exp(-1.0) * exp(-1.0); // exp is multiplied twice to account for light attenuation and camera ray attenuation.
      CustomAssertDelta(radiance, analytical_radiance, 0.02);
      }

  private:
    std::vector<intrusive_ptr<const Primitive> > m_primitives;

    intrusive_ptr<Scene> mp_scene;
  };

#endif // LTE_INTEGRATOR_TEST_H