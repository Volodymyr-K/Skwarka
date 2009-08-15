#include "Sphere.h"
#include <cmath>
#include <map>
#include <utility>
#include "Math\Constants.h"

Sphere::Sphere()
  {
  }

shared_ptr<TriangleMesh> Sphere::BuildMesh()
  {
  Point3D_f center;
  float radius;
  unsigned int subdivisions = DEFAULT_SUBDIVISIONS;

  GetParameter("Center", center, true);
  GetParameter("Radius", radius, true);
  GetParameter("Subdivisions", subdivisions, false);

  std::vector<Point3D_f> vertices(4);
  std::vector<MeshTriangle> triangles(4);
  std::vector<bool> deleted(4, false);

  float base_radius = sqrtf(8.f)/3.f;
  vertices[0]=Point3D_f(0.f, 0.f, 1.f);
  vertices[1]=Point3D_f(base_radius,  0.f, -1.f/3.f);
  vertices[2]=Point3D_f(base_radius*cos(2.f*M_PI_3), base_radius*sin(2.f*M_PI_3), -1.f/3.f);
  vertices[3]=Point3D_f(base_radius*cos(2.f*M_PI_3), -base_radius*sin(2.f*M_PI_3), -1.f/3.f);

  triangles[0]=MeshTriangle(1,3,2);
  triangles[1]=MeshTriangle(1,0,3);
  triangles[2]=MeshTriangle(1,2,0);
  triangles[3]=MeshTriangle(0,2,3);

  std::map<std::pair<size_t,size_t>,size_t> edges_to_vertices;
  for (unsigned int i=0;i<subdivisions;++i)
    {
    size_t original_size = triangles.size();
    for (size_t j=0;j<original_size;++j)
      if (deleted[j]==false)
        {
        size_t indices[3]={
          triangles[j].m_vertices[0],
          triangles[j].m_vertices[1],
          triangles[j].m_vertices[2]};

        deleted[j]=true;
        Point3D_f v01 = (vertices[indices[0]]+vertices[indices[1]])*0.5f;
        Point3D_f v12 = (vertices[indices[1]]+vertices[indices[2]])*0.5f;
        Point3D_f v20 = (vertices[indices[2]]+vertices[indices[0]])*0.5f;

        v01 /= Vector3D_f(v01).Length();
        v12 /= Vector3D_f(v12).Length();
        v20 /= Vector3D_f(v20).Length();

        size_t new_vertices[3];
        if (edges_to_vertices.find(std::make_pair(indices[0],indices[1]))==edges_to_vertices.end())
          {
          vertices.push_back(v01);
          new_vertices[0]=vertices.size()-1;
          edges_to_vertices[std::make_pair(indices[0],indices[1])]=new_vertices[0];
          edges_to_vertices[std::make_pair(indices[1],indices[0])]=new_vertices[0];
          }
        else
          new_vertices[0]=edges_to_vertices[std::make_pair(indices[0],indices[1])];

        if (edges_to_vertices.find(std::make_pair(indices[1],indices[2]))==edges_to_vertices.end())
          {
          vertices.push_back(v12);
          new_vertices[1]=vertices.size()-1;
          edges_to_vertices[std::make_pair(indices[1],indices[2])]=new_vertices[1];
          edges_to_vertices[std::make_pair(indices[2],indices[1])]=new_vertices[1];
          }
        else
          new_vertices[1]=edges_to_vertices[std::make_pair(indices[1],indices[2])];

        if (edges_to_vertices.find(std::make_pair(indices[2],indices[0]))==edges_to_vertices.end())
          {
          vertices.push_back(v20);
          new_vertices[2]=vertices.size()-1;
          edges_to_vertices[std::make_pair(indices[2],indices[0])]=new_vertices[2];
          edges_to_vertices[std::make_pair(indices[0],indices[2])]=new_vertices[2];
          }
        else
          new_vertices[2]=edges_to_vertices[std::make_pair(indices[2],indices[0])];

        triangles.push_back(MeshTriangle(triangles[j].m_vertices[0],new_vertices[0],new_vertices[2]));
        triangles.push_back(MeshTriangle(triangles[j].m_vertices[1],new_vertices[1],new_vertices[0]));
        triangles.push_back(MeshTriangle(triangles[j].m_vertices[2],new_vertices[2],new_vertices[1]));
        triangles.push_back(MeshTriangle(new_vertices[0],new_vertices[1],new_vertices[2]));

        deleted.push_back(false);
        deleted.push_back(false);
        deleted.push_back(false);
        deleted.push_back(false);
        }
    }

  std::vector<MeshTriangle> triangles_cleaned;
  for (size_t i=0;i<triangles.size();++i)
    if (deleted[i]==false)
      triangles_cleaned.push_back(triangles[i]);

  std::vector<Point2D_f> uv_parameterization(vertices.size());

  for (size_t i=0;i<vertices.size();++i)
    {
    float phi = atan2(vertices[i][1],vertices[i][0]);
    if (phi < 0.0) phi+=2.0*M_PI;
    float theta = acos(vertices[i][2]);

    uv_parameterization[i]=Point2D_f(phi*INV_2PI, theta*INV_PI);

    vertices[i]=vertices[i]*radius+center;
    }

  TriangleMesh *p_mesh = new TriangleMesh(vertices, triangles_cleaned);
  p_mesh->SetUVParameterization(uv_parameterization);
  p_mesh->SetUseShadingNormals(true);
  return shared_ptr<TriangleMesh>(p_mesh);
  }

Sphere::~Sphere()
  {
  }