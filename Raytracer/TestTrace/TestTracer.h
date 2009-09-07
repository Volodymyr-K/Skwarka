#ifndef TESTTRACER_H
#define TESTTRACER_H

#pragma warning(disable : 4003)
#include <Common/Common.h>

#include "tbb/pipeline.h"
#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"

#include "tbb_pipeline_stuff.h"

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

  void RenderImage();
  bool ComputeDG(Point3D_d c, Vector3D_d dir, DifferentialGeometry &dg);

  public:
  Byte*   m_image;
  int     m_imageWidth;
  int     m_imageHeight;

  shared_ptr<TriangleMesh> mp_mesh;
  TriangleTree *mp_tree;
  };

inline void TestTracer::LoadMesh()
  {/*
  Sphere s;
  s.SetParameter("Center","0 0 0");
  s.SetParameter("Radius","0.4");
  s.SetParameter("Subdivisions","-3");
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
    //if(++ver>10000) break;
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
   // if(++tr>1000) break;
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

  mp_mesh = shared_ptr<TriangleMesh>( new TriangleMesh(vertices, triangles, false) );

  mp_tree = new TriangleTree();
  mp_tree->AddTriangleMesh(mp_mesh);
  mp_tree->BuildTree();
  }

// 73 291
inline void TestTracer::RenderImage()
  {
  //Point3D_d c(0.0,-0.0,-0.2);
/*
  long tick1 = GetTickCount();
  double inv1 = 1.0/double(GetImageHeight());
  double inv2 = 1.0/double(GetImageWidth());
  int k=0;
  for(int j=0;j<1;++j)
  for(int y=0;y<GetImageHeight();++y)
    for(int x=0;x<GetImageWidth();++x)
      {
      Vector3D_d dir = Vector3D_d(0,1,0)+
        double(y-GetImageHeight()*0.5)*inv1*Vector3D_d(0,0,-1)+
        double(x-GetImageWidth()*0.5)*inv2*Vector3D_d(1,0,0);

      Ray ray(c,dir);
      mp_tree->Intersect(ray);
      //IntersectResult result = mp_tree->Intersect(ray);
      //if (result.m_intersection_found)
      //  ++k;
      }

  long tick2 = GetTickCount();
  printf("%d\n", (tick2-tick1));

  Log::Info("%d",k);*/

  FilmFilter *filter = new BoxFilter(1.0,1.0);
  Film *film = new Film(GetImageWidth(), GetImageHeight(), shared_ptr<FilmFilter>(filter));
  //film->SetCropWindow(Point2D_d(0.3,0.0),Point2D_d(0.7,1.0));

  Point2D_i window_begin,window_end;
  film->GetSampleExtent(window_begin, window_end);

  Vector3D_d direction = Vector3D_d(0,-0.5,-1).Normalized();
  Camera *cam =  new PerspectiveCamera( MakeLookAt(Point3D_d(0.0,0.25,0.17),direction,Vector3D_d(0,1,0)), shared_ptr<Film>(film), 0.02, 0.165, 2.0);

  //Sampler *sampler = new RandomSampler(Point2D_i(0,0),Point2D_i(GetImageWidth(), GetImageHeight()),10);
  Sampler *sampler = new StratifiedSampler(window_begin, window_end, 4, 4, true);
  //sampler->AddSamplesSequence2D(100);

  tbb::task_scheduler_init init( 2 );

  tbb::pipeline pipeline;

  MyInputFilter input_filter(sampler);
  pipeline.add_filter( input_filter );

  // Create capitalization stage and add it to the pipeline
  MyTransformFilter transform_filter(cam, mp_tree); 
  pipeline.add_filter(transform_filter);

  // Create file-writing stage and add it to the pipeline
  MyOutputFilter output_filter(cam->GetFilm());
  pipeline.add_filter( output_filter );

  // Run the pipeline
  tbb::tick_count t0 = tbb::tick_count::now();
  pipeline.run( MyInputFilter::n_chunks );
  tbb::tick_count t1 = tbb::tick_count::now();

  printf("time = %g\n", (t1-t0).seconds());

  // Remove filters from pipeline before they are implicitly destroyed.
  pipeline.clear(); 

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(int y=0;y<GetImageHeight();++y)
    for(int x=0;x<GetImageWidth();++x)
      {
      float alfa;
      Spectrum_f sp;
      cam->GetFilm()->GetPixel(Point2D_i(x,y),sp,alfa);

      unsigned int pixel_index = (y*GetImageWidth()+x)*4;
      Byte* pixel = m_image;
      pixel[pixel_index+0] = Byte(std::min(sp[0],255.f));
      pixel[pixel_index+1] = Byte(std::min(sp[1],255.f));
      pixel[pixel_index+2] = Byte(std::min(sp[2],255.f));
      }
  }

#endif // TESTTRACER_H