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

#ifndef TESTTRACER_H
#define TESTTRACER_H

#include "stdafx.h"

#pragma warning(disable : 4003)
#include <Common/Common.h>

#include "tbb/pipeline.h"
#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"

//#include "vld.h"

#include "MeshLoader.h"
#include <Math/Geometry.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Raytracer/Core/Spectrum.h>
#include <Math/MathRoutines.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/FilmFilters/BoxFilter.h>
#include <Raytracer/FilmFilters/MitchellFilter.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Samplers/RandomSampler.h>
#include <Raytracer/Samplers/StratifiedSampler.h>
#include <Raytracer/Samplers/LDSampler.h>
#include <Raytracer/Cameras/PerspectiveCamera.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Samplers/UniformImagePixelsOrder.h>
#include <Raytracer/Samplers/ConsecutiveImagePixelsOrder.h>
#include <Raytracer/Samplers/RandomBlockedImagePixelsOrder.h>
#include <Raytracer/Films/ImageFilm.h>
#include <Raytracer/Films/InteractiveFilm.h>
#include <Raytracer/Renderers/SamplerBasedRenderer.h>
#include <Raytracer/LTEIntegrators/DirectLightingLTEIntegrator.h>
#include <Raytracer/LTEIntegrators/PhotonLTEIntegrator.h>
#include <Raytracer/Core/MIPMap.h>

// Disable compiler warning raised in the OpenEXR's code.
#pragma warning( push )
#pragma warning( disable : 4231 )
#include <ImfRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>
#pragma warning( pop )

#include <CADImport/SceneImporters/PbrtSceneImporter.h>

class TestTracer
  {
  public:
    TestTracer(int w, int h)
      {
      m_imageWidth=w;
      m_imageHeight=h;
      }

    void LoadMesh();

    void SetDisplayUpdateCallback(DisplayUpdateCallback *ip_callback)
      {
      mp_callback=ip_callback;
      }

    int GetImageWidth() const { return m_imageWidth; }
    int GetImageHeight() const { return m_imageHeight; }

    void RenderImage();

  public:
  int     m_imageWidth;
  int     m_imageHeight;

  DisplayUpdateCallback *mp_callback;
  intrusive_ptr<TriangleMesh> mp_mesh;
  intrusive_ptr<const Scene> mp_scene;
  intrusive_ptr<const Camera> mp_pbrt_camera;
  };


inline void TestTracer::LoadMesh()
{
  intrusive_ptr<Log> p_log( new StreamLog(std::cerr, Log::ERROR_LEVEL) );

  tbb::tick_count t0 = tbb::tick_count::now();
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\sponza-phomap.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\plants-godrays.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\plants-dusk.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\yeahright.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\tt.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\tt2.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\teapot-metal.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\spheres-differentials-texfilt.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\spheres.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\buddhamesh.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\metal-ssynth.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\smoke-2.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\sibenik-igi.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\sanmiguel.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\sanmiguel_cam3.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\sanmiguel_cam14.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\sanmiguel_cam25.pbrt", p_log);
  PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\dof-dragons.pbrt", p_log);

  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\villa-photons.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\villa-lights-on.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\raytracing\\pbrt_scenes\\villa-daylight.pbrt", p_log);

  
  importer.GetScene();

  mp_scene = importer.GetScene();
  mp_pbrt_camera = importer.GetCameras()[0];
  m_imageWidth = (int)mp_pbrt_camera->GetFilm()->GetXResolution();
  m_imageHeight = (int)mp_pbrt_camera->GetFilm()->GetYResolution();
  
  tbb::tick_count t1 = tbb::tick_count::now();
  printf("Importing: %lf\n", (t1-t0).seconds());
  return;
  }

inline void TestTracer::RenderImage()
  {
  //return;
  tbb::tick_count t0, t1;
  /*
  FilmFilter *filter = new MitchellFilter(2.0,2.0);
  //FilmFilter *filter = new BoxFilter(0.5,0.5);
  //intrusive_ptr<InteractiveFilm> p_film(new InteractiveFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  intrusive_ptr<ImageFilm> p_film(new ImageFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  //p_film->SetCropWindow(Point2D_i(400,500),Point2D_i(500,600));
  
  Point3D_d camera_pos(12,-1,7);
  Point3D_d look_at(-4,0.2,1.5);
  Vector3D_d direction = Vector3D_d(look_at-camera_pos).Normalized();
  intrusive_ptr<const Camera> p_camera(new PerspectiveCamera(MakeLookAt(camera_pos, direction, Vector3D_d(0, 0, 1)).Inverted(), p_film, 0.001*12.000, 6, 1.22));
  */

  intrusive_ptr<const Camera> p_camera = mp_pbrt_camera;
  //dynamic_cast<ImageFilm*>(const_cast<Film*>(p_camera->GetFilm().get()))->SetCropWindow(Point2D_i(400,250),Point2D_i(700,450));

  intrusive_ptr<ImagePixelsOrder> pixel_order(new ConsecutiveImagePixelsOrder);
  //intrusive_ptr<ImagePixelsOrder> pixel_order(new RandomBlockedImagePixelsOrder);
  //intrusive_ptr<ImagePixelsOrder> pixel_order(new UniformImagePixelsOrder);

  Point2D_i window_begin, window_end;
  p_camera->GetFilm()->GetSamplingExtent(window_begin, window_end);
  intrusive_ptr<Sampler> p_sampler( new LDSampler(window_begin, window_end, 8, pixel_order) );

 /*
  DirectLightingLTEIntegratorParams params;
  params.m_direct_light_samples_num=8;
  params.m_max_specular_depth=6;
  params.m_media_step_size=0.01;
  intrusive_ptr<DirectLightingLTEIntegrator> p_lte_int( new DirectLightingLTEIntegrator(mp_scene, params) );
 */
 
  PhotonLTEIntegratorParams params;
  params.m_direct_light_samples_num=16;
  params.m_gather_samples_num=16;
  params.m_caustic_lookup_photons_num=100;
  params.m_max_caustic_lookup_dist=0.05;
  params.m_max_specular_depth=10;
  params.m_media_step_size=0.01;
  //params.m_max_caustic_photons=6000000;
  //params.m_max_direct_photons=6000000;
  //params.m_max_indirect_photons=3000;
  intrusive_ptr<PhotonLTEIntegrator> p_lte_int( new PhotonLTEIntegrator(mp_scene, params) );

  t0 = tbb::tick_count::now();
  p_lte_int->ShootPhotons(50*1000000U, true);
  t1 = tbb::tick_count::now();
  printf("Shooting: %lf\n", (t1-t0).seconds());
  

  intrusive_ptr<SamplerBasedRenderer> p_renderer( new SamplerBasedRenderer(p_lte_int, p_sampler) );
  p_renderer->SetDisplayUpdateCallback(mp_callback, 60.0);
 
  tbb::task_scheduler_init init;
  t0 = tbb::tick_count::now();
  p_renderer->Render(p_camera, true);
  t1 = tbb::tick_count::now();

  printf("Rendering: %lf\n", (t1-t0).seconds());

  //p_film->ClearFilm();
  p_camera->GetFilm()->ClearFilm();
  p_sampler->Reset();

/*
  std::vector<Point2D_d> samples(128*128);
  SamplingRoutines::StratifiedSampling2D(samples.begin(),128,128,true);

  for(size_t i=0;i<samples.size();++i)
    {
    Vector3D_d dir;
    double pdf;
    mp_scene->GetLightSources().m_infinite_light_sources[0]->SampleLighting(samples[i], dir, pdf);

    double theta = MathRoutines::SphericalTheta(dir);
    double phi = MathRoutines::SphericalPhi(dir);

    p_film->AddSample(Point2D_d(phi*INV_2PI*p_film->GetXResolution(), theta*INV_PI*p_film->GetYResolution()), Spectrum_d(250,250,250));
    }

  mp_callback->Update(p_film);
*/
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  }

#endif // TESTTRACER_H