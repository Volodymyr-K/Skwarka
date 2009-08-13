#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <Common\Common.h>
#include <Math\Geometry.h>
#include <vector>
#include "DifferentialGeometry.h"

struct TopologyInfo
  {
  size_t m_number_of_patches;
  bool m_manifold;
  bool m_solid;
  };

struct MeshTriangle
  {
  MeshTriangle();
  MeshTriangle(size_t i_v1, size_t i_v2, size_t i_v3);
  size_t m_vertices[3];
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

    void ComputeDifferentialGeometry(size_t i_triangle_index, const RayDifferential &i_ray, DifferentialGeometry &o_dg) const;

    TopologyInfo GetTopologyInfo() const;

  private:
    // INTERNAL TYPE
    struct ConnectivityData;

  private:
    // not implemented
    TriangleMesh();
    TriangleMesh(const TriangleMesh&);
    TriangleMesh &operator=(const TriangleMesh&);

    void _ComputeShadingNormals(const ConnectivityData &i_connectivity);
    void _ComputeTopologyInfo(const ConnectivityData &i_connectivity);
    bool _ConsistentlyOriented(size_t i_triangle_index1, size_t i_triangle_index2) const;

    void _GetUVs(const MeshTriangle &i_triangle, float o_uv[3][2]) const;
    bool _ComputeIntersectionPoint(const MeshTriangle &i_triangle, const Point3Dd &i_origin, const Vector3Dd &i_direction, double &o_b1, double &o_b2, double &o_t) const;

    void _BuildConnectivityData(ConnectivityData &o_connectivity);

  private:
    std::vector<Point3Df> m_vertices;
    std::vector<MeshTriangle> m_triangles;
    std::vector<Vector3Df> m_triangle_normals;
    std::vector<Vector3Df> m_shading_normals;
    std::vector<float> m_uv_parameterization;

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

inline TopologyInfo TriangleMesh::GetTopologyInfo() const
  {
  return m_topology_info;
  }

#endif // TRIANGLEMESH_H