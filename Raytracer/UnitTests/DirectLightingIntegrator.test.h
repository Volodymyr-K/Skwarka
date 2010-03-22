#ifndef DIRECT_LIGHTING_INTEGRATOR_TEST_H
#define DIRECT_LIGHTING_INTEGRATOR_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/DirectLightingIntegrator.h>
#include <Raytracer/Core/Primitive.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Raytracer/Samplers/StratifiedSampler.h>
#include "Mocks/MaterialMock.h"
#include "Mocks/InfiniteLightSourceMock.h"
#include <Math/SamplingRoutines.h>
#include "TriangleMeshTestHelper.h"

class DirectLightingIntegratorTestSuite : public CxxTest::TestSuite
  {
  public:

    // Spheres creation separated to the constructor (instead of setUp() method) to avoid performance overhead.
    DirectLightingIntegratorTestSuite()
      {
      m_spheres[0] = TriangleMeshHelper::ConstructSphere(Point3D_d(0,0,0),1.0, 6);
      m_spheres[1] = TriangleMeshHelper::ConstructSphere(Point3D_d(10,0,0),1.0, 6);
      m_spheres[2] = TriangleMeshHelper::ConstructSphere(Point3D_d(0,10,0),1.0, 6);
      m_world_bbox = BBox3D_d(Point3D_d(-1,-1,-1), Point3D_d(11,11,1));
      }

    void test_DirectLightingIntegrator_PointLight()
      {
      intrusive_ptr<TriangleMesh> p_mesh = m_spheres[0];
      intrusive_ptr<Primitive> p_primitive = _CreatePrimitive(p_mesh);
      std::vector<intrusive_ptr<const Primitive> > primitives(1,p_primitive);

      LightSources lights;
      lights.m_delta_light_sources.push_back( intrusive_ptr<DeltaLightSource>(new PointLight(Point3D_d(10,0,0),Spectrum_d(100))) );

      intrusive_ptr<Scene> p_scene( new Scene(primitives, lights) );
      intrusive_ptr<Sampler> p_sampler = _CreaterSampler();
     
      MemoryPool pool;
      Ray ray(Point3D_d(5,5,0), Vector3D_d(1-5,0-5,0-0).Normalized());
      Intersection isect;
      p_scene->Intersect(RayDifferential(ray), isect);
      const BSDF *p_bsdf = isect.mp_primitive->GetBSDF(isect.m_dg, isect.m_triangle_index, pool);

      intrusive_ptr<DirectLightingIntegrator> p_integrator( new DirectLightingIntegrator(p_scene, NULL, 1, 1) );
      p_integrator->RequestSamples(p_sampler);
      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();
      p_sampler->GetNextSubSampler(1, &m_rng)->GetNextSample(p_sample);
      Spectrum_d radiance = p_integrator->ComputeDirectLighting(isect, ray.m_direction*(-1.0), p_bsdf, p_sample.get(), pool);

      // We use mock BxDF which is a Lambertian one so the reflectance is Spectrum_d(1.0)/M_PI.
      CustomAssertDelta(radiance, Spectrum_d(100)/(9*9) * Spectrum_d(1.0)/M_PI, 1e-4);
      }

    // There are infinity lights in the scene.
    void test_DirectLightingIntegrator_InfinityLights()
      {
      intrusive_ptr<TriangleMesh> p_mesh = m_spheres[0];
      intrusive_ptr<Primitive> p_primitive = _CreatePrimitive(p_mesh);
      std::vector<intrusive_ptr<const Primitive> > primitives(1,p_primitive);

      LightSources lights;
      lights.m_infinite_light_sources.push_back(intrusive_ptr<InfiniteLightSource>(new InfiniteLightSourceMock(Spectrum_d(1), m_world_bbox)) );
      lights.m_infinite_light_sources.push_back(intrusive_ptr<InfiniteLightSource>(new InfiniteLightSourceMock(Spectrum_d(2), m_world_bbox)) );

      intrusive_ptr<Scene> p_scene( new Scene(primitives, lights) );
      intrusive_ptr<Sampler> p_sampler = _CreaterSampler();

      MemoryPool pool;
      Ray ray(Point3D_d(5,5,0), Vector3D_d(1-5,0-5,0-0).Normalized());
      Intersection isect;
      p_scene->Intersect(RayDifferential(ray), isect);
      const BSDF *p_bsdf = isect.mp_primitive->GetBSDF(isect.m_dg, isect.m_triangle_index, pool);

      intrusive_ptr<DirectLightingIntegrator> p_integrator( new DirectLightingIntegrator(p_scene, NULL, 5000, 5100) );
      p_integrator->RequestSamples(p_sampler);
      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();
      p_sampler->GetNextSubSampler(1, &m_rng)->GetNextSample(p_sample);     
      Spectrum_d radiance = p_integrator->ComputeDirectLighting(isect, ray.m_direction*(-1.0), p_bsdf, p_sample.get(), pool);

      // We use mock BxDF which is a Lambertian one so the reflectance is Spectrum_d(1.0)/M_PI.
      // The analytical solution to the direct lighting integral is exactly the sum of infinity lights spectrums.
      CustomAssertDelta(radiance, Spectrum_d(1) + Spectrum_d(2), 1e-2);
      }

    // There are two area lights in the scene.
    void test_DirectLightingIntegrator_AreaLights()
      {
      std::vector<intrusive_ptr<const Primitive> > primitives;
      primitives.push_back(_CreatePrimitive(m_spheres[0]));
      primitives.push_back( _CreatePrimitive(m_spheres[1], _CreateAreaLight(m_spheres[1], Spectrum_d(10))) );
      primitives.push_back( _CreatePrimitive(m_spheres[2], _CreateAreaLight(m_spheres[2], Spectrum_d(20))) );

      LightSources lights;
      lights.m_area_light_sources.push_back(primitives[1]->GetAreaLightSource());
      lights.m_area_light_sources.push_back(primitives[2]->GetAreaLightSource());

      intrusive_ptr<Scene> p_scene( new Scene(primitives, lights) );
      intrusive_ptr<Sampler> p_sampler = _CreaterSampler();

      MemoryPool pool;
      Ray ray(Point3D_d(5,5,0), Vector3D_d(1-5,1-5,0-0).Normalized());
      Intersection isect;
      p_scene->Intersect(RayDifferential(ray), isect);
      const BSDF *p_bsdf = isect.mp_primitive->GetBSDF(isect.m_dg, isect.m_triangle_index, pool);

      intrusive_ptr<DirectLightingIntegrator> p_integrator( new DirectLightingIntegrator(p_scene, NULL, 5000, 5100) );
      p_integrator->RequestSamples(p_sampler);
      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();
      p_sampler->GetNextSubSampler(1, &m_rng)->GetNextSample(p_sample);     
      Spectrum_d radiance = p_integrator->ComputeDirectLighting(isect, ray.m_direction*(-1.0), p_bsdf, p_sample.get(), pool);

      // Compute the estimate numerically.
      // We use mock BxDF which is a Lambertian one so the reflectance is Spectrum_d(1.0)/M_PI.
      double cos_integrated = _ComputeCosineIntegral(isect.m_dg, Point3D_d(0,10,0), 1);
      Spectrum_d area_light_estimate_1 = Spectrum_d(10) * cos_integrated * INV_PI;
      Spectrum_d area_light_estimate_2 = Spectrum_d(20) * cos_integrated * INV_PI; // Here we reuse the same cos_integrated because they are equal for both spheres.
      CustomAssertDelta(radiance, area_light_estimate_1+area_light_estimate_2, 1e-2);
      }

    // There are two area lights and two infinity lights in the scene. Also, the integrator is called without Sample instance.
    void test_DirectLightingIntegrator_InfinityAndAreaLights()
      {
      std::vector<intrusive_ptr<const Primitive> > primitives;
      primitives.push_back(_CreatePrimitive(m_spheres[0]));
      primitives.push_back( _CreatePrimitive(m_spheres[1], _CreateAreaLight(m_spheres[1], Spectrum_d(10))) );
      primitives.push_back( _CreatePrimitive(m_spheres[2], _CreateAreaLight(m_spheres[2], Spectrum_d(20))) );

      LightSources lights;
      lights.m_infinite_light_sources.push_back(intrusive_ptr<InfiniteLightSource>(new InfiniteLightSourceMock(Spectrum_d(5), m_world_bbox)) );
      lights.m_infinite_light_sources.push_back(intrusive_ptr<InfiniteLightSource>(new InfiniteLightSourceMock(Spectrum_d(15), m_world_bbox)) );
      lights.m_area_light_sources.push_back(primitives[1]->GetAreaLightSource());
      lights.m_area_light_sources.push_back(primitives[2]->GetAreaLightSource());

      intrusive_ptr<Scene> p_scene( new Scene(primitives, lights) );
      intrusive_ptr<Sampler> p_sampler = _CreaterSampler();

      MemoryPool pool;
      Ray ray(Point3D_d(5,5,0), Vector3D_d(1-5,1-5,0-0).Normalized());
      Intersection isect;
      p_scene->Intersect(RayDifferential(ray), isect);
      const BSDF *p_bsdf = isect.mp_primitive->GetBSDF(isect.m_dg, isect.m_triangle_index, pool);

      intrusive_ptr<DirectLightingIntegrator> p_integrator( new DirectLightingIntegrator(p_scene, NULL, 10000, 11000) );  
      Spectrum_d radiance = p_integrator->ComputeDirectLighting(isect, ray.m_direction*(-1.0), p_bsdf, NULL, pool); // Call without Sample.

      // Compute the estimate numerically.
      // We use mock BxDF which is a Lambertian one so the reflectance is Spectrum_d(1.0)/M_PI.
      double cos_integrated = _ComputeCosineIntegral(isect.m_dg, Point3D_d(0,10,0), 1);
      Spectrum_d area_light_estimate_1 = Spectrum_d(10) * cos_integrated * INV_PI;
      Spectrum_d area_light_estimate_2 = Spectrum_d(20) * cos_integrated * INV_PI; // Here we reuse the same cos_integrated because they are equal for both spheres.
      Spectrum_d infinity_light_estimate_1 = Spectrum_d(5)/M_PI * (M_PI - 2.0*cos_integrated); // We subtract 2.0*cos_integrated to account for the infinity light hidden by the spheres.
      Spectrum_d infinity_light_estimate_2 = Spectrum_d(15)/M_PI * (M_PI - 2.0*cos_integrated);
      CustomAssertDelta(radiance, area_light_estimate_1+area_light_estimate_2 + infinity_light_estimate_1+infinity_light_estimate_2, 1e-1);
      }

    // There are two area lights and two infinity lights in the scene but the intersection is inside the sphere so there's no lighting.
    void test_DirectLightingIntegrator_NoLighting()
      {
      std::vector<intrusive_ptr<const Primitive> > primitives;
      primitives.push_back(_CreatePrimitive(m_spheres[0]));
      primitives.push_back( _CreatePrimitive(m_spheres[1], _CreateAreaLight(m_spheres[1], Spectrum_d(10))) );
      primitives.push_back( _CreatePrimitive(m_spheres[2], _CreateAreaLight(m_spheres[2], Spectrum_d(20))) );

      LightSources lights;
      lights.m_infinite_light_sources.push_back(intrusive_ptr<InfiniteLightSource>(new InfiniteLightSourceMock(Spectrum_d(5), m_world_bbox)) );
      lights.m_infinite_light_sources.push_back(intrusive_ptr<InfiniteLightSource>(new InfiniteLightSourceMock(Spectrum_d(15), m_world_bbox)) );
      lights.m_area_light_sources.push_back(primitives[1]->GetAreaLightSource());
      lights.m_area_light_sources.push_back(primitives[2]->GetAreaLightSource());

      intrusive_ptr<Scene> p_scene( new Scene(primitives, lights) );
      intrusive_ptr<Sampler> p_sampler = _CreaterSampler();

      MemoryPool pool;
      Ray ray(Point3D_d(0,0,0), Vector3D_d(1,1,1).Normalized());
      Intersection isect;
      p_scene->Intersect(RayDifferential(ray), isect);
      const BSDF *p_bsdf = isect.mp_primitive->GetBSDF(isect.m_dg, isect.m_triangle_index, pool);

      intrusive_ptr<DirectLightingIntegrator> p_integrator( new DirectLightingIntegrator(p_scene, NULL, 5000, 5100) );
      p_integrator->RequestSamples(p_sampler);
      intrusive_ptr<Sample> p_sample = p_sampler->CreateSample();
      p_sampler->GetNextSubSampler(1, &m_rng)->GetNextSample(p_sample);     
      Spectrum_d radiance = p_integrator->ComputeDirectLighting(isect, ray.m_direction*(-1.0), p_bsdf, p_sample.get(), pool);

      TS_ASSERT_EQUALS(radiance, Spectrum_d(0));
      }

  private:

    intrusive_ptr<AreaLightSource> _CreateAreaLight(intrusive_ptr<TriangleMesh> ip_mesh, const Spectrum_d &i_radiance) const
      {
      intrusive_ptr<AreaLightSource> p_light( new DiffuseAreaLightSource(i_radiance, ip_mesh) );
      return p_light;
      }

    intrusive_ptr<Primitive> _CreatePrimitive(intrusive_ptr<TriangleMesh> ip_mesh, intrusive_ptr<AreaLightSource> ip_light = NULL) const
      {
      intrusive_ptr<Material> p_material(new MaterialMock());
      intrusive_ptr<Primitive> p_primitive(new Primitive(ip_mesh, p_material, ip_light));
      return p_primitive;
      }

    intrusive_ptr<Sampler> _CreaterSampler()
      {
      return intrusive_ptr<Sampler> (new StratifiedSampler(Point2D_i(0,0), Point2D_i(1,1), 1, 1) );
      }
    
    // Computes integral of the theta angle cosine over the solid angle subtended by a sphere visible from a surface point.
    double _ComputeCosineIntegral(const DifferentialGeometry &i_dg, const Point3D_d &i_center, double i_radius)
      {
      double sum=0;
      size_t N=10000,hits=0;
      Vector3D_d to_center = Vector3D_d(i_center-i_dg.m_point);
      for(size_t i=0;i<N;++i)
        {
        Vector3D_d dir = SamplingRoutines::UniformSphereSampling(Point2D_d(RandomDouble(1.0),RandomDouble(1.0)));
        if (dir*i_dg.m_shading_normal<=0.0 || dir*to_center<=0.0) dir*=-1.0;
        double dist = (dir^to_center).Length();
        if (dist>=i_radius || dir*i_dg.m_shading_normal<=0.0 || dir*to_center<=0.0) continue;

        sum += dir*i_dg.m_shading_normal;
        ++hits;
        }

      return M_PI*i_radius*i_radius/to_center.LengthSqr() * sum/hits;
      }

  private:
    intrusive_ptr<Scene> mp_scene;
    intrusive_ptr<Sampler> mp_sampler;

    BBox3D_d m_world_bbox;
    intrusive_ptr<TriangleMesh> m_spheres[3];
    RandomGenerator<double> m_rng;
  };

#endif // DIRECT_LIGHTING_INTEGRATOR_TEST_H
