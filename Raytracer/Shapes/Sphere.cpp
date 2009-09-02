#include "Sphere.h"
#include <cmath>
#include <map>
#include <utility>
#include "Math\Constants.h"

Sphere::Parameters::Parameters()
  {
  // Set the default parameter values.
  m_subdivisions=3;
  }

Sphere::Sphere()
  {
  }

bool Sphere::_GetParameters()
  {
  _ClearErrors();

  _GetParameter("Center", m_params.m_center, true);

  if (_GetParameter("Radius", m_params.m_radius, true) && m_params.m_radius<=0.f)
    _AddError("Radius can not be negative.");

  if (_GetParameter("Subdivisions", m_params.m_subdivisions, false) && m_params.m_subdivisions<0)
    _AddError("Number of subdivisions can not be negative.");

  return _ErrorsExist()==false;
  }

shared_ptr<TriangleMesh> Sphere::BuildMesh()
  {
  if (_GetParameters()==false)
    return shared_ptr<TriangleMesh>((TriangleMesh*)NULL);

  // Vector holding all the mesh vertices.
  std::vector<Point3D_f> vertices(4);

  // Vector holding all the mesh triangles.
  std::vector<MeshTriangle> triangles(4);

  // When subdividing a triangle into four new ones, the original triangle is not removed form the vector, instead it is marked as deleted.
  // This vector tracks what triangles have been marked as deleted so far.
  std::vector<bool> deleted(4, false);

  // Constructs the initial tetrahedron manually.
  float base_radius = sqrtf(8.f)/3.f;
  vertices[0]=Point3D_f(0.f, 0.f, 1.f);
  vertices[1]=Point3D_f(base_radius,  0.f, -1.f/3.f);
  vertices[2]=Point3D_f(base_radius*cos(2.f*M_PI_3), base_radius*sin(2.f*M_PI_3), -1.f/3.f);
  vertices[3]=Point3D_f(base_radius*cos(2.f*M_PI_3), -base_radius*sin(2.f*M_PI_3), -1.f/3.f);

  triangles[0]=MeshTriangle(1,3,2);
  triangles[1]=MeshTriangle(1,0,3);
  triangles[2]=MeshTriangle(1,2,0);
  triangles[3]=MeshTriangle(0,2,3);

  // Each edge is subdivided by splitting it by a vertex.
  // Not to add separate splitting vertices for the adjacent triangles, this map keeps track of what edges have been split by what vertices.
  std::map<std::pair<size_t,size_t>,size_t> edges_to_vertices;

  // Iteratively subdivide the mesh.
  for (int i=0;i<m_params.m_subdivisions;++i)
    {
    size_t original_size = triangles.size();
    for (size_t j=0;j<original_size;++j)
      if (deleted[j]==false)
        {
        size_t indices[3]={
          triangles[j].m_vertices[0],
          triangles[j].m_vertices[1],
          triangles[j].m_vertices[2]};

        // The original triangle is marked as deleted.
        deleted[j]=true;
        Point3D_f v01 = (vertices[indices[0]]+vertices[indices[1]])*0.5f;
        Point3D_f v12 = (vertices[indices[1]]+vertices[indices[2]])*0.5f;
        Point3D_f v20 = (vertices[indices[2]]+vertices[indices[0]])*0.5f;

        v01 /= Vector3D_f(v01).Length();
        v12 /= Vector3D_f(v12).Length();
        v20 /= Vector3D_f(v20).Length();

        // Split the edges by inserting new vertices.
        // If an edge has already been split (while processing an adjacent triangle) the existing splitting vertex is reused.
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

        // Insert new triangles.
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

  // Prepare a list of non-deleted triangles.
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

    vertices[i]=vertices[i]*m_params.m_radius+m_params.m_center;
    }

  TriangleMesh *p_mesh = new TriangleMesh(vertices, triangles_cleaned);
  p_mesh->SetUVParameterization(uv_parameterization);

  // The sphere is supposed to be smooth by definition so we use interpolated normals.
  p_mesh->SetUseShadingNormals(true);

  return shared_ptr<TriangleMesh>(p_mesh);
  }

Sphere::~Sphere()
  {
  }