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
#include <Shapes/Sphere.h>
#include <Shapes/Cylinder.h>
#include <Shapes/Disk.h>
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
#include <Raytracer/Materials/MatteMaterial.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Renderers/SamplerBasedRenderer.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Raytracer/LightSources/SpotPointLight.h>
#include <Raytracer/LightSources/ParallelLight.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Raytracer/LTEIntegrators/DirectLightingLTEIntegrator.h>
#include <Raytracer/LTEIntegrators/PhotonLTEIntegrator.h>
#include <UnitTests/Mocks/InfiniteLightSourceMock.h>
#include <Raytracer/LightsSamplingStrategies/IrradianceLightsSamplingStrategy.h>
#include <Raytracer/LightsSamplingStrategies/PowerLightsSamplingStrategy.h>
#include <Raytracer/Core/MIPMap.h>
#include <Raytracer/Textures/ImageTexture.h>
#include <Raytracer/Mappings/SphericalMapping2D.h>
#include <Raytracer/Mappings/UVMapping2D.h>
#include <Raytracer/Materials/TransparentMaterial.h>
#include <Raytracer/Materials/MetalMaterial.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/CompressedDirection.h>
#include <Raytracer/Materials/SubstrateMaterial.h>
#include <Raytracer/Materials/PlasticMaterial.h>
#include <Raytracer/Materials/MixMaterial.h>
#include <Raytracer/Materials/MERLMeasuredMaterial.h>
#include <Raytracer/BxDFs/FresnelBlend.h>
#include <Raytracer/MicrofacetDistributions/AnisotropicDistribution.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include "EasyBMP.h"
#include <Raytracer/LightSources/ImageEnvironmentalLight.h>
#include <Raytracer/VolumeRegions/HomogeneousVolumeRegion.h>
#include <Raytracer/VolumeRegions/GridDensityVolumeRegion.h>
#include <Raytracer/PhaseFunctions/IsotropicPhaseFunction.h>
#include <Raytracer/PhaseFunctions/MieHazyPhaseFunction.h>
#include <Raytracer/PhaseFunctions/MieMurkyPhaseFunction.h>
#include <Raytracer/PhaseFunctions/RayleighPhaseFunction.h>
#include <Raytracer/ImageSources/RGB24ImageSource.h>
#include <Raytracer/ImageSources/OpenEXRRgbaImageSource.h>

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
  };

intrusive_ptr<const Camera> p_pbrt_camera;

inline void TestTracer::LoadMesh()
  {
  intrusive_ptr<Log> p_log( new StreamLog );
  PbrtSceneImporter importer("D:\\pbrt\\v2.0\\pbrt-scenes\\pbrt-scenes\\plants-modified.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\pbrt\\v2.0\\pbrt-scenes\\pbrt-scenes\\tt.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\pbrt\\v2.0\\pbrt-scenes\\pbrt-scenes\\yeahright.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\pbrt\\v2.0\\pbrt-scenes\\pbrt-scenes\\sponza-phomap.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\pbrt\\v2.0\\pbrt-source\\scenes\\prt-teapot.pbrt", p_log);
  //PbrtSceneImporter importer("D:\\pbrt\\v2.0\\pbrt-scenes\\pbrt-scenes\\geometry\\sanmiguel\\plantas.pbrt", p_log);
  importer.GetScene();

  mp_scene = importer.GetScene();
  p_pbrt_camera = importer.GetCameras()[0];
  m_imageWidth = (int)p_pbrt_camera->GetFilm()->GetXResolution();
  m_imageHeight = (int)p_pbrt_camera->GetFilm()->GetYResolution();
  return;
  }

inline void TestTracer::RenderImage()
  {
  tbb::tick_count t0, t1;
  FilmFilter *filter = new MitchellFilter(2.0,2.0);
  //FilmFilter *filter = new BoxFilter(1.0,1.0);
  //intrusive_ptr<InteractiveFilm> p_film(new InteractiveFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  intrusive_ptr<ImageFilm> p_film(new ImageFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  //p_film->SetCropWindow(Point2D_i(400,500),Point2D_i(500,600));

  Point2D_i window_begin,window_end;
  p_film->GetSamplingExtent(window_begin, window_end);

  Point3D_d camera_pos(9.5,-2.4,7);
  Point3D_d look_at(2.5,-1.5,0.0);
  Vector3D_d direction = Vector3D_d(look_at-camera_pos).Normalized();
  intrusive_ptr<const Camera> p_camera( new PerspectiveCamera( MakeLookAt(camera_pos,direction,Vector3D_d(0,0,1)).Inverted(), p_film, 0.001*12.000, 6, 1.22) );

  p_camera = p_pbrt_camera;

  intrusive_ptr<ImagePixelsOrder> pixel_order(new ConsecutiveImagePixelsOrder);
  //intrusive_ptr<ImagePixelsOrder> pixel_order(new RandomBlockedImagePixelsOrder);

  intrusive_ptr<Sampler> p_sampler( new LDSampler(window_begin, window_end, 2, pixel_order) );


  DirectLightingLTEIntegratorParams params;
  params.m_direct_light_samples_num=8;
  params.m_max_specular_depth=6;
  params.m_media_step_size=0.01;
  intrusive_ptr<DirectLightingLTEIntegrator> p_lte_int( new DirectLightingLTEIntegrator(mp_scene, params) );

/*
  PhotonLTEIntegratorParams params;
  params.m_direct_light_samples_num=8;
  params.m_gather_samples_num=8;
  params.m_caustic_lookup_photons_num=100;
  params.m_max_caustic_lookup_dist=0.05;
  params.m_max_specular_depth=10;
  params.m_media_step_size=0.01;
  intrusive_ptr<PhotonLTEIntegrator> p_lte_int( new PhotonLTEIntegrator(mp_scene, params) );
*/

  t0 = tbb::tick_count::now();
  //p_lte_int->ShootPhotons(1000000, 20000000, 20000000, true);
  t1 = tbb::tick_count::now();
  printf("Shooting: %lf\n", (t1-t0).seconds());

  intrusive_ptr<SamplerBasedRenderer> p_renderer( new SamplerBasedRenderer(p_lte_int, p_sampler) );
  p_renderer->SetDisplayUpdateCallback(mp_callback, 60.0);
 
  tbb::task_scheduler_init init;
  t0 = tbb::tick_count::now();
  p_renderer->Render(p_camera, true);
  t1 = tbb::tick_count::now();

  printf("Rendering: %lf\n", (t1-t0).seconds());
  p_film->ClearFilm();
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