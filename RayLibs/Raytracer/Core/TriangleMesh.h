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
* The class provides an option to interpolate the normals inside the triangles to make the imitate a smooth surface.
*
* The mesh is constant in the sense that once created the geometry and connectivity never changes.
*/
class TriangleMesh: public ReferenceCounted
  {
  public:
    /**
    * Creates TriangleMesh instance with the given set of triangles and vertices.
    * @param i_vertices Vertices coordinates.
    * @param i_triangles Mesh triangles.
    * @param i_use_shading_normals true if normals need to be interpolated inside the triangles.
    * @param i_invert_normals true if triangle normals need to be inverted.
    */
    TriangleMesh(const std::vector<Point3D_f> &i_vertices, const std::vector<MeshTriangle> &i_triangles, bool i_use_shading_normals=true, bool i_invert_normals=false);

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

    Vector3D_f GetTriangleNormal(size_t i_triangle_index) const;

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

    TopologyInfo GetTopologyInfo() const;

  private:
    // Internal types.
    struct ConnectivityData;
    class IllegalTrianglePredicate;

  private:
    // Not implemented, TriangleMesh should only be passed by a reference to avoid large data copying.
    TriangleMesh();
    TriangleMesh(const TriangleMesh&);
    TriangleMesh &operator=(const TriangleMesh&);

    void _ComputeShadingNormals(const ConnectivityData &i_connectivity);
    void _ComputeTopologyInfo(const ConnectivityData &i_connectivity);
    bool _ConsistentlyOriented(size_t i_triangle_index1, size_t i_triangle_index2) const;

    bool _ComputeIntersectionPoint(const Point3D_d i_vertices[3], const Point3D_d &i_origin, const Vector3D_d &i_direction, double &o_b1, double &o_b2, double &o_t) const;

    void _BuildConnectivityData(ConnectivityData &o_connectivity);

  private:
    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes TriangleMesh to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int version);

  private:
    std::vector<Point3D_f> m_vertices;
    std::vector<MeshTriangle> m_triangles;
    std::vector<Vector3D_f> m_shading_normals;

    bool m_use_shading_normals, m_invert_normals;

    BBox3D_f m_bbox;
    float m_area;

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

inline BBox3D_f TriangleMesh::GetBounds() const
  {
  return m_bbox;
  }

inline float TriangleMesh::GetArea() const
  {
  return m_area;
  }

inline TopologyInfo TriangleMesh::GetTopologyInfo() const
  {
  return m_topology_info;
  }

/**
* Saves the data which is needed to construct TriangleMesh to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const TriangleMesh *ip_mesh, const unsigned int i_version)
  {
  // Nothing to save here really. Everything will be serialzied by the serialize() method.
  }

/**
* Constructs TriangleMesh with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, TriangleMesh *ip_mesh, const unsigned int i_version)
  {
  // Just create an empty mesh.
  ::new(ip_mesh)TriangleMesh(std::vector<Point3D_f>(), std::vector<MeshTriangle>());
  }

/**
* Serializes TriangleMesh to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void TriangleMesh::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<ReferenceCounted>(*this);

  i_ar & m_vertices;
  i_ar & m_triangles;
  i_ar & m_shading_normals;

  i_ar & m_use_shading_normals;
  i_ar & m_invert_normals;

  i_ar & m_bbox;
  i_ar & m_area;

  i_ar & m_topology_info.m_manifold;
  i_ar & m_topology_info.m_number_of_patches;
  i_ar & m_topology_info.m_solid;
  }

/**
* Serializes MeshTriangle to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, MeshTriangle &i_triangle, const unsigned int i_version)
  {
  i_ar & i_triangle.m_uvs;
  i_ar & i_triangle.m_vertices;
  }

// Don't store class info for MeshTriangle.
BOOST_CLASS_IMPLEMENTATION(MeshTriangle, boost::serialization::object_serializable)

#endif // TRIANGLE_MESH_H