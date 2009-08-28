#ifndef TESTTRACER_H
#define TESTTRACER_H

#include <Common\Common.h>
#include <Math\Geometry.h>
#include <Core\TriangleMesh.h>
#include <Shapes\Sphere.h>
#include <Core\TriangleTree.h>
#include "stdafx.h"
#include <WinBase.h>
#include <cstdio>
#include <Core\Spectrum.h>
#include <Math\Util.h>
#include <Core\Camera.h>

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
  while(true)
    {
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

  fp=fopen("triangles.txt","r");
  while(true)
    {
    size_t v1,v2,v3;
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
//  Camera *cam =  new PerspectiveCamera( MakeLookAt(Point3D_d(0.0,-1.1,0.0),Vector3D_d(0,1,0),Vector3D_d(0,0,1)), shared_ptr<Film>(film), 0.0, 2.0, 2.0);
  Vector3D_d direction = Vector3D_d(0,-0.5,-1).Normalized();
  Camera *cam =  new PerspectiveCamera( MakeLookAt(Point3D_d(0.0,0.25,0.17),direction,Vector3D_d(0,1,0)), shared_ptr<Film>(film), 0.03, 0.165, 2.0);


  for(int y=0;y<GetImageHeight();++y)
    for(int x=0;x<GetImageWidth();++x)
      {
      double r=0,g=0,b=0;
      int rays_num=1;

      for(int ray_index=0;ray_index<rays_num;++ray_index)
        {
        Point2D_d lens_point(rand()/(double)RAND_MAX,rand()/(double)RAND_MAX);

        Ray ray;
        cam->GenerateRay(Point2D_d(x,y),lens_point,ray);

        DifferentialGeometry dg;
        bool hit = ComputeDG(ray.m_origin,ray.m_direction,dg);

        unsigned int pixel_index = (y*GetImageWidth()+x)*4;
        Byte* pixel = m_image;
        if (hit==false)
          r+=255.0;
        else
          {
          double color = fabs(-(dg.m_shading_normal*direction));
          r+=color*255.0;
          g+=color*255.0;
          b+=color*255.0;
          }
        }

      unsigned int pixel_index = (y*GetImageWidth()+x)*4;
      Byte* pixel = m_image;
      pixel[pixel_index+0] = Byte(r/double(rays_num));
      pixel[pixel_index+1] = Byte(g/double(rays_num));
      pixel[pixel_index+2] = Byte(b/double(rays_num));
      }

  }

inline bool TestTracer::ComputeDG(Point3D_d c, Vector3D_d dir, DifferentialGeometry &dg)
  {
  IntersectResult result = mp_tree->Intersect(Ray(c,dir));

  if (result.m_intersection_found==false)
    return false;

  RayDifferential rd;
  rd.m_base_ray.m_origin=c;
  rd.m_base_ray.m_direction=dir;
  result.mp_mesh->ComputeDifferentialGeometry(result.m_triangle_index,rd,dg);
  return true;

  /*
  double min_t=DBL_INF;
  int index=-1;

  for(int i=0;i<int(mp_mesh->GetNumberOfTriangles());++i)
    {
    MeshTriangle triangle = mp_mesh->GetTriangle(i);
    const Point3D_d &v0 = mp_mesh->GetVertex(triangle.m_vertices[0]);
    const Point3D_d &v1 = mp_mesh->GetVertex(triangle.m_vertices[1]);
    const Point3D_d &v2 = mp_mesh->GetVertex(triangle.m_vertices[2]);

    Vector3D_d e1 = Vector3D_d(v1 - v0);
    Vector3D_d e2 = Vector3D_d(v2 - v0);
    Vector3D_d s1 = dir^e2;
    double divisor = s1*e1;
    if (divisor == 0.0)
      continue;
    double invDivisor = 1.0 / divisor;

    // Compute first barycentric coordinate
    Vector3D_d d = Vector3D_d(c - v0);
    double b1 = (d*s1) * invDivisor;
    if(b1 < 0.0 || b1 > 1.0)
      continue;

    // Compute second barycentric coordinate
    Vector3D_d s2 = d^e1;
    double b2 = (dir*s2) * invDivisor;
    if(b2 < 0.0 || b1 + b2 > 1.0)
      continue;

    // Compute t to intersection point
    double t = (e2*s2) * invDivisor;
    if (t>=0.0 && t<min_t)
      {
      min_t=t;
      index=i;
      }
    }

  if (index==-1)
    return false;

  RayDifferential rd;
  rd.m_base_ray.m_origin=c;
  rd.m_base_ray.m_direction=dir;
  mp_mesh->ComputeDifferentialGeometry(index,rd,dg);
  return true;
  */
  }

#endif // TESTTRACER_H