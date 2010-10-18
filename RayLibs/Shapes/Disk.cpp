#include "Disk.h"
#include <cmath>
#include <vector>
#include <map>
#include <Math/Constants.h>

Disk::Disk()
  {
  m_subdivisions = 360;
  m_max_phi = 2.0*M_PI;
  m_inner_radius = 0.0;
  }

void Disk::SetSubdivisions(size_t i_subdivisions)
  {
  ASSERT(i_subdivisions>=3);
  m_subdivisions = std::max((size_t)3,i_subdivisions);
  }

void Disk::SetInnerRadius(double i_inner_radius)
  {
  ASSERT(i_inner_radius>=0.0 && i_inner_radius<1.0);
  m_inner_radius = std::max(0.0,std::min(1.0,i_inner_radius));
  }

void Disk::SetMaxPhi(double i_max_phi)
  {
  m_max_phi = std::max(0.0,std::min(2.0*M_PI,i_max_phi));
  }

void Disk::SetTransformation(const Transform &i_transform)
  {
  m_transform = i_transform;
  }

intrusive_ptr<TriangleMesh> Disk::BuildMesh()
  {
  if (m_inner_radius==0.0)
    return _BuildFullMesh();
  else
    return _BuildPartialMesh();
  }

intrusive_ptr<TriangleMesh> Disk::_BuildFullMesh()
  {
  // Vector holding all the mesh vertices.
  std::vector<Point3D_f> vertices;

  // Vector holding all the mesh triangles.
  std::vector<MeshTriangle> triangles;

  std::vector<Vector3D_f> normals, tangents;

  // If transformation inverts geometric normals we need to invert shading normals to be consistent.
  const double normal_invert = m_transform.InvertsOrientation() ? -1.0 : 1.0;
  Vector3D_f normal = Convert<float>(normal_invert*m_transform.TransformNormal(Vector3D_d(0.0,0.0,1.0)));
  const double d_phi = m_max_phi/m_subdivisions;

  vertices.push_back( Convert<float>(m_transform(Point3D_d(0.0,0.0,0.0))) );
  normals.push_back(normal);
  tangents.push_back(Vector3D_f(0.0,0.0,0.0));

  double phi = 0.0;
  for(size_t i=0;i<m_subdivisions+1;++i)
    {
    double cos_phi = cos(phi);
    double sin_phi = sin(phi);
    vertices.push_back( Convert<float>(m_transform(Point3D_d(cos_phi,sin_phi,0.0))) );

    Vector3D_f tangent =  Convert<float>(m_transform(Vector3D_d(-sin_phi,cos_phi,0.0)).Normalized());

    normals.push_back(normal);
    tangents.push_back(tangent);

    phi += d_phi;
    }

  for(size_t i=0;i<m_subdivisions;++i)
    {
    MeshTriangle t1 = MeshTriangle(0,i+1,i+2);

    t1.m_uvs[0] = Point2D_f(0.f, 0.f);
    t1.m_uvs[1] = Point2D_f((float)i/m_subdivisions, 1.f);
    t1.m_uvs[2] = Point2D_f((float)(i+1)/m_subdivisions, 1.f);

    triangles.push_back(t1);
    }

  TriangleMesh *p_mesh = new TriangleMesh(vertices, triangles, normals, tangents, false, m_transform.InvertsOrientation());
  return intrusive_ptr<TriangleMesh>(p_mesh);
  }

intrusive_ptr<TriangleMesh> Disk::_BuildPartialMesh()
  {
  // Vector holding all the mesh vertices.
  std::vector<Point3D_f> vertices;

  // Vector holding all the mesh triangles.
  std::vector<MeshTriangle> triangles;

  std::vector<Vector3D_f> normals, tangents;

  // If transformation inverts geometric normals we need to invert shading normals to be consistent.
  const double normal_invert = m_transform.InvertsOrientation() ? -1.0 : 1.0;
  Vector3D_f normal = Convert<float>(normal_invert*m_transform.TransformNormal(Vector3D_d(0.0,0.0,1.0)));
  const double d_phi = m_max_phi/m_subdivisions;

  double phi = 0.0;
  for(size_t i=0;i<m_subdivisions+1;++i)
    {
    double cos_phi = cos(phi);
    double sin_phi = sin(phi);
    vertices.push_back( Convert<float>(m_transform(Point3D_d(m_inner_radius*cos_phi,m_inner_radius*sin_phi,0.0))) );
    vertices.push_back( Convert<float>(m_transform(Point3D_d(cos_phi,sin_phi,0.0))) );

    Vector3D_f tangent =  Convert<float>(m_transform(Vector3D_d(-sin_phi,cos_phi,0.0)).Normalized());

    normals.push_back(normal);
    normals.push_back(normal);
    tangents.push_back(tangent);
    tangents.push_back(tangent);

    phi += d_phi;
    }

  for(size_t i=0;i<m_subdivisions;++i)
    {
    MeshTriangle t1 = MeshTriangle(2*i,2*i+1,2*i+3);
    MeshTriangle t2 = MeshTriangle(2*i+3,2*i+2,2*i);

    t1.m_uvs[0] = Point2D_f((float)i/m_subdivisions, 0.f);
    t1.m_uvs[1] = Point2D_f((float)i/m_subdivisions, 1.f);
    t1.m_uvs[2] = Point2D_f((float)(i+1)/m_subdivisions, 1.f);

    t2.m_uvs[0] = Point2D_f((float)(i+1)/m_subdivisions, 1.f);
    t2.m_uvs[1] = Point2D_f((float)(i+1)/m_subdivisions, 0.f);
    t2.m_uvs[2] = Point2D_f((float)i/m_subdivisions, 0.f);

    triangles.push_back(t1);
    triangles.push_back(t2);
    }

  TriangleMesh *p_mesh = new TriangleMesh(vertices, triangles, normals, tangents, false, m_transform.InvertsOrientation());
  return intrusive_ptr<TriangleMesh>(p_mesh);
  }