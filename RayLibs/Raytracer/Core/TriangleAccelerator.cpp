#include "TriangleAccelerator.h"
#include "TriangleMesh.h"
#include <numeric>
#include <cstring>

TriangleAccelerator::TriangleAccelerator(std::vector<intrusive_ptr<const Primitive> > i_primitives):
mp_root(NULL), m_primitives(i_primitives), m_pool(100000*sizeof(TriangleAccelerator::Node))
  {
  size_t number_of_triangles=0;
  for(size_t i=0;i<m_primitives.size();++i)
    number_of_triangles += m_primitives[i]->GetTriangleMesh()->GetNumberOfTriangles();

  m_triangles.resize(number_of_triangles);
  m_primitive_indices.resize(number_of_triangles);
  m_triangle_indices.resize(number_of_triangles);

  std::vector<BBox3D_f> bboxes(number_of_triangles);

  size_t index = 0;
  for(size_t i=0;i<m_primitives.size();++i)
    {
    intrusive_ptr<const TriangleMesh> p_mesh=m_primitives[i]->GetTriangleMesh();
    for(size_t j=0;j<p_mesh->GetNumberOfTriangles();++j)
      {
      MeshTriangle triangle = p_mesh->GetTriangle(j);
      m_triangles[index]=Triangle3D_f(
        p_mesh->GetVertex(triangle.m_vertices[0]),
        p_mesh->GetVertex(triangle.m_vertices[1]),
        p_mesh->GetVertex(triangle.m_vertices[2]));

      m_primitive_indices[index]=i;
      m_triangle_indices[index]=j;

      bboxes[index].Unite(m_triangles[index][0]);
      bboxes[index].Unite(m_triangles[index][1]);
      bboxes[index].Unite(m_triangles[index][2]);

      ++index;
      }
    }

  void * ptr = m_pool.Alloc(sizeof(Node));
  mp_root = new (ptr) Node(*this, bboxes, 0, m_triangles.size(), 0, 0);
  }

BBox3D_d TriangleAccelerator::GetWorldBounds() const
  {
  return Convert<double>(mp_root->m_bbox);
  }

bool TriangleAccelerator::Intersect(const RayDifferential &i_ray, Intersection &o_intersection, double *o_t) const
  {
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());
  Ray ray(i_ray.m_base_ray);

  double invs[3];
  invs[0]=1.0/ray.m_direction[0];
  invs[1]=1.0/ray.m_direction[1];
  invs[2]=1.0/ray.m_direction[2];

  int triangle_index = -1;

  typedef Node *NodePtr;
  NodePtr todo[2*MAX_TREE_DEPTH+1];
  todo[0]=mp_root;
  int todo_size=1;

  while (todo_size>0)
    {
    ASSERT(todo_size<=2*MAX_TREE_DEPTH+1);
    Node *p_node = todo[--todo_size];

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

      for(size_t i=p_node->m_begin;i<p_node->m_end;++i)
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
        if(b1 < -DBL_EPS || b1 > (1.0+DBL_EPS) || divisor==0.0)
          continue;

        // Compute second barycentric coordinate.
        Vector3D_d s2 = d^e1;
        double b2 = (ray.m_direction*s2) * inv_divisor;
        if(b2 < -DBL_EPS || b1 + b2 > (1.0+DBL_EPS))
          continue;

        // Compute t to intersection point.
        double t = (e2*s2) * inv_divisor;
        if (t >= ray.m_min_t && t <= ray.m_max_t)
          {
          ray.m_max_t = t;
          triangle_index=i;
          }
        }

      }

    }

  if (triangle_index != -1)
    {
    if (o_t) *o_t = ray.m_max_t;
    const TriangleMesh *p_mesh = m_primitives[m_primitive_indices[triangle_index]]->GetTriangleMesh_RawPtr();

    o_intersection.mp_primitive = m_primitives[m_primitive_indices[triangle_index]].get();
    o_intersection.m_triangle_index = m_triangle_indices[triangle_index];
    p_mesh->ComputeDifferentialGeometry(o_intersection.m_triangle_index, i_ray, o_intersection.m_dg);
  
    // Save the triangle information that will be used for determining epsilon value when a next outgoing ray is being shot.
    MeshTriangle triangle=p_mesh->GetTriangle(o_intersection.m_triangle_index);
    Point3D_d v0 = Convert<double>(p_mesh->GetVertex(triangle.m_vertices[0]));
    Point3D_d v1 = Convert<double>(p_mesh->GetVertex(triangle.m_vertices[1]));
    Point3D_d v2 = Convert<double>(p_mesh->GetVertex(triangle.m_vertices[2]));

    o_intersection.m_cross =  Vector3D_d(v1-v0)^Vector3D_d(v2-v0);
    o_intersection.m_dot = (Vector3D_d(v0-o_intersection.m_dg.m_point) ^ Vector3D_d(v1-v0)) * Vector3D_d(v2-v0);

    return true;
    }

  if (o_t) *o_t=DBL_INF;
  return false;
  }


bool TriangleAccelerator::IntersectTest(Ray i_ray) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());
  double invs[3];
  invs[0]=1.0/i_ray.m_direction[0];
  invs[1]=1.0/i_ray.m_direction[1];
  invs[2]=1.0/i_ray.m_direction[2];

  typedef Node *NodePtr;
  NodePtr todo[2*MAX_TREE_DEPTH+1];
  todo[0]=mp_root;
  int todo_size=1;

  while (todo_size>0)
    {
    ASSERT(todo_size<=2*MAX_TREE_DEPTH+1);
    Node *p_node = todo[--todo_size];

    // Check whether the ray intersects the bbox of the node.
    double tNear1 = (p_node->m_bbox.m_min[0] - i_ray.m_origin[0]) * invs[0];
    double tFar1  = (p_node->m_bbox.m_max[0] - i_ray.m_origin[0]) * invs[0];
    if (tNear1 > tFar1) std::swap(tNear1, tFar1);

    double tNear2 = (p_node->m_bbox.m_min[1] - i_ray.m_origin[1]) * invs[1];
    double tFar2  = (p_node->m_bbox.m_max[1] - i_ray.m_origin[1]) * invs[1];
    if (tNear2 > tFar2) std::swap(tNear2, tFar2);

    double tNear3 = (p_node->m_bbox.m_min[2] - i_ray.m_origin[2]) * invs[2];
    double tFar3  = (p_node->m_bbox.m_max[2] - i_ray.m_origin[2]) * invs[2];
    if (tNear3 > tFar3) std::swap(tNear3, tFar3);

    if (
      i_ray.m_min_t > tFar1 || tNear1 > tFar2 || tNear2 > tFar1 || tNear3 > tFar1 ||
      i_ray.m_min_t > tFar2 || tNear1 > tFar3 || tNear2 > tFar3 || tNear3 > tFar2 ||
      i_ray.m_min_t > tFar3 || tNear1 > i_ray.m_max_t || tNear2 > i_ray.m_max_t || tNear3 > i_ray.m_max_t)
      continue;

    if (p_node->IsLeaf()==false)
      {
      int axis = p_node->GetSplitAxis();

      // Insert the children in a reverse order because this is a stack and they are pulled back from the top.
      if (i_ray.m_direction[axis]>0.0)
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

      for(size_t i=p_node->m_begin;i<p_node->m_end;++i)
        {
        const Triangle3D_f &triangle = m_triangles[i];

        Point3D_d v0 = Convert<double>(triangle[0]);
        Point3D_d v1 = Convert<double>(triangle[1]);
        Point3D_d v2 = Convert<double>(triangle[2]);

        Vector3D_d e1 = Vector3D_d(v1-v0);
        Vector3D_d e2 = Vector3D_d(v2-v0);
        Vector3D_d s1 = i_ray.m_direction^e2;
        double divisor = s1*e1;
        double inv_divisor = 1.0/divisor;

        // Compute first barycentric coordinate.
        Vector3D_d d = Vector3D_d(i_ray.m_origin - v0);
        double b1 = (d*s1) * inv_divisor;
        if(b1 < -DBL_EPS || b1 > (1.0+DBL_EPS) || divisor==0.0)
          continue;

        // Compute second barycentric coordinate.
        Vector3D_d s2 = d^e1;
        double b2 = (i_ray.m_direction*s2) * inv_divisor;
        if(b2 < -DBL_EPS || b1 + b2 > (1.0+DBL_EPS))
          continue;

        // Compute t to intersection point.
        double t = (e2*s2) * inv_divisor;
        if (t >= i_ray.m_min_t && t <= i_ray.m_max_t)
          return true;
        }

      }

    }

  return false;
  }

BBox3D_d TriangleAccelerator::_ConstructBBox(const std::vector<BBox3D_f> &i_bboxes, size_t i_begin, size_t i_end) const
  {
  BBox3D_d bbox;

  for(size_t i=i_begin;i!=i_end;++i)
    bbox.Unite(i_bboxes[i]);

  return bbox;
  }

void TriangleAccelerator::_SwapTriangles(size_t i_index1, size_t i_index2, std::vector<BBox3D_f> &i_bboxes)
  {
  ASSERT(i_index1<m_triangles.size());
  ASSERT(i_index2<m_triangles.size());

  std::swap(m_triangles[i_index1], m_triangles[i_index2]);
  std::swap(m_primitive_indices[i_index1], m_primitive_indices[i_index2]);
  std::swap(m_triangle_indices[i_index1], m_triangle_indices[i_index2]);
  std::swap(i_bboxes[i_index1], i_bboxes[i_index2]);
  }

std::pair<unsigned char,double> TriangleAccelerator::_DetermineBestSplit(TriangleAccelerator &i_accelerator,
                                                          std::vector<BBox3D_f> &i_bboxes,
                                                          const BBox3D_d &i_node_bbox,
                                                          size_t i_begin, 
                                                          size_t i_end, 
                                                          unsigned char i_middle_split_mask)
  {
  ASSERT(i_begin<i_end && i_middle_split_mask<(1<<3));
  std::pair<unsigned char,double> ret;
  double best_cost = DBL_INF;

  // Try all possible split axes to find the best one.
  for(unsigned char split_axis=0;split_axis<3;++split_axis)
    if ((i_middle_split_mask&(1<<split_axis)) == 0)
      {
      /*
      * First precompute the needed information for all split positions.
      * The information for each split is the bounding boxes of the child and number of triangles in each child.
      */
      BBox3D_d left_bboxes[MAX_SPLIT_TRIES], middle_bboxes[MAX_SPLIT_TRIES], right_bboxes[MAX_SPLIT_TRIES];
      size_t left_num[MAX_SPLIT_TRIES], middle_num[MAX_SPLIT_TRIES], right_num[MAX_SPLIT_TRIES];

      memset(left_num, 0, MAX_SPLIT_TRIES*sizeof(size_t));
      memset(middle_num, 0, MAX_SPLIT_TRIES*sizeof(size_t));
      memset(right_num, 0, MAX_SPLIT_TRIES*sizeof(size_t));

      size_t num_tries = std::min(MAX_SPLIT_TRIES, 2*(i_end-i_begin));
      double coef = num_tries/(i_node_bbox.m_max[split_axis]-i_node_bbox.m_min[split_axis]);
      for(size_t i=i_begin;i<i_end;++i)
        {
        const BBox3D_f &bbox = i_bboxes[i];

        size_t lefts_begin = std::min(num_tries, (size_t)( (bbox.m_max[split_axis]-i_node_bbox.m_min[split_axis]) * coef + 1.0));
        size_t rights_end =  std::min(num_tries, (size_t)( (bbox.m_min[split_axis]-i_node_bbox.m_min[split_axis]) * coef + 1.0));
        ASSERT(lefts_begin<=num_tries && rights_end<=num_tries && rights_end<=lefts_begin);

        /*
        * Here's the trickiest part.
        * Each triangle affects all left_bboxes and left_num values starting from the triangle's max coordinate.
        * Each triangle affects all right_bboxes and right_num values ending with the triangle's min coordinate.
        * There's no need to iterate through the entire range of these values, we can only set the starting/ending value and then accumulate the final values after the loop by triangles.
        * 
        * This does not work for middle_bboxes and middle_num though, so these values need to be updated in an internal loop.
        */

        if (rights_end>0)
          {
          right_bboxes[rights_end-1].Unite(bbox);
          ++right_num[rights_end-1];
          }

        if (lefts_begin<num_tries)
          {
          left_bboxes[lefts_begin].Unite(bbox);
          ++left_num[lefts_begin];
          }

        for(size_t j=rights_end;j<lefts_begin;++j)
          {
          middle_bboxes[j].Unite(bbox);
          ++middle_num[j];
          }
        }

      // Accumulate left_bboxes and left_num values.
      for(size_t i=1;i<num_tries;++i)
        {
        left_bboxes[i].Unite(left_bboxes[i-1]);
        left_num[i]+=left_num[i-1];
        }

      // Accumulate right_bboxes and right_num values.
      for(int i=(int)num_tries-2;i>=0;--i)
        {
        right_bboxes[i].Unite(right_bboxes[i+1]);
        right_num[i]+=right_num[i+1];
        }

      double inv_area = 1.0/i_node_bbox.Area();
      for (size_t i=0;i<num_tries;++i)
        {
        // Compute probability that a random ray intersects the children given that it intersects the node.
        double p1=std::min(1.0, left_bboxes[i].Area()*inv_area);
        double p2=std::min(1.0, middle_bboxes[i].Area()*inv_area);
        double p3=std::min(1.0, right_bboxes[i].Area()*inv_area);

        ASSERT(left_num[i]+middle_num[i]+right_num[i] == i_end-i_begin);

        // The cost function assumes that the children are all leaves.
        double cost = left_num[i]*p1+middle_num[i]*p2+right_num[i]*p3;

        // This is the heuristics that encourages splits resulting in empty left or right children nodes.
        if (left_num[i]==0 || right_num[i]==0) cost*=0.8;

        if (cost<best_cost)
          {
          best_cost = cost;
          ret.first = split_axis;
          ret.second = i_node_bbox.m_min[split_axis] + i*(i_node_bbox.m_max[split_axis]-i_node_bbox.m_min[split_axis])/num_tries;
          }
        }
      }

  return ret;
  }

//////////////////////////////////////////////////////////// NODE //////////////////////////////////////////////////////

TriangleAccelerator::Node::
  Node(TriangleAccelerator &i_accelerator,
  std::vector<BBox3D_f> &i_bboxes,
  size_t i_begin, 
  size_t i_end, 
  unsigned char i_middle_split_mask,
  size_t i_depth): 
m_begin(i_begin), m_end(i_end), m_bbox(i_accelerator._ConstructBBox(i_bboxes, i_begin, i_end))
  {

  // Create leaf if there's less triangles than MAX_TRIANGLES_IN_LEAF, if the maximum tree depth is reached or
  // if there's no effective split possible (this is the case if the node has three middle splits on its path back to the root).
  if (i_end-i_begin<=MAX_TRIANGLES_IN_LEAF || i_middle_split_mask == (1<<3)-1 || i_depth>=MAX_TREE_DEPTH)
    {
    SetType(true,0);
    return;
    }

  std::pair<unsigned char,double> split = i_accelerator._DetermineBestSplit(i_accelerator, i_bboxes, m_bbox, i_begin, i_end, i_middle_split_mask);
  unsigned char split_axis = split.first;
  double split_coord = split.second;

  SetType(false, split_axis);

  size_t middle_begin=i_begin;
  size_t right_begin=i_end;
  size_t i=i_begin;

  // Partition the triangles in three regions:
  // 1. The triangles lying below the splinting plane.
  // 2. The triangles intersecting the splinting plane.
  // 3. The triangles lying above the splinting plane.
  while(i<right_begin)
    {
    const BBox3D_f &bbox = i_bboxes[i];

    if (bbox.m_max[split_axis] < split_coord)
      {
      // First sub-region.
      i_accelerator._SwapTriangles(middle_begin,i, i_bboxes);
      ++middle_begin;
      ++i;
      }
    else
      if (bbox.m_min[split_axis] > split_coord)
        {
        // Third sub-region.
        --right_begin;
        i_accelerator._SwapTriangles(i,right_begin, i_bboxes);
        }
      else
        // Second sub-region.
        ++i;
    }

  // Create left child node.
  if (i_begin<middle_begin)
    {
    void * ptr = i_accelerator.m_pool.Alloc(sizeof(Node));
    m_children[0] = new (ptr) Node (i_accelerator, i_bboxes, i_begin, middle_begin, i_middle_split_mask, i_depth+1);
    }
  else
    m_children[0] = NULL;

  // Create middle child node.
  if (middle_begin<right_begin)
    {
    void * ptr = i_accelerator.m_pool.Alloc(sizeof(Node));
    m_children[1] = new (ptr) Node(i_accelerator, i_bboxes, middle_begin, right_begin, i_middle_split_mask | (1<<split_axis), i_depth+1);
    }
  else
    m_children[1] = NULL;

  // Create right child node.
  if (right_begin<i_end)
    {
    void * ptr = i_accelerator.m_pool.Alloc(sizeof(Node));
    m_children[2] = new (ptr) Node(i_accelerator ,i_bboxes, right_begin, i_end, i_middle_split_mask, i_depth+1);
    }
  else
    m_children[2] = NULL;
  }