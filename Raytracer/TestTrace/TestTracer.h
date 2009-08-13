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
  bool ComputeDG(Point3Dd c, Vector3Dd dir, DifferentialGeometry &dg);

  public:
  Byte*   m_image;
  int     m_imageWidth;
  int     m_imageHeight;

  shared_ptr<TriangleMesh> mp_mesh;
  TriangleTree *mp_tree;
  };

inline void TestTracer::LoadMesh()
  {
/*
  Sphere s;
  s.SetParameter("Center","0 0 0");
  s.SetParameter("Radius","0.4");
  s.SetParameter("Subdivisions","7");
  mp_mesh = s.BuildMesh();
*/

  std::vector<Point3Df> vertices;
  std::vector<MeshTriangle> triangles;
  std::vector<float> uv_parameterization;

  #pragma warning(disable : 4996)
  FILE *fp=fopen("vertices.txt","r");
  while(true)
    {
    float x,y,z;
    int read = fscanf(fp,"%f %f %f",&x,&y,&z);
    if (read<=0) break;
    vertices.push_back(Point3Df(x,y,z));
    }
  fclose(fp);

  fp=fopen("triangles.txt","r");
  while(true)
    {
    int v1,v2,v3;
    int read = fscanf(fp,"%d %d %d",&v1,&v2,&v3);
    if (read<=0) break;

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
  //Point3Dd c(0.0,-1.10,0.0);
  Point3Dd c(0.0,-0.0,-0.2);
/*
  long tick1 = GetTickCount();
  double inv1 = 1.0/double(GetImageHeight());
  double inv2 = 1.0/double(GetImageWidth());
  int k=0;
  for(int j=0;j<5;++j)
  for(int y=0;y<GetImageHeight();++y)
    for(int x=0;x<GetImageWidth();++x)
      {
      Vector3Dd dir = Vector3Dd(0,1,0)+
        double(y-GetImageHeight()*0.5)*inv1*Vector3Dd(0,0,-1)+
        double(x-GetImageWidth()*0.5)*inv2*Vector3Dd(1,0,0);

      Vector3Df fff = Convert<float>(dir);
      IntersectResult result = mp_tree->Intersect(Ray(c,dir));
      if (result.m_intersection_found)
        ++k;
      }

  long tick2 = GetTickCount();
  printf("%d\n", (tick2-tick1));

  Log::Info("%d",k);
*/
  bool f = false;
  for(int y=0;y<GetImageHeight();++y)
    for(int x=0;x<GetImageWidth();++x)
      {
      Vector3Dd dir = Vector3Dd(0,1,1)+
        double(y-GetImageHeight()/2.0)/double(GetImageHeight())*Vector3Dd(0,0,-1)+
        double(x-GetImageWidth()/2.0)/double(GetImageWidth())*Vector3Dd(1,0,0);        
      dir.Normalize();

      DifferentialGeometry dg;
      bool hit = ComputeDG(c,dir,dg);

      unsigned int pixel_index = (y*GetImageWidth()+x)*4;
      Byte* pixel = m_image;
      if (hit==false)
        {
        pixel[pixel_index+0] = 255;
        pixel[pixel_index+1] = 0;
        pixel[pixel_index+2] = 0;
        }
      else
        {
        f = true;
        double color = dg.m_shading_normal*Vector3Dd(0,-1,0);
        pixel[pixel_index+0] = Byte(color*255.0);
        pixel[pixel_index+1] = Byte(color*255.0);
        pixel[pixel_index+2] = Byte(color*255.0);
        }

      }

  }

inline bool TestTracer::ComputeDG(Point3Dd c, Vector3Dd dir, DifferentialGeometry &dg)
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
    const Point3Dd &v0 = mp_mesh->GetVertex(triangle.m_vertices[0]);
    const Point3Dd &v1 = mp_mesh->GetVertex(triangle.m_vertices[1]);
    const Point3Dd &v2 = mp_mesh->GetVertex(triangle.m_vertices[2]);

    Vector3Dd e1 = Vector3Dd(v1 - v0);
    Vector3Dd e2 = Vector3Dd(v2 - v0);
    Vector3Dd s1 = dir^e2;
    double divisor = s1*e1;
    if (divisor == 0.0)
      continue;
    double invDivisor = 1.0 / divisor;

    // Compute first barycentric coordinate
    Vector3Dd d = Vector3Dd(c - v0);
    double b1 = (d*s1) * invDivisor;
    if(b1 < 0.0 || b1 > 1.0)
      continue;

    // Compute second barycentric coordinate
    Vector3Dd s2 = d^e1;
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