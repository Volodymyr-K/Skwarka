/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SAMPLER_BASED_RENDERER_TEST_H
#define SAMPLER_BASED_RENDERER_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Renderers/SamplerBasedRenderer.h>
#include "Mocks/LTEIntegratorMock.h"
#include "Mocks/MaterialMock.h"
#include "Mocks/InfiniteLightSourceMock.h"
#include "Mocks/VolumeRegionMock.h"
#include <Raytracer/Cameras/PerspectiveCamera.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Raytracer/Films/ImageFilm.h>
#include <Raytracer/FilmFilters/BoxFilter.h>
#include <Raytracer/Samplers/StratifiedSampler.h>
#include <UnitTests/TestHelpers/TriangleMeshTestHelper.h>

class SamplerBasedRendererTestSuite : public CxxTest::TestSuite
  {
  public:

    // Primitives creation separated to the constructor (instead of setUp() method) to avoid performance overhead.
    SamplerBasedRendererTestSuite()
      {
      intrusive_ptr<TriangleMesh> p_sphere( TriangleMeshHelper::ConstructSphere(Point3D_d(0,0,0),1.0, 6) );
      intrusive_ptr<Material> p_material(new MaterialMock());
      intrusive_ptr<Primitive> p_sphere_primitive(new Primitive(p_sphere, Transform(), p_material));

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

      intrusive_ptr<const VolumeRegion> p_volume( new VolumeRegionMock(BBox3D_d(Point3D_d(-1,-1,-1),Point3D_d(1,1,1)), Spectrum_d(0), SpectrumCoef_d(0.5), SpectrumCoef_d(0.5)) );

      mp_scene.reset( new Scene(m_primitives, p_volume, lights) );

      intrusive_ptr<FilmFilter> p_filter( new BoxFilter(0.5,0.5) );
      intrusive_ptr<Film> p_film( new ImageFilm(10, 10, p_filter) );
      mp_camera.reset( new PerspectiveCamera( MakeLookAt(Point3D_d(0.0,0.0,0.0),Vector3D_d(1.0,0,0),Vector3D_d(0,0,1)), p_film, 0.000, 1.0, 1.3) );

      Point2D_i window_begin,window_end;
      p_film->GetSamplingExtent(window_begin, window_end);
      mp_sampler.reset( new StratifiedSampler(window_begin, window_end, 2, 2) );
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    // This will run in multiple threads depending on the number of threads available.
    void test_SamplerBasedRendererInsideSphere_Render()
      {
      intrusive_ptr<LTEIntegrator> p_lte_int( new LTEIntegratorMock(mp_scene) );
      intrusive_ptr<SamplerBasedRenderer> p_renderer( new SamplerBasedRenderer(p_lte_int, mp_sampler, intrusive_ptr<Log>(new StreamLog())) );

      p_renderer->Render(mp_camera);
      intrusive_ptr<Film> film = mp_camera->GetFilm();

      for(size_t x=0;x<film->GetXResolution();++x)
        for(size_t y=0;y<film->GetXResolution();++y)
          {
          Spectrum_d spectrum;
          bool pixel_computed = film->GetPixel(Point2D_i((int)x, (int)y), spectrum);

          if (pixel_computed==false)
            {
            TS_FAIL("Film pixel can not be computed.");
            return;
            }

          // Since the mesh is a unit radius sphere with a point light in its center we can compute the radiance analytically.
          Spectrum_d analytical_spectrum = Spectrum_d(100.0)*INV_PI * exp(-1.0) * exp(-1.0); // exp is multiplied twice to account for light attenuation and camera ray attenuation.
          Spectrum_d dif = analytical_spectrum-spectrum;
          if (fabs(dif[0])>0.02 || fabs(dif[1])>0.02 || fabs(dif[2])>0.02)
            {
            TS_FAIL("Wrong spectrum value.");
            return;
            }
          }
      }

  private:
    std::vector<intrusive_ptr<const Primitive>> m_primitives;

    intrusive_ptr<Scene> mp_scene;
    intrusive_ptr<Sampler> mp_sampler;
    intrusive_ptr<Camera> mp_camera;
  };

#endif // SAMPLER_BASED_RENDERER_TEST_H