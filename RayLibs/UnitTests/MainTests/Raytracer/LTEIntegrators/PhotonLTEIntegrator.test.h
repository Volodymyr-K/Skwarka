#ifndef PHOTON_LTE_INTEGRATOR_TEST_H
#define PHOTON_LTE_INTEGRATOR_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/LTEIntegrators/PhotonLTEIntegrator.h>
#include <Raytracer/Core/Primitive.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Raytracer/Samplers/StratifiedSampler.h>
#include <Raytracer/Materials/MatteMaterial.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include "Mocks/InfiniteLightSourceMock.h"
#include <UnitTests/TestHelpers/TriangleMeshTestHelper.h>

class PhotonLTEIntegratorTestSuite : public CxxTest::TestSuite
  {
  public:

    // Sphere creation separated to the constructor (instead of setUp() method) to avoid performance overhead.
    PhotonLTEIntegratorTestSuite()
      {
      mp_sphere = TriangleMeshHelper::ConstructSphere(Point3D_d(0,0,0), 1.0, 7);
      m_ts.mp_pool = &m_pool;
      m_ts.mp_random_generator = &m_rng;
      }

    // The case with a camera placed inside of a self-illuminated sphere with lambertian BSDF.
    // The analytical solution for the radiance is known. The radiance is constant everywhere and is equal to "self_illumination / (1.0 - reflectance)".
    void test_PhotonLTEIntegrator_SelfIlluminatedSphere()
      {
      mp_sphere->SetInvertNormals(true);

      Spectrum_d light_radiance(100,90,80);
      SpectrumCoef_d reflectance(0.7,0.8,0.9);
      intrusive_ptr<AreaLightSource> p_light( new DiffuseAreaLightSource(light_radiance, mp_sphere) );
      intrusive_ptr<Primitive> p_primitive = _CreatePrimitive(mp_sphere, reflectance, p_light);
      std::vector<intrusive_ptr<const Primitive> > primitives(1, p_primitive);

      LightSources lights;
      lights.m_area_light_sources.push_back(p_light);

      intrusive_ptr<Scene> p_scene( new Scene(primitives, NULL, lights) );
      intrusive_ptr<Sampler> p_sampler = _CreaterSampler();

      PhotonLTEIntegratorParams params;
      params.m_direct_light_samples_num=4096;
      params.m_gather_samples_num=1024*16;
      params.m_caustic_lookup_photons_num=100; // no need to set caustic-related fields actually
      params.m_max_caustic_lookup_dist=0.01;
      params.m_media_step_size=0.01;
      params.m_max_specular_depth=6; // no need since there's no specular objects actually
      intrusive_ptr<PhotonLTEIntegrator> p_photon_lte_integrator( new PhotonLTEIntegrator(p_scene, params) );
      p_photon_lte_integrator->ShootPhotons(0,20000,100000);

      p_photon_lte_integrator->RequestSamples(p_sampler);

      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();
      p_sampler->GetNextSubSampler(1, &m_rng)->GetNextSample(p_sample);

      Ray ray(Point3D_d(0,0,0), Vector3D_d(1,0,0).Normalized());

      Spectrum_d radiance = p_photon_lte_integrator->Radiance(RayDifferential(ray), p_sample.get(), m_ts);
      TS_ASSERT_DELTA(radiance[0], light_radiance[0]/(1.0-reflectance[0]), 0.02*radiance[0]);
      TS_ASSERT_DELTA(radiance[1], light_radiance[1]/(1.0-reflectance[1]), 0.02*radiance[1]);
      TS_ASSERT_DELTA(radiance[2], light_radiance[2]/(1.0-reflectance[2]), 0.02*radiance[2]);
      }

    // The case with a camera placed inside of a sphere with lambertian BSDF.
    // Point light is placed in center of the sphere.
    // The analytical solution for the radiance is known. The radiance is constant everywhere and is equal to "direct_illumination / (1.0 - reflectance)".
    void test_PhotonLTEIntegrator_PointLightInSphere()
      {
      Spectrum_d light_intentsity(100,90,80);
      SpectrumCoef_d reflectance(0.7,0.8,0.9);
      intrusive_ptr<Primitive> p_primitive = _CreatePrimitive(mp_sphere, reflectance, NULL);
      std::vector<intrusive_ptr<const Primitive> > primitives(1, p_primitive);

      LightSources lights;
      intrusive_ptr<DeltaLightSource> p_light( new PointLight(Point3D_d(0,0,0), light_intentsity) );
      lights.m_delta_light_sources.push_back(p_light);

      intrusive_ptr<Scene> p_scene( new Scene(primitives, NULL, lights) );
      intrusive_ptr<Sampler> p_sampler = _CreaterSampler();

      PhotonLTEIntegratorParams params;
      params.m_direct_light_samples_num=4096;
      params.m_gather_samples_num=1024*16;
      params.m_caustic_lookup_photons_num=100; // no need to set caustic-related fields actually
      params.m_max_caustic_lookup_dist=0.01;
      params.m_media_step_size=0.01;
      params.m_max_specular_depth=6; // no need since there's no specular objects actually
      intrusive_ptr<PhotonLTEIntegrator> p_photon_lte_integrator( new PhotonLTEIntegrator(p_scene, params) );
      p_photon_lte_integrator->ShootPhotons(0,20000,100000);

      p_photon_lte_integrator->RequestSamples(p_sampler);

      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();
      p_sampler->GetNextSubSampler(1, &m_rng)->GetNextSample(p_sample);

      Ray ray(Point3D_d(0,0,0), Vector3D_d(1,0,0).Normalized());

      Spectrum_d radiance = p_photon_lte_integrator->Radiance(RayDifferential(ray), p_sample.get(), m_ts);
      TS_ASSERT_DELTA(radiance[0], INV_PI*light_intentsity[0]*reflectance[0]/(1.0-reflectance[0]), 0.02*radiance[0]);
      TS_ASSERT_DELTA(radiance[1], INV_PI*light_intentsity[1]*reflectance[1]/(1.0-reflectance[1]), 0.02*radiance[1]);
      TS_ASSERT_DELTA(radiance[2], INV_PI*light_intentsity[2]*reflectance[2]/(1.0-reflectance[2]), 0.02*radiance[2]);
      }

    // The case with a camera placed outside of a sphere with lambertian BSDF.
    // Point light is placed in center of the sphere and uniform infinity light shines the sphere from the outside.
    // The analytical solution for the radiance is known. The radiance consists of only the direct part because the sphere is convex.
    void test_PhotonLTEIntegrator_InfinityLight()
      {
      Spectrum_d light_radiance(100,90,80);
      SpectrumCoef_d reflectance(0.7,0.8,0.9);
      intrusive_ptr<Primitive> p_primitive = _CreatePrimitive(mp_sphere, reflectance, NULL);
      std::vector<intrusive_ptr<const Primitive> > primitives(1, p_primitive);

      LightSources lights;
      intrusive_ptr<DeltaLightSource> p_light( new PointLight(Point3D_d(0,0,0), light_radiance) );
      lights.m_delta_light_sources.push_back(p_light);

      BBox3D_d bbox = Convert<double>(mp_sphere->GetBounds());
      lights.m_infinite_light_sources.push_back(intrusive_ptr<InfiniteLightSource>(new InfiniteLightSourceMock(light_radiance, bbox)) );

      intrusive_ptr<Scene> p_scene( new Scene(primitives, NULL, lights) );
      intrusive_ptr<Sampler> p_sampler = _CreaterSampler();

      PhotonLTEIntegratorParams params;
      params.m_direct_light_samples_num=4096;
      params.m_gather_samples_num=1024*16;
      params.m_caustic_lookup_photons_num=100; // no need to set caustic-related fields actually
      params.m_max_caustic_lookup_dist=0.01;
      params.m_media_step_size=0.01;
      params.m_max_specular_depth=6; // no need since there's no specular objects actually
      intrusive_ptr<PhotonLTEIntegrator> p_photon_lte_integrator( new PhotonLTEIntegrator(p_scene, params) );
      p_photon_lte_integrator->ShootPhotons(0,20000,20000);

      p_photon_lte_integrator->RequestSamples(p_sampler);

      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();
      p_sampler->GetNextSubSampler(1, &m_rng)->GetNextSample(p_sample);

      Ray ray(Point3D_d(2,0,0), Vector3D_d(-1,0,0).Normalized());

      Spectrum_d radiance = p_photon_lte_integrator->Radiance(RayDifferential(ray), p_sample.get(), m_ts);
      TS_ASSERT_DELTA(radiance[0], light_radiance[0]*reflectance[0], 0.01*radiance[0]);
      TS_ASSERT_DELTA(radiance[1], light_radiance[1]*reflectance[1], 0.01*radiance[1]);
      TS_ASSERT_DELTA(radiance[2], light_radiance[2]*reflectance[2], 0.01*radiance[2]);
      }

  private:
    intrusive_ptr<Primitive> _CreatePrimitive(intrusive_ptr<TriangleMesh> ip_mesh, SpectrumCoef_d i_reflectance, intrusive_ptr<AreaLightSource> ip_light = NULL) const
      {
      intrusive_ptr<Texture<SpectrumCoef_d> > p_reflectance( new ConstantTexture<SpectrumCoef_d>(i_reflectance) );
      intrusive_ptr<Texture<double> > p_sigma( new ConstantTexture<double>(0.0) );
      intrusive_ptr<Material> p_material(new MatteMaterial(p_reflectance, p_sigma));
      intrusive_ptr<Primitive> p_primitive(new Primitive(ip_mesh, p_material, ip_light));
      return p_primitive;
      }

    intrusive_ptr<Sampler> _CreaterSampler()
      {
      return intrusive_ptr<Sampler> (new StratifiedSampler(Point2D_i(0,0), Point2D_i(1,1), 1, 1) );
      }

  private:
    intrusive_ptr<TriangleMesh> mp_sphere;
    RandomGenerator<double> m_rng;
    MemoryPool m_pool;

    ThreadSpecifics m_ts;
  };

#endif // PHOTON_LTE_INTEGRATOR_TEST_H
