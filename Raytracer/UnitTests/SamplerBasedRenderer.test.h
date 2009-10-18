#ifndef SAMPLER_BASED_RENDERER_TEST_H
#define SAMPLER_BASED_RENDERER_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/MemoryPool.h>
#include <Raytracer/Renderers/SamplerBasedRenderer.h>
#include "Mocks/SurfaceIntegratorMock.h"
#include "Mocks/VolumeIntegratorMock.h"
#include "Mocks/MaterialMock.h"
#include "Mocks/InfiniteLightSourceMock.h"
#include <Shapes/Sphere.h>
#include <Raytracer/Cameras/PerspectiveCamera.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Raytracer/Films/ImageFilm.h>
#include <Raytracer/FilmFilters/BoxFilter.h>
#include <Raytracer/Samplers/StratifiedSampler.h>

class SamplerBasedRendererTestSuite : public CxxTest::TestSuite
  {
  public:

    // Primitives creation separated to the constructor (instead of setUp() method) to avoid performance overhead.
    SamplerBasedRendererTestSuite()
      {
      Sphere s;
      s.SetParameter("Center","0 0.0 0.0");
      s.SetParameter("Radius","1.0");
      s.SetParameter("Subdivisions","6");
      intrusive_ptr<TriangleMesh> p_sphere( s.BuildMesh() );
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
      lights.m_infinitiy_light_sources.push_back(p_infinite_light);
      //////////////////////

      mp_scene.reset( new Scene(m_primitives, lights) );

      intrusive_ptr<FilmFilter> p_filter( new BoxFilter(0.5,0.5) );
      intrusive_ptr<Film> p_film( new ImageFilm(10, 10, p_filter) );
      mp_camera.reset( new PerspectiveCamera( MakeLookAt(Point3D_d(0.0,0.0,0.0),Vector3D_d(1.0,0,0),Vector3D_d(0,0,1)), p_film, 0.000, 1.0, 1.3) );

      Point2D_i window_begin,window_end;
      p_film->GetSamplingExtent(window_begin, window_end);
      intrusive_ptr<Sampler> p_sampler( new StratifiedSampler(window_begin, window_end, 2, 2, true) );

      mp_log.reset( new StreamLog() );
      mp_renderer.reset( new SamplerBasedRenderer(mp_scene, p_sampler, mp_log) );
      mp_surf_int.reset( new SurfaceIntegratorMock(mp_renderer) );
      mp_volume_int.reset( new VolumeIntegratorMock(mp_renderer) );
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_SamplerBasedRenderer_RadianceInsideSphere_SurfaceIntegratorOnly()
      {
      mp_renderer->SetSurfaceIntegrator(mp_surf_int);
      MemoryPool pool;
      Spectrum_d radiance = mp_renderer->Radiance(RayDifferential(Ray(Point3D_d(0,0,0),Vector3D_d(1,0,0))), NULL, pool);

      // Since the mesh is a unit radius sphere with a point light in its center we can compute the radiance analytically.
      Spectrum_d analytical_radiance = Spectrum_d(100.0)*INV_PI;
      CustomAssertDelta(radiance, analytical_radiance, 0.05);
      }

    void test_SamplerBasedRenderer_RadianceOutsideSphere_SurfaceIntegratorOnly()
      {
      mp_renderer->SetSurfaceIntegrator(mp_surf_int);
      MemoryPool pool;
      Spectrum_d radiance = mp_renderer->Radiance(RayDifferential(Ray(Point3D_d(20,0,0),Vector3D_d(-1,0,0))), NULL, pool);

      // Since the sphere is lighted by the infinity light mock we can compute the radiance analytically.
      Spectrum_d analytical_radiance = (Spectrum_d(10.0)*M_PI) * INV_PI;
      CustomAssertDelta(radiance, analytical_radiance, 0.001);
      }

    void test_SamplerBasedRenderer_RadianceOutsideSphere_SurfaceAndVolumeIntegrators()
      {
      mp_renderer->SetSurfaceIntegrator(mp_surf_int);
      mp_renderer->SetVolumeIntegrator(mp_volume_int);
      MemoryPool pool;
      Spectrum_d radiance = mp_renderer->Radiance(RayDifferential(Ray(Point3D_d(20,0,0),Vector3D_d(-1,0,0))), NULL, pool);

      // Since the participating media completely absorbs the infinite light the radiance is black.
      TS_ASSERT_EQUALS(radiance, Spectrum_d(0.0));
      }

    void test_SamplerBasedRenderer_RadianceInsideSphere_SurfaceAndVolumeIntegrators()
      {
      mp_renderer->SetSurfaceIntegrator(mp_surf_int);
      mp_renderer->SetVolumeIntegrator(mp_volume_int);
      MemoryPool pool;
      RayDifferential ray(Ray(Point3D_d(0,0,0),Vector3D_d(1,0,0)));
      Spectrum_d radiance = mp_renderer->Radiance(ray, NULL, pool);

      // Since the mesh is a unit radius sphere with a point light in its center we can compute the radiance analytically.
      Spectrum_d analytical_radiance = Spectrum_d(100.0)*INV_PI * exp(-1.0) * exp(-1.0); // exp is multiplied twice to account for light attenuation and camera ray attenuation.
      CustomAssertDelta(radiance, analytical_radiance, 0.02);
      }

    void test_SamplerBasedRenderer_Transmittance()
      {
      mp_renderer->SetSurfaceIntegrator(mp_surf_int);
      mp_renderer->SetVolumeIntegrator(mp_volume_int);
      Ray ray(Point3D_d(0,0,0),Vector3D_d(1,0,0),0,1);
      Spectrum_d transmittance = mp_renderer->Transmittance(ray, NULL);

      TS_ASSERT_EQUALS(transmittance, mp_volume_int->Transmittance(ray,NULL));
      }

    // This will run in multiple threads depending on the number of threads available.
    void test_SamplerBasedRendererInsideSphere_Render()
      {
      mp_renderer->SetSurfaceIntegrator(mp_surf_int);
      mp_renderer->SetVolumeIntegrator(mp_volume_int);

      mp_renderer->Render(mp_camera);
      intrusive_ptr<Film> film = mp_camera->GetFilm();

      bool to_break=false;
      for(size_t x=0;x<film->GetXResolution() && to_break==false;++x)
        for(size_t y=0;y<film->GetXResolution();++y)
          {
          Spectrum_f spectrum;
          bool pixel_computed = film->GetPixel(Point2D_i(x,y), spectrum);

          if (pixel_computed==false) {TS_FAIL("Film pixel can not be computed.");to_break=false;break;}

          // Since the mesh is a unit radius sphere with a point light in its center we can compute the radiance analytically.
          Spectrum_f analytical_spectrum = Spectrum_f(100.0)*INV_PI * exp(-1.0) * exp(-1.0); // exp is multiplied twice to account for light attenuation and camera ray attenuation.
          Spectrum_f dif = analytical_spectrum-spectrum;
          if (fabs(dif[0])>0.02 || fabs(dif[0])>0.02 || fabs(dif[0])>0.02) {TS_FAIL("Wrong spectrum value.");to_break=false;break;}
          }
      }

  private:
    std::vector<intrusive_ptr<Primitive> > m_primitives;

    intrusive_ptr<Scene> mp_scene;
    intrusive_ptr<SurfaceIntegrator> mp_surf_int;
    intrusive_ptr<VolumeIntegrator> mp_volume_int;
    intrusive_ptr<SamplerBasedRenderer> mp_renderer;
    intrusive_ptr<Camera> mp_camera;
    intrusive_ptr<Log> mp_log;
  };

#endif // SAMPLER_BASED_RENDERER_TEST_H