#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <Common\Common.h>
#include <Math\Geometry.h>
#include <vector>
#include "DifferentialGeometry.h"

struct MeshTriangle
  {
  MeshTriangle();
  MeshTriangle(size_t i_v1, size_t i_v2, size_t i_v3);
  size_t m_vertices[3];
  };

struct TopologyInfo
  {
  size_t m_number_of_patches;
  bool m_manifold;
  bool m_solid;
  };

class TriangleMesh
  {
  public:
    TriangleMesh(const std::vector<Point3Df> &i_vertices, const std::vector<MeshTriangle> &i_triangles, bool i_use_shading_normals=true);

    void SetUVParameterization(const std::vector<float> &i_uv_parameterization);
    void SetUseShadingNormals(bool i_use_shading_normals);

    size_t GetNumberOfVertices() const;
    size_t GetNumberOfTriangles() const;

    const Point3Df &GetVertex(size_t i_vertex_index) const;
    const MeshTriangle &GetTriangle(size_t i_triangle_index) const;
    const Vector3Df &GetTriangleNormal(size_t i_triangle_index) const;

    size_t GetNumberOfIncidentTriangles(size_t i_vertex_index) const;
    const MeshTriangle &GetIncidentTriangle(size_t i_vertex_index, size_t i_incident_triangle_index) const;
    size_t GetIncidentTriangleIndex(size_t i_vertex_index, size_t i_incident_triangle_index) const;

    void GetIncidentTriangles(size_t i_vertex_index1, size_t i_vertex_index2, std::vector<size_t> &o_triangles) const;

    void ComputeDifferentialGeometry(size_t i_triangle_index, const RayDifferential &i_ray, DifferentialGeometry &o_dg) const;

    TopologyInfo GetTopologyInfo() const;

  private:
    TriangleMesh(); // not implemented
    void _ComputeShadingNormals();
    void _ComputeShadingNormalsDerivatives();
    void _GetUVs(const MeshTriangle &i_triangle, float o_uv[3][2]) const;
    bool _ComputeIntersectionPoint(const MeshTriangle &i_triangle, const Point3Dd &i_origin, const Vector3Dd &i_direction, double &o_b1, double &o_b2, double &o_t) const;

    bool _ConsistentlyOriented(size_t i_triangle_index1, size_t i_triangle_index2) const;
    void _ComputeTopologyInfo();

  private:
    std::vector<Point3Df> m_vertices;
    std::vector<MeshTriangle> m_triangles;
    std::vector<Vector3Df> m_triangle_normals;
    std::vector<Vector3Df> m_shading_normals;
    std::vector<Vector3Df> m_dn_du, m_dn_dv;
    std::vector<float> m_uv_parameterization;

    std::vector<size_t> m_incident_triangles;
    std::vector<size_t> m_incident_triangles_index;

    bool m_use_shading_normals;

    TopologyInfo m_topology_info;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline size_t TriangleMesh::GetNumberOfVertices() const
  {
  return m_vertices.size();
  }

inline size_t TriangleMesh::GetNumberOfTriangles() const
  {
  return m_triangles.size();
  }

inline const Point3Df &TriangleMesh::GetVertex(size_t i_vertex_index) const
  {
  ASSERT(i_vertex_index < m_vertices.size());
  return m_vertices[i_vertex_index];
  }

inline const MeshTriangle &TriangleMesh::GetTriangle(size_t i_triangle_index) const
  {
  ASSERT(i_triangle_index < m_triangles.size());
  return m_triangles[i_triangle_index];
  }

inline const Vector3Df &TriangleMesh::GetTriangleNormal(size_t i_triangle_index) const
  {
  ASSERT(i_triangle_index < m_triangles.size());
  return m_triangle_normals[i_triangle_index];
  }

inline size_t TriangleMesh::GetNumberOfIncidentTriangles(size_t i_vertex_index) const
  {
  ASSERT(i_vertex_index < m_vertices.size());
  return m_incident_triangles_index[i_vertex_index+1]-m_incident_triangles_index[i_vertex_index];
  }

inline const MeshTriangle &TriangleMesh::GetIncidentTriangle(size_t i_vertex_index, size_t i_incident_triangle_index) const
  {
  ASSERT(i_vertex_index < m_vertices.size());
  ASSERT(i_incident_triangle_index < GetNumberOfIncidentTriangles(i_vertex_index));
  return m_triangles[m_incident_triangles[m_incident_triangles_index[i_vertex_index] + i_incident_triangle_index]];
  }

inline size_t TriangleMesh::GetIncidentTriangleIndex(size_t i_vertex_index, size_t i_incident_triangle_index) const
  {
  ASSERT(i_vertex_index < m_vertices.size());
  ASSERT(i_incident_triangle_index < GetNumberOfIncidentTriangles(i_vertex_index));
  return m_incident_triangles[m_incident_triangles_index[i_vertex_index] + i_incident_triangle_index];
  }

inline TopologyInfo TriangleMesh::GetTopologyInfo() const
  {
  return m_topology_info;
  }

#endif // TRIANGLEMESH_H