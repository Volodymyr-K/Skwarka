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

#include "TriangleAccelerator.h"
#include "TriangleMesh.h"
#include <tbb/tbb.h>
#include <numeric>
#include <cstring>
#include <map>

TriangleAccelerator::TriangleAccelerator(std::vector<intrusive_ptr<const Primitive>> i_primitives):
mp_root(NULL), m_primitives(i_primitives), m_pool(100000*sizeof(TriangleAccelerator::Node))
  {
  typedef std::map<const TriangleMesh *, std::vector<size_t>> InstancesMap;
  typedef InstancesMap::const_iterator InstancesIterator;

  /*
  * Step 1. Group all primitives by their meshes.
  * Some meshes can be shared by more than one primitive in which case we deal with instanced objects.
  */
  InstancesMap instances_map;
  for(size_t i=0;i<i_primitives.size();++i)
    {
    const TriangleMesh *p_mesh = i_primitives[i]->GetTriangleMesh_RawPtr();
    std::vector<size_t> &instanced_primitives = instances_map[p_mesh];
    instanced_primitives.push_back(i);
    }

  // Count triangles from unique meshes (different primitives can share the same mesh).
  size_t number_of_triangles=0;
  for(InstancesIterator it = instances_map.begin(); it!=instances_map.end(); ++it)
    number_of_triangles += (it->first)->GetNumberOfTriangles();

  m_triangles.reserve(number_of_triangles);
  m_primitive_indices.reserve(number_of_triangles);
  m_triangle_indices.reserve(number_of_triangles);
  m_triangle_bboxes.reserve(number_of_triangles);

  /*
  * Step 2. Process the primitives that share mesh with other primitive(s).
  * For each shared mesh we construct its own (sub)tree that will later be included in the final tree.
  */
  for(InstancesIterator it = instances_map.begin(); it!=instances_map.end(); ++it)
    if (it->second.size() > 1) // Test that this primitive shares the mesh.
      {
      const TriangleMesh *p_mesh = it->first;
      const std::vector<size_t> &instanced_primitives = it->second;

      size_t previous_triangles_num = m_triangles.size();
      for(size_t j=0;j<p_mesh->GetNumberOfTriangles();++j)
        {
        MeshTriangle triangle = p_mesh->GetTriangle(j);
        Triangle3D_f triangle_3d(
          p_mesh->GetVertex(triangle.m_vertices[0]),
          p_mesh->GetVertex(triangle.m_vertices[1]),
          p_mesh->GetVertex(triangle.m_vertices[2]));

        m_triangles.push_back(triangle_3d);
        m_primitive_indices.push_back(std::numeric_limits<size_t>::max()); // No specific primitive associated with the triangle.
        m_triangle_indices.push_back(j);

        BBox3D_f triangle_bbox;
        triangle_bbox.Unite(triangle_3d[0]);
        triangle_bbox.Unite(triangle_3d[1]);
        triangle_bbox.Unite(triangle_3d[2]);
        m_triangle_bboxes.push_back(triangle_bbox);
        }

      void *ptr = m_pool.Alloc(sizeof(Node));
      const Node *p_sub_tree = new (ptr) Node(*this, previous_triangles_num, m_triangles.size(), 0, 0, 0, 0);

      // Now for all primitives sharing this mesh store the information such as world space bbox, pointer to the subtree etc.
      for(size_t i=0;i<instanced_primitives.size();++i)
        {
        ASSERT( m_primitives[instanced_primitives[i]]->GetTriangleMesh_RawPtr() == p_mesh );

        Transform instance_to_world = m_primitives[instanced_primitives[i]]->GetMeshToWorldTransform();
        BBox3D_f instance_bbox;
        for(unsigned char j=0;j<8;++j)
          {
          Point3D_f mn = p_sub_tree->m_bbox.m_min, mx = p_sub_tree->m_bbox.m_max;
          Point3D_f bbox_vertex((j&1)?mn[0]:mx[0], (j&2)?mn[1]:mx[1], (j&4)?mn[2]:mx[2]);
          instance_bbox.Unite(instance_to_world(bbox_vertex));
          }

        m_instance_primitive_indices.push_back(instanced_primitives[i]);
        m_instance_nodes.push_back(p_sub_tree);
        m_instance_to_world_transformations.push_back(instance_to_world);
        m_instance_bboxes.push_back(instance_bbox);
        }
      }

  /*
  * Step 3. Process the primitives that do not share mesh with other primitives.
  */
  size_t previous_triangles_num = m_triangles.size();
  for(InstancesIterator it = instances_map.begin(); it!=instances_map.end(); ++it)
    if (it->second.size() == 1)
      {
      const TriangleMesh *p_mesh = it->first;
      size_t primitive_index = it->second[0];
      Transform mesh_to_world = m_primitives[primitive_index]->GetMeshToWorldTransform();

      for(size_t j=0;j<p_mesh->GetNumberOfTriangles();++j)
        {
        MeshTriangle triangle = p_mesh->GetTriangle(j);
        Triangle3D_f triangle_3d(
          mesh_to_world( p_mesh->GetVertex(triangle.m_vertices[0]) ),
          mesh_to_world( p_mesh->GetVertex(triangle.m_vertices[1]) ),
          mesh_to_world( p_mesh->GetVertex(triangle.m_vertices[2]) ));

        m_triangles.push_back(triangle_3d);
        m_primitive_indices.push_back(primitive_index);
        m_triangle_indices.push_back(j);

        BBox3D_f triangle_bbox;
        triangle_bbox.Unite(triangle_3d[0]);
        triangle_bbox.Unite(triangle_3d[1]);
        triangle_bbox.Unite(triangle_3d[2]);
        m_triangle_bboxes.push_back(triangle_bbox);
        }
      }

  /*
  * Step 4. Finally, construct the resulting tree with all the triangles and instanced objects.
  */
  void *ptr = m_pool.Alloc(sizeof(Node));
  mp_root = new (ptr) Node(*this, previous_triangles_num, m_triangles.size(), 0, m_instance_nodes.size(), 0, 0);

  // Check the final size of the triangles vector, each uniques triangle should have been added exactly once.
  ASSERT(m_triangles.size() == number_of_triangles);

  // Release the memory, we don't longer need the bboxes.
  m_triangle_bboxes.swap(std::vector<BBox3D_f>());
  m_instance_bboxes.swap(std::vector<BBox3D_f>());
  }

BBox3D_d TriangleAccelerator::GetWorldBounds() const
  {
  return Convert<double>(mp_root->m_bbox);
  }

bool TriangleAccelerator::_NodeIntersect(const TriangleAccelerator::Node *ip_node, Ray &io_ray, size_t &o_primitive_index, size_t &o_triangle_index) const
  {
  Ray ray(io_ray);

  double invs[3];
  invs[0]=1.0/ray.m_direction[0];
  invs[1]=1.0/ray.m_direction[1];
  invs[2]=1.0/ray.m_direction[2];

  typedef const Node *NodePtr;
  NodePtr todo[2*MAX_TREE_DEPTH+1];
  todo[0]=ip_node;
  int todo_size=1;

  bool intersected = false, instanced_primitive_intersected = false;
  size_t triangle_index;
  while (todo_size>0)
    {
    ASSERT(todo_size<=2*MAX_TREE_DEPTH+1);
    const Node *p_node = todo[--todo_size];

    // Check whether the ray intersects the bbox of the node.
    double tNear1 = (p_node->m_bbox.m_min[0] - ray.m_origin[0]) * invs[0];
    double tFar1  = (p_node->m_bbox.m_max[0] - ray.m_origin[0]) * invs[0];
    if (tNear1 > tFar1) std::swap(tNear1, tFar1);

    double tNear2 = (p_node->m_bbox.m_min[1] - ray.m_origin[1]) * invs[1];
    double tFar2  = (p_node->m_bbox.m_max[1] - ray.m_origin[1]) * invs[1];
    if (tNear2 > tFar2) std::swap(tNear2, tFar2);

    double tNear3 = (p_node->m_bbox.m_min[2] - ray.m_origin[2]) * invs[2];
    double tFar3  = (p_node->m_bbox.m_max[2] - ray.m_origin[2]) * invs[2];
    if (tNear3 > tFar3) std::swap(tNear3, tFar3);

    if (
      ray.m_min_t > tFar1 || tNear1 > tFar2 || tNear2 > tFar1 || tNear3 > tFar1 ||
      ray.m_min_t > tFar2 || tNear1 > tFar3 || tNear2 > tFar3 || tNear3 > tFar2 ||
      ray.m_min_t > tFar3 || tNear1 > ray.m_max_t || tNear2 > ray.m_max_t || tNear3 > ray.m_max_t)
      continue;

    if (p_node->IsLeaf()==false)
      {
      int axis = p_node->GetSplitAxis();

      // Insert the children in a reverse order because this is a stack and they are pulled back from the top.
      if (ray.m_direction[axis]>0.0)
        {
        if (p_node->m_children[2]) todo[todo_size++]=p_node->m_children[2];
        if (p_node->m_children[1]) todo[todo_size++]=p_node->m_children[1];
        if (p_node->m_children[0]) todo[todo_size++]=p_node->m_children[0];
        }
      else
        {
        if (p_node->m_children[0]) todo[todo_size++]=p_node->m_children[0];
        if (p_node->m_children[1]) todo[todo_size++]=p_node->m_children[1];
        if (p_node->m_children[2]) todo[todo_size++]=p_node->m_children[2];
        }
      }
    else
      {
      // Process all object instances associated with the leaf. This is done by calling the method recursively for the subtrees.
      for(size_t i=p_node->m_instances_begin;i<p_node->m_instances_end;++i)
        {
        Ray transformed_ray;
        m_instance_to_world_transformations[i].Inverted().operator()(ray, transformed_ray);

        size_t triangle_index2, primitive_index2;
        if ( _NodeIntersect(m_instance_nodes[i], transformed_ray, primitive_index2, triangle_index2) )
          {
          intersected = true;
          instanced_primitive_intersected = true;

          ray.m_max_t = transformed_ray.m_max_t;
          o_triangle_index = triangle_index2;
          o_primitive_index = m_instance_primitive_indices[i];

          // NOTE: We do not use the value of primitive_index2 because only one level of instance nesting is supported.
          ASSERT(primitive_index2 == std::numeric_limits<size_t>::max());
          }
        }

      // And finally process all triangles in the leaf.
      for(size_t i=p_node->m_triangles_begin;i<p_node->m_triangles_end;++i)
        {
        const Triangle3D_f &triangle = m_triangles[i];

        Point3D_d v0 = Convert<double>(triangle[0]);
        Point3D_d v1 = Convert<double>(triangle[1]);
        Point3D_d v2 = Convert<double>(triangle[2]);

        Vector3D_d e1 = Vector3D_d(v1-v0);
        Vector3D_d e2 = Vector3D_d(v2-v0);
        Vector3D_d s1 = ray.m_direction^e2;
        double divisor = s1*e1;
        double inv_divisor = 1.0/divisor;

        // Compute first barycentric coordinate.
        Vector3D_d d = Vector3D_d(ray.m_origin - v0);
        double b1 = (d*s1) * inv_divisor;

        // The two nested if-s are structured carefully to deal with the cases when the ray is parallel to the triangle's plane.
        // In this case the barycentric coordinates will have NaN values and the if-s will return false.
        if (b1 > -DBL_EPS && b1 < (1.0+DBL_EPS))
          {
          // Compute second barycentric coordinate.
          Vector3D_d s2 = d^e1;
          double b2 = (ray.m_direction*s2) * inv_divisor;
          if (b2 > -DBL_EPS && b1 + b2 < (1.0+DBL_EPS))
            {
            // Compute t to intersection point.
            double t = (e2*s2) * inv_divisor;
            if (t >= ray.m_min_t && t <= ray.m_max_t)
              {
              ray.m_max_t = t;
              triangle_index = i;

              intersected = true;
              instanced_primitive_intersected = false;
              }
            }
          }
        } // end of loop by triangles in the leaf

      }

    }

  io_ray.m_max_t = ray.m_max_t;
  if (instanced_primitive_intersected)
    return true;
  else
    if (intersected)
      {
      o_primitive_index = m_primitive_indices[triangle_index];
      o_triangle_index = m_triangle_indices[triangle_index];
      return true;
      }
    else
      return false;
  }

bool TriangleAccelerator::Intersect(const RayDifferential &i_ray, Intersection &o_intersection, double *o_t) const
  {
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());
  size_t primitive_index, triangle_index;

  Ray ray(i_ray.m_base_ray);
  if (_NodeIntersect(mp_root, ray, primitive_index, triangle_index))
    {
    if (o_t) *o_t = ray.m_max_t;

    intrusive_ptr<const Primitive> p_primitive = m_primitives[primitive_index];
    o_intersection.mp_primitive = p_primitive.get();
    o_intersection.m_triangle_index = triangle_index;

    const TriangleMesh *p_mesh = p_primitive->GetTriangleMesh_RawPtr();
    Transform mesh_to_world = p_primitive->GetMeshToWorldTransform();
    Transform world_to_mesh = mesh_to_world.Inverted();

    // Transform ray to the instance space.
    RayDifferential transformed_ray(i_ray);
    world_to_mesh(i_ray.m_base_ray, transformed_ray.m_base_ray);
    world_to_mesh(i_ray.m_origin_dx, transformed_ray.m_origin_dx);
    world_to_mesh(i_ray.m_origin_dy, transformed_ray.m_origin_dy);
    world_to_mesh(i_ray.m_direction_dx, transformed_ray.m_direction_dx);
    world_to_mesh(i_ray.m_direction_dy, transformed_ray.m_direction_dy);

    DifferentialGeometry transformed_dg;
    p_mesh->ComputeDifferentialGeometry(o_intersection.m_triangle_index, transformed_ray, transformed_dg);

    // Transform DifferentialGeometry back to the world space.
    o_intersection.m_dg = transformed_dg;
    mesh_to_world(transformed_dg.m_point, o_intersection.m_dg.m_point);
    mesh_to_world(transformed_dg.m_point_dx, o_intersection.m_dg.m_point_dx);
    mesh_to_world(transformed_dg.m_point_dy, o_intersection.m_dg.m_point_dy);
    mesh_to_world(transformed_dg.m_tangent, o_intersection.m_dg.m_tangent);
    o_intersection.m_dg.m_geometric_normal = mesh_to_world.TransformNormal(transformed_dg.m_geometric_normal);
    o_intersection.m_dg.m_shading_normal = mesh_to_world.TransformNormal(transformed_dg.m_shading_normal);
    o_intersection.m_dg.m_normal_dx = mesh_to_world.TransformNormal(transformed_dg.m_normal_dx);
    o_intersection.m_dg.m_normal_dy = mesh_to_world.TransformNormal(transformed_dg.m_normal_dy);
  
    // Save the triangle information that will be used for determining epsilon value when a next outgoing ray is being shot.
    MeshTriangle triangle=p_mesh->GetTriangle(o_intersection.m_triangle_index);
    Point3D_d v0 = Convert<double>(p_mesh->GetVertex(triangle.m_vertices[0]));
    Point3D_d v1 = Convert<double>(p_mesh->GetVertex(triangle.m_vertices[1]));
    Point3D_d v2 = Convert<double>(p_mesh->GetVertex(triangle.m_vertices[2]));

    // Compute the values of the ray-triangle intersection test which are used later to avoid the intersection with the same triangle.
    // Notice that instead of using transformed_dg.m_point we call world_to_mesh(o_intersection.m_dg.m_point) to get *exactly* the same coordinates
    // that would be used later during the intersection test.
    o_intersection.m_cross = Vector3D_d(v1-v0)^Vector3D_d(v2-v0);
    o_intersection.m_dot = (Vector3D_d(v0-world_to_mesh(o_intersection.m_dg.m_point)) ^ Vector3D_d(v1-v0)) * Vector3D_d(v2-v0);

    return true;
    }

  if (o_t) *o_t=DBL_INF;
  return false;
  }

bool TriangleAccelerator::_NodeIntersectTest(const TriangleAccelerator::Node *ip_node, const Ray &i_ray) const
  {
  Ray ray(i_ray);

  double invs[3];
  invs[0]=1.0/ray.m_direction[0];
  invs[1]=1.0/ray.m_direction[1];
  invs[2]=1.0/ray.m_direction[2];

  typedef const Node *NodePtr;
  NodePtr todo[2*MAX_TREE_DEPTH+1];
  todo[0]=ip_node;
  int todo_size=1;

  while (todo_size>0)
    {
    ASSERT(todo_size<=2*MAX_TREE_DEPTH+1);
    const Node *p_node = todo[--todo_size];

    // Check whether the ray intersects the bbox of the node.
    double tNear1 = (p_node->m_bbox.m_min[0] - ray.m_origin[0]) * invs[0];
    double tFar1  = (p_node->m_bbox.m_max[0] - ray.m_origin[0]) * invs[0];
    if (tNear1 > tFar1) std::swap(tNear1, tFar1);

    double tNear2 = (p_node->m_bbox.m_min[1] - ray.m_origin[1]) * invs[1];
    double tFar2  = (p_node->m_bbox.m_max[1] - ray.m_origin[1]) * invs[1];
    if (tNear2 > tFar2) std::swap(tNear2, tFar2);

    double tNear3 = (p_node->m_bbox.m_min[2] - ray.m_origin[2]) * invs[2];
    double tFar3  = (p_node->m_bbox.m_max[2] - ray.m_origin[2]) * invs[2];
    if (tNear3 > tFar3) std::swap(tNear3, tFar3);

    if (
      ray.m_min_t > tFar1 || tNear1 > tFar2 || tNear2 > tFar1 || tNear3 > tFar1 ||
      ray.m_min_t > tFar2 || tNear1 > tFar3 || tNear2 > tFar3 || tNear3 > tFar2 ||
      ray.m_min_t > tFar3 || tNear1 > ray.m_max_t || tNear2 > ray.m_max_t || tNear3 > ray.m_max_t)
      continue;

    if (p_node->IsLeaf()==false)
      {
      int axis = p_node->GetSplitAxis();

      // Insert the children in a reverse order because this is a stack and they are pulled back from the top.
      if (ray.m_direction[axis]>0.0)
        {
        if (p_node->m_children[2]) todo[todo_size++]=p_node->m_children[2];
        if (p_node->m_children[1]) todo[todo_size++]=p_node->m_children[1];
        if (p_node->m_children[0]) todo[todo_size++]=p_node->m_children[0];
        }
      else
        {
        if (p_node->m_children[0]) todo[todo_size++]=p_node->m_children[0];
        if (p_node->m_children[1]) todo[todo_size++]=p_node->m_children[1];
        if (p_node->m_children[2]) todo[todo_size++]=p_node->m_children[2];
        }
      }
    else
      {
      // Process all object instances associated with the leaf. This is done by calling the method recursively for the subtrees.
      for(size_t i=p_node->m_instances_begin;i<p_node->m_instances_end;++i)
        {
        Ray transformed_ray;
        m_instance_to_world_transformations[i].Inverted().operator()(ray, transformed_ray);
        if ( _NodeIntersectTest(m_instance_nodes[i], transformed_ray) )
          return true;
        }

      // And finally process all triangles in the leaf.
      for(size_t i=p_node->m_triangles_begin;i<p_node->m_triangles_end;++i)
        {
        const Triangle3D_f &triangle = m_triangles[i];

        Point3D_d v0 = Convert<double>(triangle[0]);
        Point3D_d v1 = Convert<double>(triangle[1]);
        Point3D_d v2 = Convert<double>(triangle[2]);

        Vector3D_d e1 = Vector3D_d(v1-v0);
        Vector3D_d e2 = Vector3D_d(v2-v0);
        Vector3D_d s1 = ray.m_direction^e2;
        double divisor = s1*e1;
        double inv_divisor = 1.0/divisor;

        // Compute first barycentric coordinate.
        Vector3D_d d = Vector3D_d(ray.m_origin - v0);
        double b1 = (d*s1) * inv_divisor;

        // The two nested if-s are structured carefully to deal with the cases when the ray is parallel to the triangle's plane.
        // In this case the barycentric coordinates will have NaN values and the if-s will return false.
        if (b1 > -DBL_EPS && b1 < (1.0+DBL_EPS))
          {
          // Compute second barycentric coordinate.
          Vector3D_d s2 = d^e1;
          double b2 = (ray.m_direction*s2) * inv_divisor;
          if (b2 > -DBL_EPS && b1 + b2 < (1.0+DBL_EPS))
            {
            // Compute t to intersection point.
            double t = (e2*s2) * inv_divisor;
            if (t >= ray.m_min_t && t <= ray.m_max_t)
              return true;
            }
          }
        } // end of loop by triangles in the leaf

      }

    }

  return false;
  }

bool TriangleAccelerator::IntersectTest(const Ray &i_ray) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());
  return _NodeIntersectTest(mp_root, i_ray);
  }

BBox3D_f TriangleAccelerator::_ConstructBBox(size_t i_triangles_begin, size_t i_triangles_end, size_t i_instances_begin, size_t i_instances_end) const
  {
  BBox3D_f bbox;

  for(size_t i=i_triangles_begin;i!=i_triangles_end;++i)
    bbox.Unite(m_triangle_bboxes[i]);

  for(size_t i=i_instances_begin;i!=i_instances_end;++i)
    bbox.Unite(m_instance_bboxes[i]);

  return bbox;
  }

void TriangleAccelerator::_SwapTriangles(size_t i_index1, size_t i_index2)
  {
  ASSERT(i_index1<m_triangles.size());
  ASSERT(i_index2<m_triangles.size());

  std::swap(m_triangles[i_index1], m_triangles[i_index2]);
  std::swap(m_primitive_indices[i_index1], m_primitive_indices[i_index2]);
  std::swap(m_triangle_indices[i_index1], m_triangle_indices[i_index2]);
  std::swap(m_triangle_bboxes[i_index1], m_triangle_bboxes[i_index2]);
  }

void TriangleAccelerator::_SwapInstances(size_t i_index1, size_t i_index2)
  {
  ASSERT(i_index1<m_instance_nodes.size());
  ASSERT(i_index2<m_instance_nodes.size());

  std::swap(m_instance_nodes[i_index1], m_instance_nodes[i_index2]);
  std::swap(m_instance_to_world_transformations[i_index1], m_instance_to_world_transformations[i_index2]);
  std::swap(m_instance_primitive_indices[i_index1], m_instance_primitive_indices[i_index2]);
  std::swap(m_instance_bboxes[i_index1], m_instance_bboxes[i_index2]);
  }

std::pair<unsigned char,double> TriangleAccelerator::_DetermineBestSplit(const BBox3D_f &i_node_bbox,
                                                          size_t i_triangles_begin, size_t i_triangles_end,
                                                          size_t i_instances_begin, size_t i_instances_end,
                                                          unsigned char i_middle_split_mask) const
  {
  ASSERT(i_middle_split_mask<(1<<3));
  size_t num_triangles = i_triangles_end-i_triangles_begin;
  size_t num_instances = i_instances_end-i_instances_begin;
  ASSERT(num_triangles+num_instances>0);

  double best_split_position[3], best_cost[3] = { DBL_INF, DBL_INF, DBL_INF };
  
  // Run it in parallel threads and let TBB parallelize it. For small numbers of triangles (plus instances) we run it in single thread by increasing the grainsize.
  size_t tbb_grain = (num_triangles+num_instances>70) ? 1 : 6;
  tbb::parallel_for(tbb::blocked_range<size_t>(0, 3, tbb_grain), [&](const tbb::blocked_range<size_t> &i_range)
    {
    for(size_t split_axis = i_range.begin(); split_axis != i_range.end(); ++split_axis) if ((i_middle_split_mask&(1<<split_axis)) == 0)
      {
      /*
      * First precompute the needed information for all split positions.
      * The information for each split is the bounding boxes of the child and number of triangles and instances in each child.
      */
      BBox3D_d left_bboxes[MAX_SPLIT_TRIES], middle_bboxes[MAX_SPLIT_TRIES], right_bboxes[MAX_SPLIT_TRIES];
      size_t left_num[MAX_SPLIT_TRIES], middle_num[MAX_SPLIT_TRIES], right_num[MAX_SPLIT_TRIES];

      memset(left_num, 0, MAX_SPLIT_TRIES*sizeof(size_t));
      memset(middle_num, 0, MAX_SPLIT_TRIES*sizeof(size_t));
      memset(right_num, 0, MAX_SPLIT_TRIES*sizeof(size_t));

      size_t num_tries = std::min(MAX_SPLIT_TRIES, 2*(i_triangles_end-i_triangles_begin) + 2*(i_instances_end-i_instances_begin));
      double coef = num_tries/(i_node_bbox.m_max[split_axis]-i_node_bbox.m_min[split_axis]);
      for (size_t i = 0; i<num_triangles+num_instances; ++i)
        {
        const BBox3D_f &bbox = i<num_triangles ? m_triangle_bboxes[i_triangles_begin+i] : m_instance_bboxes[i_instances_begin+i-num_triangles];

        size_t lefts_begin = std::min(num_tries, (size_t)((bbox.m_max[split_axis]-i_node_bbox.m_min[split_axis]) * coef + 1.0));
        size_t rights_end = std::min(num_tries, (size_t)((bbox.m_min[split_axis]-i_node_bbox.m_min[split_axis]) * coef + 1.0));
        ASSERT(lefts_begin<=num_tries && rights_end<=num_tries && rights_end<=lefts_begin);

        /*
        * Here's the trickiest part.
        * Each bbox affects all left_bboxes and left_num values starting from the bbox's max coordinate.
        * Each bbox affects all right_bboxes and right_num values ending with the bbox's min coordinate.
        * There's no need to iterate through the entire range of these values, we can only set the starting/ending value and then accumulate the final values after the loop by bboxes.
        *
        * This does not work for middle_bboxes and middle_num though, so these values need to be updated in an internal loop.
        */

        size_t triangles_count = i<num_triangles ? 1 : m_instance_nodes[i_instances_begin+i-num_triangles]->m_triangles_end-m_instance_nodes[i_instances_begin+i-num_triangles]->m_triangles_begin;
        if (rights_end>0)
          {
          right_bboxes[rights_end-1].Unite(bbox);
          right_num[rights_end-1] += triangles_count;
          }

        if (lefts_begin<num_tries)
          {
          left_bboxes[lefts_begin].Unite(bbox);
          left_num[lefts_begin] += triangles_count;
          }

        for (size_t j = rights_end; j<lefts_begin; ++j)
          {
          middle_bboxes[j].Unite(bbox);
          middle_num[j] += triangles_count;
          }
        }

      // Accumulate left_bboxes and left_num values.
      for (size_t i = 1; i<num_tries; ++i)
        {
        left_bboxes[i].Unite(left_bboxes[i-1]);
        left_num[i] += left_num[i-1];
        }

      // Accumulate right_bboxes and right_num values.
      for (int i = (int)num_tries-2; i>=0; --i)
        {
        right_bboxes[i].Unite(right_bboxes[i+1]);
        right_num[i] += right_num[i+1];
        }

      double inv_area = 1.0/i_node_bbox.Area();
      for (size_t i = 0; i<num_tries; ++i)
        {
        // Compute probability that a random ray intersects the children given that it intersects the node.
        double p1 = std::min(1.0, left_bboxes[i].Area()*inv_area);
        double p2 = std::min(1.0, middle_bboxes[i].Area()*inv_area);
        double p3 = std::min(1.0, right_bboxes[i].Area()*inv_area);

        // The cost function assumes that the children are all leaves.
        double cost = left_num[i]*p1+middle_num[i]*p2+right_num[i]*p3;

        // This is the heuristics that encourages splits resulting in empty left or right children nodes.
        if (left_num[i]==0 || right_num[i]==0) cost *= 0.8;

        if (cost<best_cost[split_axis])
          {
          best_cost[split_axis] = cost;
          best_split_position[split_axis] = i_node_bbox.m_min[split_axis] + i*(i_node_bbox.m_max[split_axis]-i_node_bbox.m_min[split_axis])/num_tries;
          }
        }
      }
    });

  // Determine the best split axis and return the result.
  if (best_cost[0]<=best_cost[1] && best_cost[0]<=best_cost[2])
    return std::make_pair(0, best_split_position[0]);
  else if (best_cost[1]<=best_cost[0] && best_cost[1]<=best_cost[2])
    return std::make_pair(1, best_split_position[1]);
  else
    return std::make_pair(2, best_split_position[2]);
  }

//////////////////////////////////////////////////////////// NODE //////////////////////////////////////////////////////

TriangleAccelerator::Node::
  Node(TriangleAccelerator &i_accelerator,
  size_t i_triangles_begin, size_t i_triangles_end, 
  size_t i_instances_begin, size_t i_instances_end,
  unsigned char i_middle_split_mask, size_t i_depth): 
m_triangles_begin(i_triangles_begin), m_triangles_end(i_triangles_end), m_instances_begin(i_instances_begin), m_instances_end(i_instances_end),
m_bbox(i_accelerator._ConstructBBox(i_triangles_begin, i_triangles_end, i_instances_begin, i_instances_end))
  {

  // Create leaf if there are less triangles than MAX_TRIANGLES_IN_LEAF, if the maximum tree depth is reached or
  // if there's no effective split possible (this is the case if the node has three middle splits on its path back to the root).
  if ((i_triangles_end-i_triangles_begin) + (i_instances_end-i_instances_begin) <= MAX_TRIANGLES_IN_LEAF || i_middle_split_mask == (1<<3)-1 || i_depth>=MAX_TREE_DEPTH)
    {
    SetType(true,0);
    return;
    }

  std::pair<unsigned char,double> split = i_accelerator._DetermineBestSplit(m_bbox, i_triangles_begin, i_triangles_end, i_instances_begin, i_instances_end, i_middle_split_mask);
  unsigned char split_axis = split.first;
  double split_coord = split.second;

  SetType(false, split_axis);

  size_t triangles_middle_begin=i_triangles_begin;
  size_t triangles_right_begin=i_triangles_end;
  size_t i=i_triangles_begin;

  // Partition the triangles in three regions:
  // 1. The triangles lying below the splitting plane.
  // 2. The triangles intersecting the splitting plane.
  // 3. The triangles lying above the splitting plane.
  while(i<triangles_right_begin)
    {
    const BBox3D_f &bbox = i_accelerator.m_triangle_bboxes[i];

    if (bbox.m_max[split_axis] < split_coord)
      {
      // First sub-region.
      i_accelerator._SwapTriangles(triangles_middle_begin,i);
      ++triangles_middle_begin;
      ++i;
      }
    else
      if (bbox.m_min[split_axis] > split_coord)
        {
        // Third sub-region.
        --triangles_right_begin;
        i_accelerator._SwapTriangles(i,triangles_right_begin);
        }
      else
        // Second sub-region.
        ++i;
    }

  size_t instances_middle_begin=i_instances_begin;
  size_t instances_right_begin=i_instances_end;
  i=i_instances_begin;

  // Now partition the instances in the same way.
  while(i<instances_right_begin)
    {
    const BBox3D_f &bbox = i_accelerator.m_instance_bboxes[i];

    if (bbox.m_max[split_axis] < split_coord)
      {
      // First sub-region.
      i_accelerator._SwapInstances(instances_middle_begin,i);
      ++instances_middle_begin;
      ++i;
      }
    else
      if (bbox.m_min[split_axis] > split_coord)
        {
        // Third sub-region.
        --instances_right_begin;
        i_accelerator._SwapInstances(i,instances_right_begin);
        }
      else
        // Second sub-region.
        ++i;
    }

  // Create left child node.
  if (i_triangles_begin<triangles_middle_begin || i_instances_begin<instances_middle_begin)
    {
    void * ptr = i_accelerator.m_pool.Alloc(sizeof(Node));
    m_children[0] = new (ptr) Node(i_accelerator, i_triangles_begin, triangles_middle_begin, i_instances_begin, instances_middle_begin, i_middle_split_mask, i_depth+1);
    }
  else
    m_children[0] = NULL;

  // Create middle child node.
  if (triangles_middle_begin<triangles_right_begin || instances_middle_begin<instances_right_begin)
    {
    void * ptr = i_accelerator.m_pool.Alloc(sizeof(Node));
    m_children[1] = new (ptr) Node(i_accelerator, triangles_middle_begin, triangles_right_begin, instances_middle_begin, instances_right_begin, i_middle_split_mask | (1<<split_axis), i_depth+1);
    }
  else
    m_children[1] = NULL;

  // Create right child node.
  if (triangles_right_begin<i_triangles_end || instances_right_begin<i_instances_end)
    {
    void * ptr = i_accelerator.m_pool.Alloc(sizeof(Node));
    m_children[2] = new (ptr) Node(i_accelerator, triangles_right_begin, i_triangles_end, instances_right_begin, i_instances_end, i_middle_split_mask, i_depth+1);
    }
  else
    m_children[2] = NULL;
  }