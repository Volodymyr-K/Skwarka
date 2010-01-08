#ifndef TESTTRACER_H
#define TESTTRACER_H

#pragma warning(disable : 4003)
#include <Common/Common.h>

#include "tbb/pipeline.h"
#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"
#include <sstream>

#include "MeshLoader.h"
#include <Math/Geometry.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Shapes/Sphere.h>
#include "stdafx.h"
#include <WinBase.h>
#include <cstdio>
#include <Raytracer/Core/Spectrum.h>
#include <Math/MathRoutines.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/FilmFilters/BoxFilter.h>
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
#include <Raytracer/Materials/Matte.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Renderers/SamplerBasedRenderer.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Raytracer/LightSources/ParallelLight.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Raytracer/LTEIntegrators/DirectLightingLTEIntegrator.h>
#include <UnitTests/Mocks/InfiniteLightSourceMock.h>
#include <UnitTests/Mocks/VolumeIntegratorMock.h>
#include <Raytracer/LightsSamplingStrategies/IrradianceLightsSampling.h>
#include <Raytracer/LightsSamplingStrategies/PowerLightsSampling.h>
#include <Raytracer/Core/MIPMap.h>
#include <Raytracer/Textures/ImageTexture.h>
#include <Raytracer/Mappings/SphericalMapping2D.h>
#include <Raytracer/Mappings/UVMapping2D.h>
#include <Raytracer/Materials/Transparent.h>
#include <Raytracer/Materials/Metal.h>
#include <Raytracer/Core/Fresnel.h>
#include "EasyBMP.h"

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
  intrusive_ptr<Scene> mp_scene;
  };

intrusive_ptr<Primitive> LoadWallsPrimitive(std::string i_filename, bool i_smooth)
  {
  intrusive_ptr<TriangleMesh> p_mesh( LoadMeshFromStl(i_filename, i_smooth) );

  BMP Input;
  Input.ReadFromFile("walls.bmp");

  std::vector<std::vector<Spectrum_f> > values(Input.TellHeight(),std::vector<Spectrum_f>(Input.TellWidth()));
  for( int j=0 ; j < Input.TellHeight() ; j++)
    {
    for( int i=0 ; i < Input.TellWidth() ; i++)
      {
      values[j][i]=Spectrum_f(Input(i,j)->Red,Input(i,j)->Green,Input(i,j)->Blue)*1.0;
      values[j][i]/=255.0;
      }
    }
    
  intrusive_ptr<Mapping2D> p_mapping( new SphericalMapping2D(Point3D_d(-770,-2315,1500), Vector3D_d(0,0,1), Vector3D_d(1,0,0)) );
  intrusive_ptr< ImageTexture<Spectrum_f,Spectrum_d> > p_text(new ImageTexture<Spectrum_f,Spectrum_d>(values, p_mapping) );

  intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.04));
  intrusive_ptr<Material> p_material(new Matte(p_text, p_sigma));

  intrusive_ptr<Primitive> p_primitive(new Primitive(p_mesh, p_material));
  return p_primitive;
  }

intrusive_ptr<Primitive> LoadDiffusePrimitive(std::string i_filename, bool i_smooth, Spectrum_d i_color)
  {
  intrusive_ptr<TriangleMesh> p_mesh( LoadMeshFromStl(i_filename, i_smooth) );

  intrusive_ptr<Texture<Spectrum_d> > p_reflectance(new ConstantTexture<Spectrum_d> (i_color));
  intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.04));
  intrusive_ptr<Material> p_material(new Matte(p_reflectance, p_sigma));

  intrusive_ptr<Primitive> p_primitive(new Primitive(p_mesh, p_material));
  return p_primitive;
  }

intrusive_ptr<Primitive> LoadGlassPrimitive(std::string i_filename, bool i_smooth, Spectrum_d i_color)
  {
  intrusive_ptr<TriangleMesh> p_mesh( LoadMeshFromStl(i_filename, i_smooth) );

  intrusive_ptr<Texture<Spectrum_d> > p_reflectance(new ConstantTexture<Spectrum_d> (Spectrum_d(1.0)));
  intrusive_ptr<Texture<Spectrum_d> > p_transmittance(new ConstantTexture<Spectrum_d> (i_color));
  intrusive_ptr<Material> p_material(new Transparent(p_reflectance, p_transmittance, 1.4));

  intrusive_ptr<Primitive> p_primitive(new Primitive(p_mesh, p_material));
  return p_primitive;
  }

intrusive_ptr<Primitive> LoadMetalPrimitive(std::string i_filename, bool i_smooth, Spectrum_d i_color, double i_roughness)
  {
  intrusive_ptr<TriangleMesh> p_mesh( LoadMeshFromStl(i_filename, i_smooth) );

  intrusive_ptr<Texture<Spectrum_d> > p_refrlection(new ConstantTexture<Spectrum_d> (i_color));
  intrusive_ptr<Texture<double> > p_roughness(new ConstantTexture<double> (i_roughness));
  intrusive_ptr<Material> p_material(new Metal(p_refrlection, p_roughness));

  intrusive_ptr<Primitive> p_primitive(new Primitive(p_mesh, p_material));
  return p_primitive;
  }


inline void TestTracer::LoadMesh()
  {
  std::vector<intrusive_ptr<const Primitive> > primitives;

  primitives.push_back(LoadWallsPrimitive("stls/walls.stl", false));
  primitives.push_back(LoadDiffusePrimitive("stls/floor.stl", false, Spectrum_d(248,244,180)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/ceiling.stl", false, Spectrum_d(248,244,180)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/window.stl", false, Spectrum_d(238,194,121)/255.0));

  primitives.push_back(LoadDiffusePrimitive("stls/door.stl", false, Spectrum_d(238,194,121)/255.0));
  primitives.push_back(LoadGlassPrimitive("stls/door_glass.stl", false, Spectrum_d(255.0,255.0,255.0)/255.0));

  primitives.push_back(LoadDiffusePrimitive("stls/table.stl", false, Spectrum_d(238,194,121)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/chair1.stl", false, Spectrum_d(127,90,48)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/chair2.stl", false, Spectrum_d(127,90,48)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/chair3.stl", false, Spectrum_d(127,90,48)/255.0));

  primitives.push_back(LoadMetalPrimitive("stls/lamp1.stl", true, Spectrum_d(240,240,240)/255.0, 0.05));
  primitives.push_back(LoadMetalPrimitive("stls/lamp2.stl", true, Spectrum_d(240,240,240)/255.0, 0.05));

  primitives.push_back(LoadDiffusePrimitive("stls/shelf1.stl", false, Spectrum_d(255,245,245)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/shelf2.stl", false, Spectrum_d(255,245,245)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/upper_shelf.stl", false, Spectrum_d(173,216,230)/255.0));

  primitives.push_back(LoadGlassPrimitive("stls/fruit_plate.stl", true, Spectrum_d(255,210,210)/255.0));

  primitives.push_back(LoadDiffusePrimitive("stls/oranges.stl", true, Spectrum_d(255,165,0)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/strawberry.stl", true, Spectrum_d(255,3,62)/255.0));

  //primitives.push_back(LoadMetalPrimitive("stls/bin.stl", true, Spectrum_d(255,255,150)/255.0, 0.1));
  primitives.push_back(LoadGlassPrimitive("stls/bottles.stl", true, Spectrum_d(255,255,255)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/cups.stl", true, Spectrum_d(255,255,255)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/shoes.stl", false, Spectrum_d(255,255,255)/255.0));

  primitives.push_back(LoadDiffusePrimitive("stls/cooker_plate.stl", false, Spectrum_d(50,50,50)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/saucepan_handles.stl", false, Spectrum_d(50,50,50)/255.0));
  primitives.push_back(LoadMetalPrimitive("stls/saucepan.stl", true, Spectrum_d(184,115,51)/255.0, 0.1));
  primitives.push_back(LoadMetalPrimitive("stls/teapot.stl", true, Spectrum_d(255,215,0)/255.0, 0.1));
  primitives.push_back(LoadMetalPrimitive("stls/teapot2.stl", true, Spectrum_d(255,215,0)/255.0, 0.1));
  primitives.push_back(LoadMetalPrimitive("stls/extractor_fan.stl", true, Spectrum_d(201,192,187)/255.0, 0.1));

  primitives.push_back(LoadDiffusePrimitive("stls/frying_pan1_handle.stl", false, Spectrum_d(50,50,50)/255.0));
  primitives.push_back(LoadDiffusePrimitive("stls/frying_pan2_handle.stl", false, Spectrum_d(50,50,50)/255.0));
  primitives.push_back(LoadMetalPrimitive("stls/frying_pan1.stl", true, Spectrum_d(255,40,50)/255.0, 0.1));
  primitives.push_back(LoadMetalPrimitive("stls/frying_pan2.stl", true, Spectrum_d(201,192,187)/255.0, 0.1));

  LightSources lights;

  intrusive_ptr<InfiniteLightSource> p_inf_light( new InfiniteLightSourceMock(Spectrum_d(200.0,220.0,250.0), BBox3D_d(Point3D_d(-3000,-5000,-500),Point3D_d(1500,1000,3500) ) ) );
  lights.m_infinitiy_light_sources.push_back(p_inf_light);

  intrusive_ptr<DeltaLightSource> p_parallel_light( new ParallelLight(Vector3D_d(-0.3,-0.5,-0.2).Normalized(), Spectrum_d(M_PI*500.0), BBox3D_d(Point3D_d(-3000,-5000,-500),Point3D_d(1500,1000,3500) ) ) );
  lights.m_delta_light_sources.push_back(p_parallel_light);

  mp_scene.reset(new Scene(primitives, lights));
  }

inline void TestTracer::RenderImage()
  {
  FilmFilter *filter = new BoxFilter(0.5,0.5);
  intrusive_ptr<InteractiveFilm> p_film(new InteractiveFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  //intrusive_ptr<ImageFilm> p_film(new ImageFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  //p_film->SetCropWindow(Point2D_i(0,0),Point2D_i(300,300));

  Point2D_i window_begin,window_end;
  p_film->GetSamplingExtent(window_begin, window_end);

  //Vector3D_d direction = Vector3D_d(-2441-831,38+4831,1342-1194-2200).Normalized();
  //intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(Point3D_d(-600,-1783,778)-direction*500,direction,Vector3D_d(0,0,1)), p_film, 0.001*12.000, 1300, 1.3) );

  Vector3D_d direction = Vector3D_d(-2441-831,-590+4331,1342-1194).Normalized();
  intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(Point3D_d(831,-4331,1194),direction,Vector3D_d(0,0,1)), p_film, 0.001*12.000, 1300, 1.35) );

  //intrusive_ptr<ImagePixelsOrder> pixel_order(new UniformImagePixelsOrder);
  intrusive_ptr<ImagePixelsOrder> pixel_order(new RandomBlockedImagePixelsOrder);
  //intrusive_ptr<ImagePixelsOrder> pixel_order(new ConsecutiveImagePixelsOrder);

  //intrusive_ptr<Sampler> p_sampler( new StratifiedSampler(window_begin, window_end, 2, 2/*, pixel_order*/) );
  intrusive_ptr<Sampler> p_sampler( new LDSampler(window_begin, window_end, 4*2, pixel_order) );
  //intrusive_ptr<Sampler> p_sampler( new RandomSampler(window_begin, window_end, 1/*, pixel_order*/) );

  intrusive_ptr<LightsSamplingStrategy> p_sampling_strategy( new IrradianceLightsSampling(mp_scene->GetLightSources()) );

  //intrusive_ptr<VolumeIntegrator> p_volume_int( new VolumeIntegratorMock() );
  intrusive_ptr<DirectLightingIntegrator> p_direct_int( new DirectLightingIntegrator(mp_scene, NULL, 64/4, 16/4, /*121, 25,*/ p_sampling_strategy) );
  intrusive_ptr<LTEIntegrator> p_lte_int( new DirectLightingLTEIntegrator(mp_scene, NULL, p_direct_int, 6) );

  intrusive_ptr<SamplerBasedRenderer> p_renderer( new SamplerBasedRenderer(p_lte_int, p_sampler) );
  p_renderer->SetDisplayUpdateCallback(mp_callback, 2.0);

  tbb::task_scheduler_init init;
  tbb::tick_count t0 = tbb::tick_count::now();
  p_renderer->Render(p_camera);
  tbb::tick_count t1 = tbb::tick_count::now();

  printf("time = %g\n", (t1-t0).seconds());
  p_film->ClearFilm();
  p_sampler->Reset();

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  }

#endif // TESTTRACER_H