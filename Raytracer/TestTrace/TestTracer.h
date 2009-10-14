#ifndef TESTTRACER_H
#define TESTTRACER_H

#pragma warning(disable : 4003)
#include <Common/Common.h>

#include "tbb/pipeline.h"
#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"

#include <Math/Geometry.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Shapes/Sphere.h>
#include <Raytracer/Core/TriangleTree.h>
#include "stdafx.h"
#include <WinBase.h>
#include <cstdio>
#include <Raytracer/Core/Spectrum.h>
#include <Math/Util.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/FilmFilters/BoxFilter.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Samplers/RandomSampler.h>
#include <Raytracer/Samplers/StratifiedSampler.h>
#include <Raytracer/Cameras/PerspectiveCamera.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Samplers/UniformImagePixelsOrder.h>
#include <Raytracer/Samplers/ConsecutiveImagePixelsOrder.h>
#include <Raytracer/Films/ImageFilm.h>
#include <Raytracer/Films/InteractiveFilm.h>
#include <Raytracer/Materials/Matte.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Renderers/SamplerBasedRenderer.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <UnitTests/Mocks/SurfaceIntegratorMock.h>
#include <UnitTests/Mocks/VolumeIntegratorMock.h>

class TestTracer
  {
  public:
    TestTracer(int w, int h)
      {
      m_imageWidth=w;
      m_imageHeight=h;
      }

    void LoadMesh();

    void SetImage(Byte* image) {
      m_image = image;
      }

    int GetImageWidth() const { return m_imageWidth; }
    int GetImageHeight() const { return m_imageHeight; }

  void RenderImage(HWND &g_hWnd, HDC &g_memDC);
  bool ComputeDG(Point3D_d c, Vector3D_d dir, DifferentialGeometry &dg);

  public:
  Byte*   m_image;
  int     m_imageWidth;
  int     m_imageHeight;

  intrusive_ptr<TriangleMesh> mp_mesh;
  intrusive_ptr<Scene> mp_scene;
  };

int pixel_counter=0;

#include "tbb_pipeline_stuff.h"

inline void TestTracer::LoadMesh()
  {
  /*
  Sphere s;
  s.SetParameter("Center","0 0 0");
  s.SetParameter("Radius","0.4");
  s.SetParameter("Subdivisions","5");
  mp_mesh = s.BuildMesh();
*/
  std::vector<Point3D_f> vertices;
  std::vector<MeshTriangle> triangles;
  std::vector<float> uv_parameterization;

  std::map<std::pair<std::pair<float,float>,float>,size_t> uniq;
  std::vector<size_t> repl;
  #pragma warning(disable : 4996)
  FILE *fp=fopen("vertices.txt","r");
  int ver=0;
  while(true)
    {
   // if(++ver>100000) break;
    float x,y,z;
    int read = fscanf(fp,"%f %f %f",&x,&y,&z);
    if (read<=0) break;
    vertices.push_back(Point3D_f(x,y,z));

    if (uniq.find(std::make_pair(std::make_pair(x,y),z))==uniq.end())
      {
      uniq[std::make_pair(std::make_pair(x,y),z)]=vertices.size()-1;
      repl.push_back(vertices.size()-1);
      }
    else
      {
      repl.push_back(uniq[std::make_pair(std::make_pair(x,y),z)]);
      }
    }
  fclose(fp);

  int num_tr=0;
  fp=fopen("triangles.txt","r");
  int tr=0;
  while(true)
    {
    size_t v1,v2,v3;
   // if(++tr>10000) break;
    int read = fscanf(fp,"%d %d %d",&v1,&v2,&v3);
    if (read<=0) break;

    v1=repl[v1];
    v2=repl[v2];
    v3=repl[v3];
    if (v1==v2 || v1==v3 || v2==v3)
      continue;

    MeshTriangle tr(v1,v2,v3);
    triangles.push_back(tr);
    }
  fclose(fp);

  std::vector<intrusive_ptr<Primitive> > primitives;

  mp_mesh = intrusive_ptr<TriangleMesh>( new TriangleMesh(vertices, triangles, true) );

  intrusive_ptr<Texture<Spectrum_d> > p_reflectance(new ConstantTexture<Spectrum_d> (Spectrum_d(212,175,55)/255.0*0.95));
  intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.15));
  intrusive_ptr<Material> p_material(new Matte(p_reflectance, p_sigma));

  intrusive_ptr<Primitive> p_primitive(new Primitive(mp_mesh, p_material));
/*
  Sphere s;
  s.SetParameter("Center","0 0.28 0.05");
  s.SetParameter("Radius","0.006");
  s.SetParameter("Subdivisions","3");
  intrusive_ptr<TriangleMesh> p_sphere( s.BuildMesh() );
  intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(Spectrum_d(20000.0), mp_mesh) );
  intrusive_ptr<Primitive> p_sphere_primitive(new Primitive(p_sphere, p_material, p_area_light));

  std::vector<intrusive_ptr<Primitive> > primitives;
  primitives.push_back(p_primitive);
  primitives.push_back(p_sphere_primitive);
*/

  primitives.push_back(p_primitive);

  LightSources lights;
  intrusive_ptr<DeltaLightSource> p_source( new PointLight(Point3D_d(0, 0.35, 0.1), Spectrum_d(40.0)) );
  lights.m_delta_light_sources.push_back(p_source);
  mp_scene.reset(new Scene(primitives, lights));
  }

// 73 291
inline void TestTracer::RenderImage(HWND &g_hWnd, HDC &g_memDC)
  {
  FilmFilter *filter = new BoxFilter(0.5,0.5);
  InteractiveFilm *film = new InteractiveFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter));
  //film->SetCropWindow(Point2D_i(100,200),Point2D_i(550,300));

  Point2D_i window_begin,window_end;
  film->GetSamplingExtent(window_begin, window_end);

  Vector3D_d direction = Vector3D_d(0,-0.5,-1).Normalized();
  intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(Point3D_d(0.0,0.26,0.17)+direction*0.08,direction,Vector3D_d(0,1,0)), intrusive_ptr<Film>(film), 0.000, 0.087, 1.3) );

  intrusive_ptr<ImagePixelsOrder> pixel_order(new UniformImagePixelsOrder);

  intrusive_ptr<Sampler> p_sampler( new StratifiedSampler(window_begin, window_end, 2, 2, pixel_order, true) );

  intrusive_ptr<SamplerBasedRenderer> p_renderer( new SamplerBasedRenderer(mp_scene, p_sampler) );
  intrusive_ptr<SurfaceIntegrator> surf_int( new SurfaceIntegratorMock(p_renderer) );
  intrusive_ptr<VolumeIntegrator> volume_int( new VolumeIntegratorMock(p_renderer) );
  p_renderer->SetSurfaceIntegrator(surf_int);
  p_renderer->SetVolumeIntegrator(volume_int);

  tbb::task_scheduler_init init;
  tbb::tick_count t0 = tbb::tick_count::now();
  p_renderer->Render(p_camera);
  tbb::tick_count t1 = tbb::tick_count::now();

  printf("time = %g\n", (t1-t0).seconds());


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  for(int y=0;y<GetImageHeight();++y)
    for(int x=0;x<GetImageWidth();++x)
      {
      Spectrum_f sp;
      p_camera->GetFilm()->GetPixel(Point2D_i(x,y),sp);

      unsigned int pixel_index = (y*GetImageWidth()+x)*4;
      Byte* pixel = m_image;
      pixel[pixel_index+2] = Byte(std::min(sp[0],255.f));
      pixel[pixel_index+1] = Byte(std::min(sp[1],255.f));
      pixel[pixel_index+0] = Byte(std::min(sp[2],255.f));
      }

  BitBlt(GetDC(g_hWnd), 0, 0, GetImageWidth(), GetImageHeight(), g_memDC, 0, 0, SRCCOPY);
  
  }

#endif // TESTTRACER_H