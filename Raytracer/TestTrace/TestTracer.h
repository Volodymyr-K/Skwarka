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
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Raytracer/SurfaceIntegrators/DirectLightingSurfaceIntegrator.h>
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

int pixel_counter=0;

inline void TestTracer::LoadMesh()
  {
  BMP Input;
  Input.ReadFromFile("image.bmp");

  std::vector<std::vector<Spectrum_f> > values(Input.TellHeight(),std::vector<Spectrum_f>(Input.TellWidth()));
  for( int j=0 ; j < Input.TellHeight() ; j++)
    {
    for( int i=0 ; i < Input.TellWidth() ; i++)
      {
      values[j][i]=Spectrum_f(Input(i,j)->Red,Input(i,j)->Green,Input(i,j)->Blue)*0.8;
      values[j][i]/=255.0;
      }
    }
/*
  MIPMap<Spectrum_d> mipmap(values,true,8);

  Input.SetSize(mipmap.m_levels[0]->GetSizeV(),mipmap.m_levels[0]->GetSizeU());
  for( int j=0 ; j < Input.TellHeight() ; j++)
    {
    for( int i=0 ; i < Input.TellWidth() ; i++)
      {
      //values[j][i] = mipmap.Get(Point2D_d(double(i+0.5)/Input.TellWidth(), double(j+0.5)/Input.TellHeight()), width);
      Spectrum_d v = mipmap.m_levels[0]->Get(j,i)*255.0;
      v.Clamp(0.0,255.0);
      Input(i,j)->Red = (int) (v[0]+0.5);
      Input(i,j)->Green = (int) (v[1]+0.5);
      Input(i,j)->Blue = (int) (v[2]+0.5);
      }
    }

  char buf[256];
  sprintf(buf,"image%d.bmp",1);
  Input.WriteToFile(buf);
*/
  intrusive_ptr<Mapping2D> p_mapping( new SphericalMapping2D(Point3D_d(100,-400,600), Vector3D_d(0,1,0), Vector3D_d(0,0,-1)) );
  //intrusive_ptr<Mapping2D> p_mapping( new UVMapping2D(16.0,16.0,Vector2D_d(-0.057,-0.003)) );
  //intrusive_ptr<Mapping2D> p_mapping( new UVMapping2D() );
  intrusive_ptr< ImageTexture<Spectrum_f,Spectrum_d> > p_text(new ImageTexture<Spectrum_f,Spectrum_d>(values, p_mapping) );

  std::vector<Point3D_f> vertices;
  std::vector<MeshTriangle> triangles;
  std::vector<float> uv_parameterization;

  std::vector<intrusive_ptr<const Primitive> > primitives;

  /////// Add car primitive ///
  
  mp_mesh = intrusive_ptr<TriangleMesh>( LoadMeshFromStl("car.stl") );

  intrusive_ptr<Texture<Spectrum_d> > p_reflectance(new ConstantTexture<Spectrum_d> (Spectrum_d(212,175,55)/255.0*0.8));
  intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.04));
  intrusive_ptr<Material> p_material(new Matte(p_text, p_sigma));

  intrusive_ptr<Primitive> p_primitive(new Primitive(mp_mesh, p_material));
  primitives.push_back(p_primitive);

  /////// Add bunny primitive ///

  mp_mesh = intrusive_ptr<TriangleMesh>( LoadMeshFromPbrt("vertices.txt","triangles.txt") );

  p_material.reset(new Matte(p_reflectance, p_sigma));

  p_primitive.reset(new Primitive(mp_mesh, p_material));
  primitives.push_back(p_primitive);


  /////// Add buddha primitive ///

  mp_mesh = intrusive_ptr<TriangleMesh>( LoadMeshFromPbrt("vertices2.txt","triangles2.txt") );

  p_reflectance.reset(new ConstantTexture<Spectrum_d> (Spectrum_d(22,235,75)/255.0*0.8));
  p_material.reset(new Matte(p_reflectance, p_sigma));

  p_primitive.reset(new Primitive(mp_mesh, p_material));
  primitives.push_back(p_primitive);


  /////// Add buddha2 primitive ///
/*
  mp_mesh = intrusive_ptr<TriangleMesh>( LoadMeshFromPbrt("vertices3.txt","triangles2.txt") );

  p_reflectance.reset(new ConstantTexture<Spectrum_d> (Spectrum_d(250,180,75)/255.0*0.8));
  p_material.reset(new Matte(p_reflectance, p_sigma));

  p_primitive.reset(new Primitive(mp_mesh, p_material));
  primitives.push_back(p_primitive);
*/
  /////// Add ground primitive ///

  vertices.clear();
  vertices.push_back(Point3D_f(-20000,-20000,0));vertices.push_back(Point3D_f(20000,-20000,0));
  vertices.push_back(Point3D_f(20000,20000,0));vertices.push_back(Point3D_f(-20000,20000,0));
  triangles.clear();
  triangles.push_back(MeshTriangle(0,1,2));triangles.push_back(MeshTriangle(2,3,0));
  triangles[0].m_uvs[0]=Point2D_f(0,0);triangles[0].m_uvs[1]=Point2D_f(1,0);triangles[0].m_uvs[2]=Point2D_f(1,1);
  triangles[1].m_uvs[0]=Point2D_f(1,1);triangles[1].m_uvs[1]=Point2D_f(0,1);triangles[1].m_uvs[2]=Point2D_f(0,0);
  intrusive_ptr<TriangleMesh> p_ground_mesh = intrusive_ptr<TriangleMesh>( new TriangleMesh(vertices, triangles, true) );

  p_reflectance.reset(new ConstantTexture<Spectrum_d> (Spectrum_d(200,200,200)/255.0*0.6));
  p_sigma.reset(new ConstantTexture<double> (0.35));
  p_material.reset(new Matte(p_reflectance, p_sigma));
  //p_material.reset(new Matte(p_text, p_sigma));

  p_primitive.reset(new Primitive(p_ground_mesh, p_material));
  primitives.push_back(p_primitive);

  LightSources lights;
 /*
    {
  Sphere s;
  s.SetParameter("Center","900 -1800 3000");
  s.SetParameter("Radius","150");
  s.SetParameter("Subdivisions","3");
  intrusive_ptr<TriangleMesh> p_sphere( s.BuildMesh() );
  intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(Spectrum_d(10000.0,10000.0,10000.0), p_sphere) );
  intrusive_ptr<Primitive> p_sphere_primitive(new Primitive(p_sphere, p_material, p_area_light));

  primitives.push_back(p_sphere_primitive);
  lights.m_area_light_sources.push_back(p_area_light);
    }
    */
  
    {
  Sphere s;
  s.SetParameter("Center","-900 -1800 3000");
  s.SetParameter("Radius","150");
  s.SetParameter("Subdivisions","3");
  intrusive_ptr<TriangleMesh> p_sphere( s.BuildMesh() );
  intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(Spectrum_d(10000.0,10000.0,5000.0), p_sphere) );
  intrusive_ptr<Primitive> p_sphere_primitive(new Primitive(p_sphere, p_material, p_area_light));

  primitives.push_back(p_sphere_primitive);
  lights.m_area_light_sources.push_back(p_area_light);
    }

  intrusive_ptr<InfiniteLightSource> p_inf_light( new InfiniteLightSourceMock(Spectrum_d(300.0,300.0,300.0), BBox3D_d(Point3D_d(-20000,-20000,0),Point3D_d(20000,20000,1000) ) ) );
  lights.m_infinitiy_light_sources.push_back(p_inf_light);

  mp_scene.reset(new Scene(primitives, lights));
  }

inline void TestTracer::RenderImage()
  {
  FilmFilter *filter = new BoxFilter(0.5,0.5);
  intrusive_ptr<Film> p_film(new InteractiveFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  //intrusive_ptr<Film> p_film(new ImageFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter)));
  //film->SetCropWindow(Point2D_i(0,106-27),Point2D_i(800,200-27));

  Point2D_i window_begin,window_end;
  p_film->GetSamplingExtent(window_begin, window_end);

  Vector3D_d direction = Vector3D_d(-0.45,0.5,-0.28).Normalized();
  intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(Point3D_d(900,-1600,900),direction,Vector3D_d(0,0,1)), p_film, 12.000, 1300, 1.3) );

  //intrusive_ptr<ImagePixelsOrder> pixel_order(new UniformImagePixelsOrder);
  intrusive_ptr<ImagePixelsOrder> pixel_order(new RandomBlockedImagePixelsOrder);
  //intrusive_ptr<ImagePixelsOrder> pixel_order(new ConsecutiveImagePixelsOrder);

  //intrusive_ptr<Sampler> p_sampler( new StratifiedSampler(window_begin, window_end, 2, 2/*, pixel_order*/) );
  intrusive_ptr<Sampler> p_sampler( new LDSampler(window_begin, window_end, 4, pixel_order) );
  //intrusive_ptr<Sampler> p_sampler( new RandomSampler(window_begin, window_end, 1/*, pixel_order*/) );

  intrusive_ptr<SamplerBasedRenderer> p_renderer( new SamplerBasedRenderer(mp_scene, p_sampler) );
  p_renderer->SetDisplayUpdateCallback(mp_callback, 5.0);

  intrusive_ptr<LightsSamplingStrategy> p_sampling_strategy( new IrradianceLightsSampling(mp_scene->GetLightSources()) );
  intrusive_ptr<DirectLightingIntegrator> p_direct_int( new DirectLightingIntegrator(p_renderer, 64/4, 16/4, /*121, 25,*/ p_sampling_strategy) );
  intrusive_ptr<SurfaceIntegrator> surf_int( new DirectLightingSurfaceIntegrator(p_renderer, p_direct_int, 5, 0.1) );
  intrusive_ptr<VolumeIntegrator> volume_int( new VolumeIntegratorMock(p_renderer) );
  p_renderer->SetSurfaceIntegrator(surf_int);
  //p_renderer->SetVolumeIntegrator(volume_int);

  tbb::task_scheduler_init init;
  tbb::tick_count t0 = tbb::tick_count::now();
  p_renderer->Render(p_camera);
  tbb::tick_count t1 = tbb::tick_count::now();

  printf("time = %g\n", (t1-t0).seconds());

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  }

#endif // TESTTRACER_H