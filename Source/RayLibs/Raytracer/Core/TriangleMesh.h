/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <vector>
#include "DifferentialGeometry.h"

/**
* The structure holds basic information about the mesh topology.
* @sa TriangleMesh
*/
struct TopologyInfo
  {
  /**
  * Number of connected patches.
  * A patch is a set of triangles so that for each pair of triangles in the set there is a chain of triangles from that set connecting them.
  */
  size_t m_number_of_patches;

  /**
  * If the value is true the mesh has manifold surface.
  * A surface is manifold if each edge has no more than two incident triangles and the triangles are consistently oriented with respect to each other.
  */
  bool m_manifold;

  /**
  * If the value is true the mesh is solid.
  * The mesh is sold if it is manifold, has no more than one patch and has no bad edges (i.e. edges with only one incident triangle).
  */
  bool m_solid;
  };

/**
* Represents triangle of the mesh.
* @sa TriangleMesh
*/
struct MeshTriangle
  {
  MeshTriangle();
  MeshTriangle(size_t i_v1, size_t i_v2, size_t i_v3);

  size_t m_vertices[3];
  Point2D_f m_uvs[3];
  };

/**
* Triangle mesh represented by a set of vertices and triangles.
* The mesh can also have UV parameterization of the surface. UV parameterization is defined by the UV coordinates of the each triangle vertices.
* The UV values are linearly interpolated inside the triangles.
* The class provides an option to interpolate the normals inside the triangles to imitate a smooth surface.
*
* The mesh is constant in the sense that once created the geometry and connectivity never changes.
*/
class TriangleMesh: public ReferenceCounted
  {
  public:

    /**
    * Creates TriangleMesh instance with the given set of triangles and vertices.
    * Shading normals will be interpolated from geometric normals.
    * Tangent vectors, which are used as "base" directions for anisotropic material properties, will be computed from UV coordinates (as tangents to U-isolines).
    * @param i_vertices Vertices coordinates.
    * @param i_triangles Mesh triangles.
    * @param i_use_shading_normals Defines if shading normals should be used for computing DifferentialGeometry. If false, geometric normals will be used instead.
    * @param i_invert_normals Defines if triangle normals need to be inverted.
    */
    TriangleMesh(const std::vector<Point3D_f> &i_vertices, const std::vector<MeshTriangle> &i_triangles,
      bool i_use_shading_normals=true, bool i_invert_normals=false);

    /**
    * Creates TriangleMesh instance with the given set of triangles and vertices and (optionally) shading and tangent normals.
    * If shading normals are not specified they will be interpolated from geometric normals.
    * Tangent vectors are used as "base" directions for anisotropic material properties. If the tangent vectors are not specified they will be
    * computed from UV coordinates (as tangents to U-isolines).
    * @param i_vertices Vertices coordinates.
    * @param i_triangles Mesh triangles.
    * @param i_shading_normals Vector of shading normals. Should be either empty or have the same size that i_vertices has. Vectors are not required to be normalized.
    * @param i_tangents Vector of tangent directions. Should be either empty or have the same size that i_vertices has. Vectors are not required to be normalized.
    * @param i_use_shading_normals Defines if shading normals should be used for computing DifferentialGeometry. If false, geometric normals will be used instead.
    * @param i_invert_normals Defines if triangle normals need to be inverted.
    */
    TriangleMesh(const std::vector<Point3D_f> &i_vertices, const std::vector<MeshTriangle> &i_triangles,
      const std::vector<Vector3D_f> &i_shading_normals, const std::vector<Vector3D_f> &i_tangents,
      bool i_use_shading_normals=true, bool i_invert_normals=false);

    /**
    * Sets whether the normals should be interpolated inside the triangles.
    */
    void SetUseShadingNormals(bool i_use_shading_normals);

    /**
    * Returns true if the normals are interpolated inside the triangles.
    */
    bool GetUseShadingNormals() const;

    /**
    * Sets whether to invert all triangle normals.
    */
    void SetInvertNormals(bool i_invert_normals);

    /**
    * Returns true if the triangle normals are inverted.
    */
    bool GetInvertNormals() const;

    size_t GetNumberOfVertices() const;

    size_t GetNumberOfTriangles() const;

    Point3D_f GetVertex(size_t i_vertex_index) const;

    MeshTriangle GetTriangle(size_t i_triangle_index) const;

    /**
    * Returns geometric normal of the triangle.
    */
    Vector3D_f GetTriangleNormal(size_t i_triangle_index) const;

    /**
    * Returns shading normal associated with the vertex.
    */
    Vector3D_f GetShadingNormal(size_t i_vertes_index) const;

    /**
    * Populates the DifferentialGeometry assuming that the specified ray intersects the specified triangle.
    */
    void ComputeDifferentialGeometry(size_t i_triangle_index, const RayDifferential &i_ray, DifferentialGeometry &o_dg) const;

    /**
    * Returns bounding box of the triangle mesh.
    */
    BBox3D_f GetBounds() const;

    /**
    * Returns sum of the areas of all triangles of the mesh.
    */
    float GetArea() const;

    /**
    * Returns TopologyInfo.
    * The first time the method is called it computes the TopologyInfo, this can take some time.
    * The next calls don't recompute the TopologyInfo and just return the cached one.
    */
    TopologyInfo GetTopologyInfo() const;

  private:
    // Internal types.
    struct ConnectivityData;
    class IllegalTrianglePredicate;

  private:
    // Not implemented, TriangleMesh should only be passed by a reference to avoid large data copying.
    TriangleMesh(const TriangleMesh&);
    TriangleMesh &operator=(const TriangleMesh&);

    void _Initialize(const std::vector<Point3D_f> &i_vertices, const std::vector<MeshTriangle> &i_triangles,
      const std::vector<Vector3D_f> &i_shading_normals, const std::vector<Vector3D_f> &i_tangents);

    void _ComputeShadingNormals(const ConnectivityData &i_connectivity);
    bool _ConsistentlyOriented(size_t i_triangle_index1, size_t i_triangle_index2) const;
    bool _ComputeIntersectionPoint(const Point3D_d i_vertices[3], const Point3D_d &i_origin, const Vector3D_d &i_direction, double &o_b1, double &o_b2, double &o_t) const;
    void _BuildConnectivityData(ConnectivityData &o_connectivity) const;
    TopologyInfo _ComputeTopologyInfo(const ConnectivityData &i_connectivity) const;

  private:
    std::vector<Point3D_f> m_vertices;
    std::vector<MeshTriangle> m_triangles;
    std::vector<Vector3D_f> m_shading_normals, m_tangents;

    bool m_use_shading_normals, m_invert_normals;

    BBox3D_f m_bbox;
    float m_area;

    mutable TopologyInfo m_topology_info;
    mutable bool m_topology_info_computed;
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

inline Point3D_f TriangleMesh::GetVertex(size_t i_vertex_index) const
  {
  ASSERT(i_vertex_index < m_vertices.size());
  return m_vertices[i_vertex_index];
  }

inline MeshTriangle TriangleMesh::GetTriangle(size_t i_triangle_index) const
  {
  ASSERT(i_triangle_index < m_triangles.size());
  return m_triangles[i_triangle_index];
  }

inline Vector3D_f TriangleMesh::GetTriangleNormal(size_t i_triangle_index) const
  {
  ASSERT(i_triangle_index < m_triangles.size());
  MeshTriangle triangle = m_triangles[i_triangle_index];
  Point3D_f vertices[3] = {
    m_vertices[triangle.m_vertices[0]],
    m_vertices[triangle.m_vertices[1]],
    m_vertices[triangle.m_vertices[2]]};

  if (m_invert_normals)
    return (Vector3D_f(vertices[2]-vertices[0])^Vector3D_f(vertices[1]-vertices[0])).Normalized();
  else
    return (Vector3D_f(vertices[1]-vertices[0])^Vector3D_f(vertices[2]-vertices[0])).Normalized();
  }

inline Vector3D_f TriangleMesh::GetShadingNormal(size_t i_vertes_index) const
  {
  ASSERT(i_vertes_index < m_shading_normals.size());
  return m_shading_normals[i_vertes_index];
  }

inline BBox3D_f TriangleMesh::GetBounds() const
  {
  return m_bbox;
  }

inline float TriangleMesh::GetArea() const
  {
  return m_area;
  }

#endif // TRIANGLE_MESH_H