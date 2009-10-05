#ifndef TRIANGLE_TREE_H
#define TRIANGLE_TREE_H

/*
This file contains temporary implementation of the acceleration tree structure.
It is not documented and will be rewritten later when there is enough of the framework to test its performance.
It is also not covered by the unit tests yet.
*/

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "TriangleMesh.h"
#include "Primitive.h"
#include "Intersection.h"
#include <boost\pool\object_pool.hpp>
#include <vector>

class TriangleTree
  {
  //////////////////////////////////////////////////// INTERNAL TYPES ///////////////////////////////////////////////////////
  private:   
    struct BaseNode;
    struct Node;
    struct Leaf;

    ////////////////////////////////////////////////// PUBLIC INTERFACE /////////////////////////////////////////////////////
  public:
    TriangleTree(std::vector<intrusive_ptr<Primitive> > i_primitives);
    ~TriangleTree();

    ////////////////////////////////////////////////////// QUERIES  /////////////////////////////////////////////////////////

    // Search for the triangle nearest to the i_point along the i_direction.
    bool Intersect(const RayDifferential &i_ray, Intersection &o_intersection) const;

    bool IntersectTest(const Ray &i_ray) const;

    BBox3D_d GetWorldBounds() const;

  private:
    TriangleTree(TriangleTree &);
    TriangleTree &operator=(TriangleTree &);

    // Destroy the tree (if it is built).
    void _DestroyTree();

    /*
    Build the tree.
    If the tree is already built this method forces its rebuilding (it can be needed if the mesh was changed after the last construction).
    The container (Wrapper) should be set before this method is called.
    */
    void _BuildTree();

    BBox3D_f _ConstructBBox(size_t i_begin, size_t i_end) const;
    void _SwapTriangles(size_t i_index1, size_t i_index2);

    //////////// DATA MEMBERS ////////////
  private:
    // Internal list of triangles.
    std::vector<Triangle3D_f> m_triangles;
    std::vector<size_t> m_primitive_indices;
    std::vector<size_t> m_triangle_indices;

    std::vector<intrusive_ptr<Primitive> > m_primitives;

    // The root node of the tree.
    BaseNode *mp_root;

    // Pools for nodes and leaves allocation.
    boost::pool<> m_NodePool;
    boost::pool<> m_LeafPool;

    // This constant value defines the number of elements to be allocated by boost allocator when the pool runs out.
    static const size_t NODES_TO_ALLOCATE = 1024;

    static const size_t TRIANGLES_IN_LEAF = 10;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

//Represents the base class for the tree elements: nodes and leaves.
struct TriangleTree::BaseNode
  {
  // Bounding box of all the triangles in the sub-tree rooted at this node.
  BBox3D_f m_bbox;

  // Indicates the interval (begin and end iterators) of the internal triangles list (see m_triangles field) that contains
  // the triangles in the sub-tree rooted at this node.
  size_t m_begin, m_end;

  BaseNode(size_t i_begin, size_t i_end);
  virtual ~BaseNode() {}

  /*
  Pure virtual method that returns the triangle nearest to the i_point along the i_direction in the sub-tree rooted at this node.
  INVALID_TRIANGLE is returned if no triangle is found.
  io_max_dist is set to the distance to the nearest triangle along the specified direction.
  */
  virtual void Intersect(Ray &i_ray, size_t &o_triangle_index) const = 0;

  virtual bool IntersectTest(const Ray &i_ray) const = 0;
  };

/*
Represents the node of the tree, i.e. tree element that has children.
Each node has exactly three children, so called "left", "medium" and "right".
These children represent the triangles classified by the dividing plane, i.e. x, y or z-constant plane that splits the bounding box of
the node in two equal parts (i.e. intersects the bounding box in the middle of its x,y or z dimension).
Left child (m_children[0]) represents the triangles that lie completely in the left subpart of the subdivided bounding box.
Medium child (m_children[1]) represents the triangles that intersect the dividing plane.
Right child (m_children[2]) represents the triangles that lie completely in the right subpart of the subdivided bounding box.

Note that each triangle of this node will get in exactly one child sub-tree of this node.
*/
struct TriangleTree::Node : public TriangleTree::BaseNode
  {
  // Children nodes.
  BaseNode *m_children[3];

  Node(TriangleTree &i_tree, size_t i_begin, size_t i_end, char i_split_index, char i_middle_split_mask);
  virtual ~Node() {}

  // The method below is implementation of pure virtual method defined in the base class. Please refer there for the details.
  virtual void Intersect(Ray &i_ray, size_t &o_triangle_index) const;

  virtual bool IntersectTest(const Ray &i_ray) const;
  };

/*
Represents the leaf of the tree, i.e. tree element that has no children.
*/
struct TriangleTree::Leaf : public TriangleTree::BaseNode
  {
  const TriangleTree &m_tree;

  Leaf(const TriangleTree &i_tree, size_t i_begin, size_t i_end);
  virtual ~Leaf() {}

  // The method below is implementation of pure virtual method defined in the base class. Please refer there for the details.
  virtual void Intersect(Ray &i_ray, size_t &o_triangle_index) const;

  virtual bool IntersectTest(const Ray &i_ray) const;
  };

#endif // TRIANGLE_TREE_H