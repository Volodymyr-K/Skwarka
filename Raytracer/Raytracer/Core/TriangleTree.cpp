#include "TriangleTree.h"

template<typename T>
bool Intersect(const BBox3D<T> &i_bbox, const RayInv &i_ray)
  {
  double t0 = i_ray.m_ray.m_min_t, t1 = i_ray.m_ray.m_max_t;
  for (int i = 0; i < 3; ++i)
    {
    // Update interval for i-th bounding box slab.
    // No need to check for zero i_ray.m_direction[i] due to the way INF values behave.
    double tNear = (i_bbox.m_min[i] - i_ray.m_ray.m_origin[i]) * i_ray.m_invs[i];
    double tFar  = (i_bbox.m_max[i] - i_ray.m_ray.m_origin[i]) * i_ray.m_invs[i];

    if (tNear > tFar) std::swap(tNear, tFar);
    t0 = tNear > t0 ? tNear : t0;
    t1 = tFar  < t1 ? tFar  : t1;
    if (t0 > t1) return false;
    }
  return true;
  }

TriangleTree::TriangleTree(std::vector<intrusive_ptr<Primitive> > i_primitives):
mp_root(NULL), m_primitives(i_primitives),
m_NodePool(sizeof(TriangleTree::Node), NODES_TO_ALLOCATE),
m_LeafPool(sizeof(TriangleTree::Leaf), NODES_TO_ALLOCATE)
  {
  _BuildTree();
  }

TriangleTree::~TriangleTree()
  {
  _DestroyTree();
  }

BBox3D_d TriangleTree::GetWorldBounds() const
  {
  return Convert<double>(mp_root->m_bbox);
  }

/*
Build the tree.
If the tree is already built this method forces its rebuilding (it can be needed if the mesh was changed after the last construction).
The container (Wrapper) should be set before this method is called.
*/

void TriangleTree::_BuildTree()
  {
  if (mp_root != NULL)
    {
    //Log::Warning("Trying to build a triangle tree that is already built. Skipping.");
    return;
    }

  size_t number_of_triangles=0;
  for(size_t i=0;i<m_primitives.size();++i)
    number_of_triangles+=m_primitives[i]->GetTriangleMesh()->GetNumberOfTriangles();

  if (number_of_triangles==0)
    {
    //Log::Warning("Trying to build an empty tree. Skipping.");
    return;
    }

  m_triangles.resize(number_of_triangles);
  m_primitive_indices.resize(number_of_triangles);
  m_triangle_indices.resize(number_of_triangles);

  size_t index = 0;
  for(size_t i=0;i<m_primitives.size();++i)
    {
    intrusive_ptr<TriangleMesh> p_mesh=m_primitives[i]->GetTriangleMesh();
    for(size_t j=0;j<p_mesh->GetNumberOfTriangles();++j)
      {
      MeshTriangle triangle = p_mesh->GetTriangle(j);
      m_triangles[index]=Triangle3D_f(
        p_mesh->GetVertex(triangle.m_vertices[0]),
        p_mesh->GetVertex(triangle.m_vertices[1]),
        p_mesh->GetVertex(triangle.m_vertices[2]));

      m_primitive_indices[index]=i;
      m_triangle_indices[index]=j;
      ++index;
      }
    }

  /*
  Create the root node of the tree.
  If there are no more than TRIANGLES_IN_LEAF triangles in the whole mesh the leaf will be created instead of the node.
  */
  if (number_of_triangles <= TRIANGLES_IN_LEAF)
    {
    void * ptr = m_LeafPool.malloc();
    mp_root = new (ptr) Leaf(*this,0,m_triangles.size());
    }
  else
    {
    void * ptr = m_NodePool.malloc();
    mp_root = new (ptr) Node(*this, 0, m_triangles.size(),0,0);
    }

  ASSERT(mp_root != NULL);
  }


void TriangleTree::_DestroyTree()
  {
  mp_root=NULL;
  m_NodePool.purge_memory();
  m_LeafPool.purge_memory();
  }

BBox3D_f TriangleTree::_ConstructBBox(size_t i_begin, size_t i_end) const
  {
  BBox3D_f bbox;

  bbox.m_min=Point3D_f(FLT_INF,FLT_INF,FLT_INF);
  bbox.m_max=Point3D_f(-FLT_INF,-FLT_INF,-FLT_INF);

  for(size_t i=i_begin;i!=i_end;++i)
    for(char j=0;j<3;++j)
      {
      Point3D_f pnt=m_triangles[i][j];

      if (pnt[0]<bbox.m_min[0]) bbox.m_min[0]=pnt[0];
      if (pnt[0]>bbox.m_max[0]) bbox.m_max[0]=pnt[0];

      if (pnt[1]<bbox.m_min[1]) bbox.m_min[1]=pnt[1];
      if (pnt[1]>bbox.m_max[1]) bbox.m_max[1]=pnt[1];

      if (pnt[2]<bbox.m_min[2]) bbox.m_min[2]=pnt[2];
      if (pnt[2]>bbox.m_max[2]) bbox.m_max[2]=pnt[2];
      }

  return bbox;
  }

void TriangleTree::_SwapTriangles(size_t i_index1, size_t i_index2)
  {
  ASSERT(i_index1<m_triangles.size());
  ASSERT(i_index2<m_triangles.size());

  std::swap(m_triangles[i_index1], m_triangles[i_index2]);
  std::swap(m_primitive_indices[i_index1], m_primitive_indices[i_index2]);
  std::swap(m_triangle_indices[i_index1], m_triangle_indices[i_index2]);
  }

/*
Search for the triangle nearest to the i_point along the i_direction.
*/

bool TriangleTree::Intersect(const RayDifferential &i_ray, Intersection &o_intersection, double *o_t) const
  {
  ASSERT(mp_root && "Intersect operation is called for not built tree.");

  Ray ray(i_ray.m_base_ray);
  size_t triangle_index = mp_root->m_end;

  RayInv ray_inv;
  ray_inv.m_ray=ray;
  ray_inv.m_invs[0]=1.0/ray.m_direction[0];
  ray_inv.m_invs[1]=1.0/ray.m_direction[1];
  ray_inv.m_invs[2]=1.0/ray.m_direction[2];

  mp_root->Intersect(ray_inv, triangle_index);
  if (triangle_index != mp_root->m_end)
    {
    if (o_t) *o_t=ray_inv.m_ray.m_max_t;
    o_intersection.mp_primitive = m_primitives[m_primitive_indices[triangle_index]];
    o_intersection.m_triangle_index = m_triangle_indices[triangle_index];
    o_intersection.mp_primitive->GetTriangleMesh()->ComputeDifferentialGeometry(o_intersection.m_triangle_index, i_ray, o_intersection.m_dg);
    return true;
    }

  if (o_t) *o_t=DBL_INF;
  return false;
  }

bool TriangleTree::IntersectTest(const Ray &i_ray) const
  {
  ASSERT(mp_root && "IntersectTest operation is called for not built tree.");

  RayInv ray_inv;
  ray_inv.m_ray=i_ray;
  ray_inv.m_invs[0]=1.0/i_ray.m_direction[0];
  ray_inv.m_invs[1]=1.0/i_ray.m_direction[1];
  ray_inv.m_invs[2]=1.0/i_ray.m_direction[2];

  return mp_root->IntersectTest(ray_inv);
  }

///////////////////////////////////////////////////////// BASE NODE ////////////////////////////////////////////////////

TriangleTree::BaseNode::BaseNode(size_t i_begin, size_t i_end):
  m_begin(i_begin), m_end(i_end)
  {
  }

//////////////////////////////////////////////////////////// NODE //////////////////////////////////////////////////////

/*
The Node constructor below encapsulates the most important part of the tree construction logic.
Please refer to the CSID for the complete description of the tree structure.

i_begin and i_end parameters specify the range of triangles this Node should be initialized for.
i_split_index parameter (with possible values 0..2) defines the splitting coordinate to be used for this node (i.e. x, y or z)
ip_parent defines the parent of this node
i_middle_split_mask is the bitmask whose first, second and third bit is enabled if this node is a descendant of some middle child of a node
with x, y or z dividing coordinate respectively (i.e. with i_split_index equal to 0,1 and 2 respectively).
i_bboxes (if not null) contains the precomputed bounding boxes of each triangle.
*/

TriangleTree::Node::
Node(TriangleTree &i_tree, 
     size_t i_begin, 
     size_t i_end, 
     char i_split_index,
     char i_middle_split_mask): 
BaseNode(i_begin,i_end)
  {
  /*
  If this node is a descendant of some middle child of a node with split index equal to i_split_index then there's no point in
  subdividing by the same coordinate again.
  We look for the next coordinate (among x, y and z) by which the split has not yet been performed.
  */
  while(i_middle_split_mask&(1<<i_split_index))
    i_split_index=(i_split_index+1)%3;

  // Initialize the bounding box of the node.
  m_bbox=i_tree._ConstructBBox(i_begin,i_end);

  float divider_coord=(m_bbox.m_min[i_split_index]+m_bbox.m_max[i_split_index])/2.f;

  for(char i=0;i<3;++i) 
    m_children[i]=NULL;

  /*
  Now we need to rearrange the triangles in the interval defined by i_begin and i_end iterators so that:
  1) first go the triangles that lie completely to the left (i.e. smaller coordinate) relatively to the diving coordinate;
  2) then go the triangles that intersect the dividing plane;
  3) then go the triangles that lie completely to the right (i.e. bigger coordinate) relatively to the diving coordinate;

  This is done in the "quick sort" manner using a single-pass approach.
  */

  /*
  l1 is the end iterator of the 1st sub-region mentioned in the comments above
  r1 is the begin iterator of the 3rd sub-region mentioned in the comments above
  the 2nd sub-region mentioned above is therefore defined by [l1;r1) iterators.
  */
  size_t l1=i_begin, r1=i_end;
  size_t i=i_begin;

  while(i<r1)
    {
    Triangle3D_f triangle = i_tree.m_triangles[i];
    
    // compute the bounding box of the triangle from the triangle coordinates
    float mn=FLT_INF,mx=-FLT_INF;
    for(char j=0;j<3;++j)
      {
      Point3D_f pnt=triangle[j];
      if (pnt[i_split_index] < mn) mn = pnt[i_split_index];
      if (pnt[i_split_index] > mx) mx = pnt[i_split_index];
      }

    if (mx<divider_coord) // If the triangle belongs to the 1st sub-region mentioned in the comments above
      {
      i_tree._SwapTriangles(l1,i);
      ++l1;
      ++i;
      }
    else
      if (mn>divider_coord)  // If the triangle belongs to the 3st sub-region mentioned in the comments above
        {
        --r1;
        i_tree._SwapTriangles(i,r1);
        }
      else
        ++i; // Otherwise the triangle belongs to the 2st sub-region mentioned in the comments above
    }

  /*
  Now we have all the triangles in [i_begin;i_end) range ordered accordingly to the way described in the comments above.
  And therefore we can explicitely build the children nodes using the [i_begin;l1) triangle range for the left child,
  [l1;r1) for the middle child and [r1;i_end) for the right child.
  */

  if (i_begin<l1)
    {
    // We construct either node or leaf depending on the number of triangles in it.
    if ((l1-i_begin)<=TRIANGLES_IN_LEAF)
      {
      void * ptr = i_tree.m_LeafPool.malloc();
      m_children[0] = new (ptr) Leaf (i_tree,i_begin,l1);
      }
    else
      {
      void * ptr = i_tree.m_NodePool.malloc();
      m_children[0] = new (ptr) Node (i_tree,i_begin,l1,(i_split_index+1)%3,i_middle_split_mask);
      }
    }

  if (l1<r1)
    {
    /*
    We construct either node or leaf depending on the number of triangles in it.      

    !!! IMPORTANT NOTE: The middle child can be constructed as leaf even if the number of triangles left is greater than m_triangles_in_leaf.
    This happens if we already subdivided the triangles in this node by all three coordinates (x, y and z) and these triangles
    get in the middle node for all of them. This means that all the triangles in this node have common x, y and z coordinate and therefore
    can not be further split by any coordinate.
    */
    if ((size_t)(r1-l1)<=TRIANGLES_IN_LEAF || (i_middle_split_mask|(1<<i_split_index)) == (1<<3)-1 )
      {
      void * ptr = i_tree.m_LeafPool.malloc();
      m_children[1] = new (ptr) Leaf(i_tree,l1,r1);
      }
    else
      {
      void * ptr = i_tree.m_NodePool.malloc();
      m_children[1] = new (ptr) Node(i_tree,l1,r1,(i_split_index+1)%3,i_middle_split_mask | (1<<i_split_index));
      }
    }

  if (r1<i_end)
    {
    // We construct either node or leaf depending on the number of triangles in it.
    if ((size_t)(i_end-r1)<=TRIANGLES_IN_LEAF)
      {
      void * ptr = i_tree.m_LeafPool.malloc();
      m_children[2] = new (ptr) Leaf(i_tree,r1,i_end);
      }
    else
      {
      void * ptr = i_tree.m_NodePool.malloc();
      m_children[2] = new (ptr) Node(i_tree,r1,i_end,(i_split_index+1)%3,i_middle_split_mask);
      }
    }

  }

/*
This method returns the triangle nearest to the i_point along the i_direction in the sub-tree rooted at this node.
INVALID_TRIANGLE is returned if no triangle is found.
io_max_dist is set to the distance to the nearest triangle along the specified direction.
*/

void TriangleTree::Node::Intersect(RayInv &i_ray, size_t &o_triangle_index) const
  {
  if (::Intersect(m_bbox,i_ray)==false)
    return;

  for(unsigned char i=0;i<3;++i)
    if (m_children[i])
      m_children[i]->Intersect(i_ray, o_triangle_index);
  }

bool TriangleTree::Node::IntersectTest(const RayInv &i_ray) const
  {
  if (::Intersect(m_bbox,i_ray)==false)
    return false;

  for(unsigned char i=0;i<3;++i)
    if (m_children[i])
      if (m_children[i]->IntersectTest(i_ray))
        return true;

  return false;
  }


/////////////////////////////////////////////////////////// LEAF /////////////////////////////////////////////////////

TriangleTree::Leaf::Leaf(const TriangleTree &i_tree, size_t i_begin,  size_t i_end): 
BaseNode(i_begin,i_end), m_tree(i_tree)
  {
  // Initialize the bounding box of the leaf.
  m_bbox=i_tree._ConstructBBox(i_begin, i_end);
  }

/*
This method returns the triangle nearest to the i_point along the i_direction in the sub-tree rooted at this node.
INVALID_TRIANGLE is returned if no triangle is found.
io_max_dist is set to the distance to the nearest triangle along the specified direction.

The method simply iterates over all triangles of this leaf and checks for each of them whether it is intersected by the ray at the distance
no greater than io_max_dist. If there are such triangles the closest is returned and the io_max_dist is updated.
*/

void TriangleTree::Leaf::Intersect(RayInv &i_ray, size_t &o_triangle_index) const
  {
  if (::Intersect(m_bbox,i_ray)==false)
    return;

  for(size_t i=m_begin;i!=m_end;++i)
    {
    const Triangle3D_f &triangle = m_tree.m_triangles[i];

    Point3D_d v0 = Convert<double>(triangle[0]);
    Point3D_d v1 = Convert<double>(triangle[1]);
    Point3D_d v2 = Convert<double>(triangle[2]);

    Vector3D_d e1 = Vector3D_d(v1-v0);
    Vector3D_d e2 = Vector3D_d(v2-v0);
    Vector3D_d s1 = i_ray.m_ray.m_direction^e2;
    double divisor = s1*e1;
    if (divisor == 0.0)
      continue;
    double invDivisor = 1.0 / divisor;

    // Compute first barycentric coordinate
    Vector3D_d d = Vector3D_d(i_ray.m_ray.m_origin - v0);
    double b1 = (d*s1) * invDivisor;
    if(b1 < -DBL_EPS || b1 > 1.0+DBL_EPS)
      continue;

    // Compute second barycentric coordinate
    Vector3D_d s2 = d^e1;
    double b2 = (i_ray.m_ray.m_direction*s2) * invDivisor;
    if(b2 < -DBL_EPS || b1 + b2 > 1.0+DBL_EPS)
      continue;

    // Compute t to intersection point
    double t = (e2*s2) * invDivisor;
    if (t >= i_ray.m_ray.m_min_t && t <= i_ray.m_ray.m_max_t)
      {
      i_ray.m_ray.m_max_t = t;
      o_triangle_index=i;
      }
    } // for(size_t i=m_begin;i!=m_end;++i)
  }

bool TriangleTree::Leaf::IntersectTest(const RayInv &i_ray) const
  {
  if (::Intersect(m_bbox,i_ray)==false)
    return false;

  for(size_t i=m_begin;i!=m_end;++i)
    {
    const Triangle3D_f &triangle = m_tree.m_triangles[i];

    Point3D_d v0 = Convert<double>(triangle[0]);
    Point3D_d v1 = Convert<double>(triangle[1]);
    Point3D_d v2 = Convert<double>(triangle[2]);

    Vector3D_d e1 = Vector3D_d(v1-v0);
    Vector3D_d e2 = Vector3D_d(v2-v0);
    Vector3D_d s1 = i_ray.m_ray.m_direction^e2;
    double divisor = s1*e1;
    if (divisor == 0.0)
      continue;
    double invDivisor = 1.0 / divisor;

    // Compute first barycentric coordinate
    Vector3D_d d = Vector3D_d(i_ray.m_ray.m_origin - v0);
    double b1 = (d*s1) * invDivisor;
    if(b1 < -DBL_EPS || b1 > 1.0+DBL_EPS)
      continue;

    // Compute second barycentric coordinate
    Vector3D_d s2 = d^e1;
    double b2 = (i_ray.m_ray.m_direction*s2) * invDivisor;
    if(b2 < -DBL_EPS || b1 + b2 > 1.0+DBL_EPS)
      continue;

    // Compute t to intersection point
    double t = (e2*s2) * invDivisor;
    if (t >= i_ray.m_ray.m_min_t && t <= i_ray.m_ray.m_max_t)
      return true;
    } // for(size_t i=m_begin;i!=m_end;++i)
  return false;
  }
