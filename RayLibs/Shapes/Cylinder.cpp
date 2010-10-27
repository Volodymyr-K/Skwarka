#include "Cylinder.h"
#include <cmath>
#include <vector>
#include <Math/Constants.h>

Cylinder::Cylinder()
  {
  m_subdivisions = 360;
  m_max_phi = 2.0*M_PI;
  m_invert_orientation = false;
  }

void Cylinder::SetSubdivisions(size_t i_subdivisions)
  {
  ASSERT(i_subdivisions>0);
  m_subdivisions = std::max((size_t)1,i_subdivisions);
  }

void Cylinder::SetTransformation(const Transform &i_transform)
  {
  m_transform = i_transform;
  }

void Cylinder::SetMaxPhi(double i_max_phi)
  {
  m_max_phi = std::max(0.0,std::min(2.0*M_PI,i_max_phi));
  }

void Cylinder::SetInvertOrientation(bool i_invert_orientation)
  {
  m_invert_orientation = i_invert_orientation;
  }

intrusive_ptr<TriangleMesh> Cylinder::BuildMesh()
  {
  // Vector holding all the mesh vertices.
  std::vector<Point3D_f> vertices;

  // Vector holding all the mesh triangles.
  std::vector<MeshTriangle> triangles;

  std::vector<Vector3D_f> normals, tangents;

  // If transformation inverts geometric normals we need to invert shading normals to be consistent.
  const double normal_invert = m_transform.InvertsOrientation() ? -1.0 : 1.0;

  double phi = 0.0;
  const double d_phi = m_max_phi / m_subdivisions;
  for(size_t i=0;i<m_subdivisions+1;++i)
    {
    double cos_phi = cos(phi);
    double sin_phi = sin(phi);
    vertices.push_back( Convert<float>(m_transform(Point3D_d(cos_phi,sin_phi,0.0))) );
    vertices.push_back( Convert<float>(m_transform(Point3D_d(cos_phi,sin_phi,1.0))) );

    Vector3D_f normal =  Convert<float>(normal_invert*m_transform.TransformNormal(Vector3D_d(cos_phi,sin_phi,0.0)));
    Vector3D_f tangent =  Convert<float>(m_transform(Vector3D_d(-sin_phi,cos_phi,0.0)).Normalized());
    normals.push_back(normal);
    normals.push_back(normal);
    tangents.push_back(tangent);
    tangents.push_back(tangent);

    phi += d_phi;
    }

  for(size_t i=0;i<m_subdivisions;++i)
    {
    MeshTriangle t1 = MeshTriangle(2*i+1,2*i,2*i+2);
    MeshTriangle t2 = MeshTriangle(2*i+2,2*i+3,2*i+1);

    t1.m_uvs[0] = Point2D_f((float)i/m_subdivisions, 1.f);
    t1.m_uvs[1] = Point2D_f((float)i/m_subdivisions, 0.f);
    t1.m_uvs[2] = Point2D_f((float)(i+1)/m_subdivisions, 0.f);

    t2.m_uvs[0] = Point2D_f((float)(i+1)/m_subdivisions, 0.f);
    t2.m_uvs[1] = Point2D_f((float)(i+1)/m_subdivisions, 1.f);
    t2.m_uvs[2] = Point2D_f((float)i/m_subdivisions, 1.f);

    triangles.push_back(t1);
    triangles.push_back(t2);
    }

  bool invert_orientation = m_invert_orientation;
  if (m_transform.InvertsOrientation()) invert_orientation = !invert_orientation;

  // The cylinder is supposed to be smooth by definition so we use interpolated normals.
  TriangleMesh *p_mesh = new TriangleMesh(vertices, triangles, normals, tangents, true, invert_orientation);
  return intrusive_ptr<TriangleMesh>(p_mesh);
  }