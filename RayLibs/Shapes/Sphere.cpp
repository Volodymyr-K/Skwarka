#include "Sphere.h"
#include <cmath>
#include <vector>
#include <map>
#include <Math/Constants.h>

Sphere::Sphere()
  {
  m_subdivisions = 3;
  m_invert_orientation = false;
  }

void Sphere::SetSubdivisions(size_t i_subdivisions)
  {
  m_subdivisions = i_subdivisions;
  }

void Sphere::SetTransformation(const Transform &i_transform)
  {
  m_transform = i_transform;
  }

void Sphere::SetInvertOrientation(bool i_invert_orientation)
  {
  m_invert_orientation = i_invert_orientation;
  }

intrusive_ptr<TriangleMesh> Sphere::BuildMesh()
  {
  // Vector holding all the mesh vertices.
  std::vector<Point3D_d> vertices(4);

  // Vector holding all the mesh triangles.
  std::vector<MeshTriangle> triangles(4);

  // When subdividing a triangle into four new ones, the original triangle is not removed form the vector, instead it is marked as deleted.
  // This vector tracks what triangles have been marked as deleted so far.
  std::vector<bool> deleted(4, false);

  // Constructs the initial tetrahedron manually.
  double base_radius = sqrt(8.0)/3.0;
  vertices[0]=Point3D_d(0.0, 0.0, 1.0);
  vertices[1]=Point3D_d(base_radius,  0.0, -1.0/3.0);
  vertices[2]=Point3D_d(base_radius*cos(2.0*M_PI_3), base_radius*sin(2.0*M_PI_3), -1.0/3.0);
  vertices[3]=Point3D_d(base_radius*cos(2.0*M_PI_3), -base_radius*sin(2.0*M_PI_3), -1.0/3.0);

  triangles[0]=MeshTriangle(1,3,2);
  triangles[1]=MeshTriangle(1,0,3);
  triangles[2]=MeshTriangle(1,2,0);
  triangles[3]=MeshTriangle(0,2,3);

  // Each edge is subdivided by splitting it by a vertex.
  // Not to add separate splitting vertices for the adjacent triangles, this map keeps track of what edges have been split by what vertices.
  std::map<std::pair<size_t,size_t>,size_t> edges_to_vertices;

  // Iteratively subdivide the mesh.
  for (size_t i=0;i<m_subdivisions;++i)
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
        Point3D_d v01 = (vertices[indices[0]]+vertices[indices[1]])*0.5;
        Point3D_d v12 = (vertices[indices[1]]+vertices[indices[2]])*0.5;
        Point3D_d v20 = (vertices[indices[2]]+vertices[indices[0]])*0.5;

        v01 /= Vector3D_d(v01).Length();
        v12 /= Vector3D_d(v12).Length();
        v20 /= Vector3D_d(v20).Length();

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

  for (size_t i=0;i<triangles.size();++i)
    if (deleted[i]==false)
      {
      Point2D_f uvs[3];
      bool to_delete = true;
      for (unsigned char j=0;j<3;++j)
        {
        Point3D_d vertex = vertices[triangles[i].m_vertices[j]];

        double phi = atan2( vertex[1],vertex[0]);
        if (phi < 0.0) phi+=2.0*M_PI;
        double theta = acos(vertex[2]);

        uvs[j]=Point2D_f((float) (phi*INV_2PI), (float) (theta*INV_PI));
        }

      // For those triangles whose edges intersect the phi==0.0 line we need to adjust the UV coordinates.
      Vector3D_d normal =
        Vector3D_d(vertices[triangles[i].m_vertices[1]]-vertices[triangles[i].m_vertices[0]])^
        Vector3D_d(vertices[triangles[i].m_vertices[2]]-vertices[triangles[i].m_vertices[0]]);

      if (normal[0]>0.0 && ((uvs[0][0]>0.5 || uvs[1][0]>0.5 || uvs[2][0]>0.5) && (uvs[0][0]<0.5 || uvs[1][0]<0.5 || uvs[2][0]<0.5)))
        {
        if (uvs[0][0]<0.5) uvs[0][0] += 1.f;
        if (uvs[1][0]<0.5) uvs[1][0] += 1.f;
        if (uvs[2][0]<0.5) uvs[2][0] += 1.f;
        }

      triangles[i].m_uvs[0]=uvs[0];
      triangles[i].m_uvs[1]=uvs[1];
      triangles[i].m_uvs[2]=uvs[2];
      }

  // Prepare the list of non-deleted triangles.
  std::vector<MeshTriangle> triangles_cleaned;
  for (size_t i=0;i<triangles.size();++i)
    if (deleted[i]==false)
      triangles_cleaned.push_back(triangles[i]);

  // If transformation inverts geometric normals we need to invert shading normals to be consistent.
  const double normal_invert = m_transform.InvertsOrientation() ? -1.0 : 1.0;

  std::vector<Point3D_f> vertices_f(vertices.size());
  std::vector<Vector3D_f> normals(vertices.size()), tangents(vertices.size());
  for (size_t i=0;i<vertices.size();++i)
    {
    vertices_f[i] = Convert<float>( m_transform(vertices[i]) );
    normals[i] = Convert<float>( normal_invert*m_transform.TransformNormal(Vector3D_d(vertices[i])) );

    Vector3D_d tangent = Vector3D_d(0,0,1)^Vector3D_d(vertices[i]);
    if (tangent.Normalize()==false) tangent = Vector3D_d(1,0,0);
    tangents[i] = Convert<float>( m_transform(tangent).Normalized() );
    }

  bool invert_orientation = m_invert_orientation;
  if (m_transform.InvertsOrientation()) invert_orientation = !invert_orientation;
  
  // The sphere is supposed to be smooth by definition so we use interpolated normals.
  TriangleMesh *p_mesh = new TriangleMesh(vertices_f, triangles_cleaned, normals, tangents, true, invert_orientation);
  return intrusive_ptr<TriangleMesh>(p_mesh);
  }