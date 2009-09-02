#include "TriangleMesh.h"
#include <queue>

//////////////// ConnectivityData  ////////////////

/**
* This internal structure holds the connectivity data for the mesh.
* The data is needed to find the triangles incident to a given vertex.
*/
struct TriangleMesh::ConnectivityData
  {
  /**
  * Returns number of triangles incident to the specified vertex.
  */
  size_t GetNumberOfIncidentTriangles(size_t i_vertex_index) const;

  /**
  * Returns index of the triangle incident to the specified vertex.
  * @param i_vertex_index Index of the vertex.
  * @param i_incident_triangle_index Index of the triangle among all the triangles incident to the specified vertex. This is not an index of the triangle inside the mesh.
  */
  size_t GetIncidentTriangleIndex(size_t i_vertex_index, size_t i_incident_triangle_index) const;

  /**
  * Fills the vector with all triangles incident to both specified vertices.
  */
  void GetIncidentTriangles(size_t i_vertex_index1, size_t i_vertex_index2, std::vector<size_t> &o_triangles) const;

  /**
  * Vector of indices of triangles incident to all mesh vertices.
  * First go all the triangles incident to the vertex 0, then all triangles incident to the vertex 1 and so on.
  */
  std::vector<size_t> m_incident_triangles;

  /**
  * i-th element corresponds to the position in m_incident_triangles vector where the list of triangles incident to i-th vertex starts.
  */
  std::vector<size_t> m_incident_triangles_index;
  };

inline size_t TriangleMesh::ConnectivityData::GetNumberOfIncidentTriangles(size_t i_vertex_index) const
  {
  ASSERT(i_vertex_index < m_incident_triangles_index.size());
  return m_incident_triangles_index[i_vertex_index+1]-m_incident_triangles_index[i_vertex_index];
  }

inline size_t TriangleMesh::ConnectivityData::GetIncidentTriangleIndex(size_t i_vertex_index, size_t i_incident_triangle_index) const
  {
  ASSERT(i_vertex_index < m_incident_triangles_index.size());
  ASSERT(i_incident_triangle_index < GetNumberOfIncidentTriangles(i_vertex_index));
  return m_incident_triangles[m_incident_triangles_index[i_vertex_index] + i_incident_triangle_index];
  }

void TriangleMesh::ConnectivityData::GetIncidentTriangles(size_t i_vertex_index1, size_t i_vertex_index2, std::vector<size_t> &o_triangles) const
  {
  ASSERT(i_vertex_index1 < m_incident_triangles_index.size());
  ASSERT(i_vertex_index2 < m_incident_triangles_index.size());
  ASSERT(i_vertex_index1 != i_vertex_index2);

  // Linear pass through both lists.
  // Here, we use fact that the triangle indices are order increasingly.
  o_triangles.clear();
  size_t i1 = m_incident_triangles_index[i_vertex_index1];
  size_t i2 = m_incident_triangles_index[i_vertex_index2];
  while(i1 < m_incident_triangles_index[i_vertex_index1+1] && i2 < m_incident_triangles_index[i_vertex_index2+1])
    {
    if (m_incident_triangles[i1] < m_incident_triangles[i2])
      ++i1;
    else if (m_incident_triangles[i1] > m_incident_triangles[i2])
      ++i2;
    else if (m_incident_triangles[i1] == m_incident_triangles[i2])
      {
      o_triangles.push_back(m_incident_triangles[i1]);
      ++i1;++i2;
      }
    }
  }

//////////////// MeshTriangle ////////////////

MeshTriangle::MeshTriangle()
  {
  }

MeshTriangle::MeshTriangle(size_t i_v1, size_t i_v2, size_t i_v3)
  {
  m_vertices[0]=i_v1;
  m_vertices[1]=i_v2;
  m_vertices[2]=i_v3;
  }

TriangleMesh::TriangleMesh(const std::vector<Point3D_f> &i_vertices, const std::vector<MeshTriangle> &i_triangles, bool i_use_shading_normals):
m_vertices(i_vertices.begin(),i_vertices.end()),
m_triangles(i_triangles.begin(), i_triangles.end()),
m_use_shading_normals(i_use_shading_normals)
  {
  // Compute and cache triangle normals.
  m_triangle_normals.reserve(m_triangles.size());
  for(size_t i=0;i<m_triangles.size();++i)
    {
    const MeshTriangle &triangle = m_triangles[i];
    if (triangle.m_vertices[0] >= m_vertices.size() || triangle.m_vertices[1] >= m_vertices.size() || triangle.m_vertices[2] >= m_vertices.size())
      ASSERT(0 && "TriangleMesh has out of-bounds vertex index.");

    Vector3D_f normal = Vector3D_f(m_vertices[triangle.m_vertices[1]]-m_vertices[triangle.m_vertices[0]]) ^ Vector3D_f(m_vertices[triangle.m_vertices[2]]-m_vertices[triangle.m_vertices[0]]);
    normal.Normalize();
    m_triangle_normals.push_back(normal);
    }

  // We don't store the connectivity info as a data member because it is takes a lot of memory and is not used once the mesh is constructed.
  // Instead, it is passed by a reference to all the methods that need it.
  ConnectivityData connectivity;
  _BuildConnectivityData(connectivity);

  _ComputeTopologyInfo(connectivity);
  _ComputeShadingNormals(connectivity);
  }

/**
* Populates the specified output parameter with the connectivity data.
*/
void TriangleMesh::_BuildConnectivityData(ConnectivityData &o_connectivity)
  {
  std::vector<size_t> number_of_incident_triangles(m_vertices.size(),0);
  for(size_t i=0;i<m_triangles.size();++i)
    {
    const MeshTriangle &triangle = m_triangles[i];
    number_of_incident_triangles[triangle.m_vertices[0]]++;
    number_of_incident_triangles[triangle.m_vertices[1]]++;
    number_of_incident_triangles[triangle.m_vertices[2]]++;
    }

  o_connectivity.m_incident_triangles_index.assign(m_vertices.size()+1,0);
  for(size_t i=1;i<m_vertices.size()+1;++i)
    o_connectivity.m_incident_triangles_index[i]=o_connectivity.m_incident_triangles_index[i-1]+number_of_incident_triangles[i-1];

  o_connectivity.m_incident_triangles.assign(o_connectivity.m_incident_triangles_index.back(),0);
  for(size_t i=0;i<m_triangles.size();++i)
    {
    const MeshTriangle &triangle = m_triangles[i];
    o_connectivity.m_incident_triangles[o_connectivity.m_incident_triangles_index[triangle.m_vertices[0]]++]=i;
    o_connectivity.m_incident_triangles[o_connectivity.m_incident_triangles_index[triangle.m_vertices[1]]++]=i;
    o_connectivity.m_incident_triangles[o_connectivity.m_incident_triangles_index[triangle.m_vertices[2]]++]=i;
    }

  // Restore the indices because we just spoiled them (this is easier than creating a separate vector of indices).
  o_connectivity.m_incident_triangles_index[0]=0;
  for(size_t i=1;i<m_vertices.size();++i)
    o_connectivity.m_incident_triangles_index[i]=o_connectivity.m_incident_triangles_index[i-1]+number_of_incident_triangles[i-1];
  }

/**
* Computes shading normals for the vertices.
* A vertex normal is a weighted sum of normals of incident triangles. The weights are the angles of the triangles at that vertex.
*/
void TriangleMesh::_ComputeShadingNormals(const ConnectivityData &i_connectivity)
  {
  m_shading_normals.assign(m_vertices.size(),Vector3D_f());

  for (size_t i=0;i<m_vertices.size();++i)
    {
    Vector3D_f normal;

    size_t number_of_incident_triangles = i_connectivity.GetNumberOfIncidentTriangles(i);
    for (size_t j=0;j<number_of_incident_triangles;++j)
      {
      size_t triangle_index = i_connectivity.GetIncidentTriangleIndex(i,j);
      MeshTriangle triangle = GetTriangle(triangle_index);
      Point3D_f vertices[3] = {
        GetVertex(triangle.m_vertices[0]),
        GetVertex(triangle.m_vertices[1]),
        GetVertex(triangle.m_vertices[2])};

      // Find the index of the i-th vertex inside this triangle.
      char index = -1;
      for(char k=0;k<3;++k)
        if (triangle.m_vertices[k]==i)
          {index=k;break;}

      ASSERT(index != -1);

      Vector3D_f edge1 = Vector3D_f(vertices[(index+1)%3]-vertices[index]);
      Vector3D_f edge2 = Vector3D_f(vertices[(index+2)%3]-vertices[index]);

      float weight;
      float sin_value = (edge1.Normalized()^edge2.Normalized()).Length();
      if(sin_value>1.0)
        weight = M_PI_2;
      else
        weight = asinf( (edge1.Normalized()^edge2.Normalized()).Length() );
      normal += GetTriangleNormal(triangle_index)*weight;
      }

    m_shading_normals[i]=normal.Normalized();
    }
  }

/**
* Computes and caches the topology info for the mesh.
* We use BFS to iterate through all the triangles.
*/
void TriangleMesh::_ComputeTopologyInfo(const ConnectivityData &i_connectivity)
  {
  m_topology_info.m_solid=true;
  m_topology_info.m_manifold=true;

  unsigned int number_of_patches = 0;

  // Keeps track of what triangles we have visited so far.
  std::vector<bool> visited(GetNumberOfTriangles(), false);

  // Iterate through all the triangles to make sure we find all the connected patches.
  for(size_t i=0;i<GetNumberOfTriangles();++i)
    if (visited[i]==false)
      {
      ++number_of_patches;

      // Construct a queue for the BFS and add the first triangle to start with.
      std::queue<size_t> qu;
      qu.push(i);
      visited[i]=true;

      while(qu.empty()==false)
        {
        size_t current_triangle_index = qu.front();
        MeshTriangle current_triangle = GetTriangle(current_triangle_index);
        qu.pop();

        // Iterate by triangle's edges.
        for(unsigned char v=0;v<3;++v)
          {
          size_t v1 = current_triangle.m_vertices[v];
          size_t v2 = current_triangle.m_vertices[(v+1)%3];

          size_t adjacent_triangles=0;
          std::vector<size_t> triangles;
          i_connectivity.GetIncidentTriangles(v1,v2,triangles);

          // Iterate by all the triangles incident to the current edge.
          for(size_t t=0;t<triangles.size();++t)
            if (triangles[t]!=current_triangle_index)
              if (_ConsistentlyOriented(current_triangle_index,triangles[t])==false)
                m_topology_info.m_manifold=false;
              else
                {
                ++adjacent_triangles;
                if (visited[triangles[t]]==false)
                  {
                  qu.push(triangles[t]);
                  visited[triangles[t]]=true;
                  }
                }

              if (adjacent_triangles==0)
                m_topology_info.m_solid=false;
              if (adjacent_triangles>1)
                m_topology_info.m_manifold=false;
          }
        } // while(qu.empty()==false)

      }

    m_topology_info.m_number_of_patches=number_of_patches;
    if (number_of_patches>1)
      m_topology_info.m_solid=false;
  }

/**
* Returns whether two triangles are consistently oriented with respect to each other.
*/
bool TriangleMesh::_ConsistentlyOriented(size_t i_triangle_index1, size_t i_triangle_index2) const
  {
  ASSERT(i_triangle_index1 != i_triangle_index2);
  const MeshTriangle &tr1 = GetTriangle(i_triangle_index1);
  const MeshTriangle &tr2 = GetTriangle(i_triangle_index2);

  for(unsigned char i=0;i<3;++i)
    {
    size_t v1 = tr1.m_vertices[i];

    char v_index=-1;
    if (v1==tr2.m_vertices[0]) v_index=0;
    else if (v1==tr2.m_vertices[1]) v_index=1;
    else if (v1==tr2.m_vertices[2]) v_index=2;
    else continue;

    if (tr1.m_vertices[(i+1)%3] == tr2.m_vertices[(v_index+2)%3])
      return true;
    if (tr1.m_vertices[(i+1)%3] == tr2.m_vertices[(v_index+1)%3])
      return false;
    }

  ASSERT(false && "Orientation consistency check: the triangles are not adjacent.");
  return false;
  }

/**
* Computes the point of intersection of the specified ray an the triangle's plane.
* @param i_triangle Mesh triangle.
* @param i_origin Ray origin.
* @param i_direction Ray direction.
* @param[out] o_b1 A barycentric coordinate of the intersection point corresponding to the 1th vertex of the specified triangle (0-based).
* @param[out] o_b2 A barycentric coordinate of the intersection point corresponding to the 2nd vertex of the specified triangle (0-based).
* @param[out] o_t The ray parameter for the intersection point.
* @return true if the ray intersects the triangle's plane and false if it is parrallel to it.
*/
bool TriangleMesh::_ComputeIntersectionPoint(const MeshTriangle &i_triangle, const Point3D_d &i_origin, const Vector3D_d &i_direction, double &o_b1, double &o_b2, double &o_t) const
  {
  const Point3D_d &v0 = Convert<double>(GetVertex(i_triangle.m_vertices[0]));
  const Point3D_d &v1 = Convert<double>(GetVertex(i_triangle.m_vertices[1]));
  const Point3D_d &v2 = Convert<double>(GetVertex(i_triangle.m_vertices[2]));

  Vector3D_d e1 = Vector3D_d(v1 - v0);
  Vector3D_d e2 = Vector3D_d(v2 - v0);
  Vector3D_d s1 = i_direction^e2;
  double divisor = s1*e1;
  if(divisor == 0.0)
    return false;

  double invDivisor = 1.0 / divisor;

  // Compute first barycentric coordinate.
  Vector3D_d d = Vector3D_d(i_origin - v0);
  o_b1 = (d*s1) * invDivisor;

  // Compute second barycentric coordinate.
  Vector3D_d s2 = d^e1;
  o_b2 = (i_direction*s2) * invDivisor;

  // Compute t to intersection point.
  o_t = (e2*s2) * invDivisor;
  return true;
  }

void TriangleMesh::ComputeDifferentialGeometry(size_t i_triangle_index, const RayDifferential &i_ray, DifferentialGeometry &o_dg) const
  {
  const MeshTriangle &triangle = GetTriangle(i_triangle_index);

  double b0,b1,b2,t;
  if (_ComputeIntersectionPoint(triangle, i_ray.m_base_ray.m_origin, i_ray.m_base_ray.m_direction, b1, b2, t)==false)
    ASSERT(0 && "The ray does not intersect the triangle.");

  ASSERT(b1 > -DBL_EPS && b1 < 1.0+DBL_EPS);
  ASSERT(b2 > -DBL_EPS && b1 + b2 < 1.0+DBL_EPS);
  ASSERT(t >= i_ray.m_base_ray.m_min_t && t <= i_ray.m_base_ray.m_max_t);
  o_dg.m_point=i_ray.m_base_ray(t);

  Point2D_d uv[3];
  _GetUVs(triangle,uv);

  // Interpolate triangle uv coordinates.
  b0 = 1.0 - b1 - b2;
  o_dg.m_uv=b0*uv[0] + b1*uv[1] + b2*uv[2];

  o_dg.m_geometric_normal=Convert<double>(m_triangle_normals[i_triangle_index]);

  // Compute shading normal and derivatives.
  if (m_use_shading_normals)
    {
    Vector3D_d vertex_normals[3] = {
      Convert<double>(m_shading_normals[triangle.m_vertices[0]]),
      Convert<double>(m_shading_normals[triangle.m_vertices[1]]),
      Convert<double>(m_shading_normals[triangle.m_vertices[2]])};

      o_dg.m_shading_normal=b0*vertex_normals[0]+b1*vertex_normals[1]+b2*vertex_normals[2];
      o_dg.m_shading_normal.Normalize();

      // Compute deltas for triangle partial derivatives.
      double du1 = uv[1][0] - uv[0][0];
      double du2 = uv[2][0] - uv[0][0];
      double dv1 = uv[1][1] - uv[0][1];
      double dv2 = uv[2][1] - uv[0][1];
      double determinant = du1 * dv2 - dv1 * du2;
      if (determinant == 0.0)
        {
        o_dg.m_dn_du = Vector3D_d();
        o_dg.m_dn_dv = Vector3D_d();
        }
      else
        {
        double invdet = 1.0 / determinant;
        o_dg.m_dn_du = ( dv2 * (vertex_normals[1] - vertex_normals[0]) - dv1 * (vertex_normals[2] - vertex_normals[0])) * invdet;
        o_dg.m_dn_dv = (-du2 * (vertex_normals[1] - vertex_normals[0]) + du1 * (vertex_normals[2] - vertex_normals[0])) * invdet;
        }
    }
  else
    {
    o_dg.m_shading_normal=o_dg.m_geometric_normal;
    o_dg.m_dn_du=o_dg.m_dn_dv=Vector3D_d();
    }

  // Compute screen-space differentials.
  double b0_x,b1_x,b2_x,t_x;
  double b0_y,b1_y,b2_y,t_y;
  if (i_ray.m_has_differentials &&
    _ComputeIntersectionPoint(triangle, i_ray.m_origin_dx, i_ray.m_direction_dx, b1_x, b2_x, t_x) &&
    _ComputeIntersectionPoint(triangle, i_ray.m_origin_dy, i_ray.m_direction_dy, b1_y, b2_y, t_y))
    {
    b0_x = 1.0 - b1_x - b2_x;
    o_dg.m_dp_dx=Vector3D_d(i_ray.m_origin_dx+i_ray.m_direction_dx*t_x-o_dg.m_point);
    o_dg.m_duv_dx=Vector2D_d(b0_x*uv[0] + b1_x*uv[1] + b2_x*uv[2] - o_dg.m_uv);

    b0_y = 1.0 - b1_y - b2_y;
    o_dg.m_dp_dy=Vector3D_d(i_ray.m_origin_dy+i_ray.m_direction_dy*t_y-o_dg.m_point);
    o_dg.m_duv_dy=Vector2D_d(b0_y*uv[0] + b1_y*uv[1] + b2_y*uv[2] - o_dg.m_uv);
    }
  else
    {
    o_dg.m_duv_dx = Vector2D_d();
    o_dg.m_duv_dy = Vector2D_d();
    o_dg.m_dp_dx = o_dg.m_dp_dy = Vector3D_d();
    }
  }

/**
* Returns UV triangles for the specified triangle.
* If UV parameterization is not set a dummy set of UV coordinates is returned.
*/
void TriangleMesh::_GetUVs(const MeshTriangle &i_triangle, Point2D_d o_uv[3]) const
  {
  if (m_uv_parameterization.empty()) 
    {
    o_uv[0] = Point2D_d(0.f, 0.f);
    o_uv[1] = Point2D_d(1.f, 0.f);
    o_uv[2] = Point2D_d(0.f, 1.f);
    }
  else
    {
    o_uv[0] = Convert<double>(m_uv_parameterization[i_triangle.m_vertices[0]]);
    o_uv[1] = Convert<double>(m_uv_parameterization[i_triangle.m_vertices[1]]);
    o_uv[2] = Convert<double>(m_uv_parameterization[i_triangle.m_vertices[2]]);
    }
  }

void TriangleMesh::SetUVParameterization(const std::vector<Point2D_f> &i_uv_parameterization)
  {
  ASSERT(i_uv_parameterization.size() == m_vertices.size());
  m_uv_parameterization.assign(i_uv_parameterization.begin(), i_uv_parameterization.end());
  }

void TriangleMesh::SetUseShadingNormals(bool i_use_shading_normals)
  {
  m_use_shading_normals=i_use_shading_normals;
  }