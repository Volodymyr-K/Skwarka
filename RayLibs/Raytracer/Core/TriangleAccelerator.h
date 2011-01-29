#ifndef TRIANGLE_ACCELERATOR_H
#define TRIANGLE_ACCELERATOR_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include "Primitive.h"
#include "Intersection.h"
#include <vector>

/**
* The class computes intersection of rays with the primitives.
* The class constructs a kd-tree-like structure for the triangles. Each internal node of the tree splits the triangles by x,y or z coordinate and
* can have up to three child nodes which are called left, middle and right children respectively.
* The left child contains all triangles that are strictly below the splitting plane.
* The right child contains all triangles that are strictly above the splitting plane.
* The middle child contains all triangles that are intersected by the splitting plane.
* Due to the middle children each triangle corresponds to exactly one leaf and therefore no mailboxing technique is used.
* The class is thread-safe.
*/
class TriangleAccelerator
  {
  public:
    /**
    * Creates TriangleAccelerator instance for the specified primitives.
    */
    TriangleAccelerator(std::vector<intrusive_ptr<const Primitive> > i_primitives);

    /**
    * Finds intersection of the specified ray.
    * @param i_ray Input ray. Direction component should be normalized.
    * @param[out] o_intersection Resulting intersection.
    * @param[out] o_t Ray parameter that corresponds to the intersection. Optional (can be NULL).
    * @return true if an intersection is found and false otherwise.
    */
    bool Intersect(const RayDifferential &i_ray, Intersection &o_intersection, double *o_t = NULL) const;

    /**
    * Returns true if the ray intersects any triangle.
    * @param i_ray Input ray. Direction component should be normalized.
    * @return true if an intersection is found and false otherwise.
    */
    bool IntersectTest(const Ray &i_ray) const;

    /**
    * Returns bounding box of all triangles.
    */
    BBox3D_d GetWorldBounds() const;

  private:
    struct Node;

  private:
    // Not implemented, not a value type.
    TriangleAccelerator();
    TriangleAccelerator(TriangleAccelerator &);
    TriangleAccelerator &operator=(TriangleAccelerator &);

    BBox3D_d _ConstructBBox(size_t i_triangles_begin, size_t i_triangles_end, size_t i_instances_begin, size_t i_instances_end) const;

    void _SwapTriangles(size_t i_index1, size_t i_index2);
    void _SwapInstances(size_t i_index1, size_t i_index2);

    /**
    * Computes best split axis and split position for the specified internal node.
    * The best split axis and split position are the ones that have the minimum cost function. The cost function is the cost of node traversal assuming its children are all leaves.
    * The method tries many splits positions that are distributed uniformly over the node's extent.
    */
    std::pair<unsigned char,double> _DetermineBestSplit(const BBox3D_d &i_node_bbox, size_t i_triangles_begin, size_t i_triangles_end,
      size_t i_instances_begin, size_t i_instances_end, unsigned char i_middle_split_mask);
    
    /**
    * Helper method that search the nearest intersection with the specified subtree. The method recursively processes all the nested instances.
    */
    bool _NodeIntersect(const TriangleAccelerator::Node *ip_node, Ray &i_ray, int &o_primitive_index, int &o_triangle_index) const;

    /**
    * Helper method that looks for any intersection with the specified subtree. The method recursively processes all the nested instances.
    */
    bool _NodeIntersectTest(const TriangleAccelerator::Node *ip_node, const Ray &i_ray) const;

  private:
    // All the triangles of the primitives.
    std::vector<Triangle3D_f> m_triangles;

    // Indices of the primitives (in m_primitives field vector) the corresponding triangles belong to.
    std::vector<size_t> m_primitive_indices;

    // Indices of the triangles in their meshes the corresponding triangles belong to.
    std::vector<size_t> m_triangle_indices;

    std::vector<intrusive_ptr<const Primitive> > m_primitives;

    // Root node of the tree.
    Node *mp_root;

    // Memory pool that is used for allocating the nodes.
    MemoryPool m_pool;

    // Bounding boxes of the (unique) triangles and instanced objects.
    // These vectors are only used during the tree constructions.
    std::vector<BBox3D_f> m_triangle_bboxes, m_instance_bboxes;

    // Contains pointers to the subtrees associated with the instanced primitives.
    std::vector<const Node *> m_instance_nodes;

    // Contains the transformations associated with the instanced primitives.
    std::vector<Transform> m_instance_to_world_transformations;

    // Contains the indices of the primitives (in m_primitives field vector) associated with the instanced primitives.
    std::vector<size_t> m_instance_primitive_indices;

    // Maximum number of triangles in leaves. The actual number of triangles may be greater for middle children if an effective split is not possible.
    static const size_t MAX_TRIANGLES_IN_LEAF = 4;

    // Number of tries of finding best splitting coordinate.
    static const size_t MAX_SPLIT_TRIES = 200;

    // Maximum tree depth.
    static const size_t MAX_TREE_DEPTH = 200;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Internal structure for the tree nodes.
* It is used for both internal nodes and leaves.
* Contains bounding box of the triangles and instanced primitives associated with the node, pointers to children,
* begin and end iterators of the associated triangles and instanced primitives and the flags bitset.
*/
struct TriangleAccelerator::Node
  {
  // Bounding box of the triangles associated with the node.
  BBox3D_d m_bbox;

  // Pointers to children (NULL if not present).
  Node *m_children[3];

  // Begin and end iterators of the associated triangles in the tree (see TriangleAccelerator::m_instance_nodes vector).
  size_t m_triangles_begin, m_triangles_end;

  // Begin and end iterators of the associated instanced primitives in the tree (see TriangleAccelerator::m_instance_primitive_indices vector).
  size_t m_instances_begin, m_instances_end;

  // Bitset that defines whether the node is internal or a leaf. If it is internal it also defines the splitting axis (x,y or z).
  unsigned char m_flags;

  /**
  * Sets the type of the node, i.e. whether the node is internal or a leaf and the splitting axis for internal nodes.
  */
  void SetType(bool i_is_leaf, unsigned char i_split_axis);

  /**
  * Returns true if the node is leaf and false otherwise.
  */
  bool IsLeaf() const;

  /**
  * Returns split axis.
  */
  unsigned char GetSplitAxis() const;

  /**
  * Creates the Node instance.
  * The constructor recursively creates the children if the node is internal.
  * @param i_accelerator TriangleAccelerator instance the node belongs to.
  * @param i_triangles_begin Begin iterator of the corresponding triangles.
  * @param i_triangles_end End iterator of the corresponding triangles.
  * @param i_instances_begin Begin iterator of the corresponding instances.
  * @param i_instances_end End iterator of the corresponding instances.
  * @param i_middle_split_mask The bitset that defines what middle splits have been done in the ancestor nodes.
  * @param i_depth Depth of the node (0 for root).
  */
  Node(TriangleAccelerator &i_accelerator, size_t i_triangles_begin, size_t i_triangles_end, size_t i_instances_begin, size_t i_instances_end,
    unsigned char i_middle_split_mask, size_t i_depth);
  };

inline void TriangleAccelerator::Node::SetType(bool i_is_leaf, unsigned char i_split_axis)
  {
  if (i_is_leaf)
    m_flags = m_flags & (~3);
  else
    {
    ASSERT(i_split_axis >=0 && i_split_axis <= 2);
    m_flags = (unsigned char)(  (m_flags & (~3)) + i_split_axis + 1  );
    }
  }

inline bool TriangleAccelerator::Node::IsLeaf() const
  {
  return (m_flags & 3) == 0;
  }

inline unsigned char TriangleAccelerator::Node::GetSplitAxis() const
  {
  ASSERT(IsLeaf()==false);
  return (m_flags&3)-1;
  }

#endif // TRIANGLE_ACCELERATOR_H