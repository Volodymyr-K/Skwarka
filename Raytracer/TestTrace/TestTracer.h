#ifndef TESTTRACER_H
#define TESTTRACER_H

#pragma warning(disable : 4003)
#include <Common/Common.h>

#include "tbb/pipeline.h"
#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"
#include <sstream>

#include <Math/Geometry.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Shapes/Sphere.h>
#include <Raytracer/Core/TriangleTree.h>
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
#include <Raytracer/SurfaceIntegrators/DirectLightingSurfaceIntegrator.h>
#include <UnitTests/Mocks/InfiniteLightSourceMock.h>
#include <UnitTests/Mocks/VolumeIntegratorMock.h>
#include <Raytracer/LightsSamplingStrategies/IrradianceLightsSampling.h>
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
  //intrusive_ptr<Mapping2D> p_mapping( new SphericalMapping2D(Point3D_d(100,-400,300), Vector3D_d(0,0,1), Vector3D_d(1,0,0)) );
  //intrusive_ptr<Mapping2D> p_mapping( new UVMapping2D(16.0,16.0,Vector2D_d(-0.057,-0.003)) );
  intrusive_ptr<Mapping2D> p_mapping( new UVMapping2D() );
  intrusive_ptr< ImageTexture<Spectrum_f,Spectrum_d> > p_text(new ImageTexture<Spectrum_f,Spectrum_d>(values, p_mapping) );

  /*
  Sphere s;
  s.SetParameter("Center","0 0 0");
  s.SetParameter("Radius","0.4");
  s.SetParameter("Subdivisions","5");
  mp_mesh = s.BuildMesh();
*/
  /*
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
  */

  std::vector<Point3D_f> vertices;
  std::vector<MeshTriangle> triangles;
  std::vector<float> uv_parameterization;

  std::map< std::pair<float,std::pair<float,float> >, size_t > vertices_tmp;
#pragma warning(disable : 4996)
  FILE *fp=fopen("car.STL","r");

  Vector3D_f normal;
  Point3D_f v1,v2,v3;
  size_t ind=0,num_ver=0;
  while(true)
    {
    char buf[1024];
    if ( !fgets(buf,1024,fp) ) break;

    std::string s=buf;
    if (s.find("normal")!=-1)
      {
      std::stringstream sstream;
      sstream << s;

      std::string dummy;
      float x,y,z;
      sstream >> dummy >> dummy >> x >> y >> z;
      normal=Vector3D_f(x,y,z);
      }
    if (s.find("vertex")!=-1)
      {
      std::stringstream sstream;
      sstream << s;

      std::string dummy;
      float x,y,z;
      sstream >> dummy >> x >> y >> z;
      if (num_ver==0) v1=Point3D_f(x,y,z);
      if (num_ver==1) v2=Point3D_f(x,y,z);
      if (num_ver==2) v3=Point3D_f(x,y,z);
      ++num_ver;
      }

    size_t ind1,ind2,ind3;
    if (num_ver==3)
      {
      num_ver=0;
      std::pair<float,std::pair<float,float> > vertex1=std::make_pair(v1[0],std::make_pair(v1[1],v1[2]));
      std::pair<float,std::pair<float,float> > vertex2=std::make_pair(v2[0],std::make_pair(v2[1],v2[2]));
      std::pair<float,std::pair<float,float> > vertex3=std::make_pair(v3[0],std::make_pair(v3[1],v3[2]));
      if (vertices_tmp.find(vertex1)==vertices_tmp.end()) {vertices.push_back(v1);vertices_tmp[vertex1]=ind1=ind++;} else ind1=vertices_tmp[vertex1];
      if (vertices_tmp.find(vertex2)==vertices_tmp.end()) {vertices.push_back(v2);vertices_tmp[vertex2]=ind2=ind++;} else ind2=vertices_tmp[vertex2];
      if (vertices_tmp.find(vertex3)==vertices_tmp.end()) {vertices.push_back(v3);vertices_tmp[vertex3]=ind3=ind++;} else ind3=vertices_tmp[vertex3];

      if ( (Vector3D_f(v2-v1)^Vector3D_f(v3-v1))*normal < 0.0) std::swap(ind1,ind2);
      MeshTriangle tr;
      tr.m_vertices[0]=ind1;tr.m_vertices[1]=ind2;tr.m_vertices[2]=ind3;
      if (ind1!=ind2 && ind1!=ind3 && ind2!=ind3) triangles.push_back(tr);
      }

    }
  fclose(fp);

  std::vector<intrusive_ptr<const Primitive> > primitives;

  /////// Add car primitive ///
  
  mp_mesh = intrusive_ptr<TriangleMesh>( new TriangleMesh(vertices, triangles, true) );

  intrusive_ptr<Texture<Spectrum_d> > p_reflectance;
  //intrusive_ptr<Texture<Spectrum_d> > p_reflectance(new ConstantTexture<Spectrum_d> (Spectrum_d(212,175,55)/255.0*0.8));
  intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.04));
  //intrusive_ptr<Material> p_material(new Matte(p_text, p_sigma));

  intrusive_ptr<Texture<Spectrum_d> > p_refr(new ConstantTexture<Spectrum_d>(Spectrum_d(0.41,1.15,1.18)));
  intrusive_ptr<Texture<Spectrum_d> > p_a(new ConstantTexture<Spectrum_d>(Spectrum_d(4.2,2.66,2.5)));
  intrusive_ptr<Material> p_material(new Metal(p_refr, p_a, p_sigma));

  intrusive_ptr<Primitive> p_primitive(new Primitive(mp_mesh, p_material));
  primitives.push_back(p_primitive);

   /////// Add sphere primitive ///
/*
  Sphere s;
  s.SetParameter("Center","400 -1000 400");
  s.SetParameter("Radius","350");
  s.SetParameter("Subdivisions","9");
  mp_mesh = s.BuildMesh();

  intrusive_ptr<Texture<Spectrum_d> > p_reflectance;
  intrusive_ptr<Texture<double> > p_sigma(new ConstantTexture<double> (0.17));
  //intrusive_ptr<Material> p_material(new Matte(p_reflectance, p_sigma));
  intrusive_ptr<Material> p_material(new Metal(p_text, p_sigma));

  //p_reflectance.reset(new ConstantTexture<Spectrum_d>(Spectrum_d(1.0)));
  //intrusive_ptr<Material> p_material(new Transparent(p_reflectance, p_reflectance, 1.4));

  intrusive_ptr<Primitive> p_primitive(new Primitive(mp_mesh, p_material));
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

/*
    {
  intrusive_ptr<DeltaLightSource> p_source( new PointLight(Point3D_d(1300, -2200, 3000), Spectrum_d(4000000000.0)) );
  lights.m_delta_light_sources.push_back(p_source);
    }
    {
  intrusive_ptr<DeltaLightSource> p_source( new PointLight(Point3D_d(-500, -2200, 3000), Spectrum_d(4000000000.0)) );
  lights.m_delta_light_sources.push_back(p_source);
    }
*/
  mp_scene.reset(new Scene(primitives, lights));
  }

// 73 291
inline void TestTracer::RenderImage(HWND &g_hWnd, HDC &g_memDC)
  {
  FilmFilter *filter = new BoxFilter(0.5,0.5);
  InteractiveFilm *film = new InteractiveFilm(GetImageWidth(), GetImageHeight(), intrusive_ptr<FilmFilter>(filter));
  //film->SetCropWindow(Point2D_i(0,106-27),Point2D_i(800,200-27));

  Point2D_i window_begin,window_end;
  film->GetSamplingExtent(window_begin, window_end);

  // this is for budha
  // Vector3D_d direction = Vector3D_d(0,-0.5,-1).Normalized();
  //intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(Point3D_d(0.0,0.26,0.17)+direction*0.08,direction,Vector3D_d(0,1,0)), intrusive_ptr<Film>(film), 0.000, 0.087, 1.3) );

  Vector3D_d direction = Vector3D_d(-0.4,0.5,-0.3).Normalized();
  intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(Point3D_d(900,-1600,900),direction,Vector3D_d(0,0,1)), intrusive_ptr<Film>(film), 0.000, 1000, 1.3) );

  //Vector3D_d direction = Vector3D_d(0.0,0.5,-0.2).Normalized();
  //intrusive_ptr<Camera> p_camera( new PerspectiveCamera( MakeLookAt(Point3D_d(500,-1600,800),direction,Vector3D_d(0,0,1)), intrusive_ptr<Film>(film), 0.000, 1000, 1.3) );

  intrusive_ptr<ImagePixelsOrder> pixel_order(new UniformImagePixelsOrder);

  intrusive_ptr<Sampler> p_sampler( new StratifiedSampler(window_begin, window_end, 3, 3, pixel_order) );

  intrusive_ptr<SamplerBasedRenderer> p_renderer( new SamplerBasedRenderer(mp_scene, p_sampler) );

  intrusive_ptr<LightsSamplingStrategy> p_sampling_strategy( new IrradianceLightsSampling(mp_scene->GetLightSources()) );
  intrusive_ptr<DirectLightingIntegrator> p_direct_int( new DirectLightingIntegrator(p_renderer, 128, 32, p_sampling_strategy) );
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