#ifndef TRIANGLE_TREE_H
#define TRIANGLE_TREE_H

#include <Common\Common.h>
#include <Math\Geometry.h>
#include "TriangleMesh.h"
#include <boost\pool\object_pool.hpp>
#include <vector>

struct IntersectResult
  {
  bool m_intersection_found;
  shared_ptr<TriangleMesh> mp_mesh;
  size_t m_triangle_index;
  };

class TriangleTree
  {
  //////////////////////////////////////////////////// INTERNAL TYPES ///////////////////////////////////////////////////////
  private:   
    struct BaseNode;
    struct Node;
    struct Leaf;

    ////////////////////////////////////////////////// PUBLIC INTERFACE /////////////////////////////////////////////////////
  public:
    TriangleTree();
    ~TriangleTree();

    // Call this method to set the container (Wrapper). This method destroys the tree if has already been constructed for some another Wrapper.
    void AddTriangleMesh(shared_ptr<TriangleMesh> ip_triangle_mesh);

    /*
    Build the tree.
    If the tree is already built this method forces its rebuilding (it can be needed if the mesh was changed after the last construction).
    The container (Wrapper) should be set before this method is called.
    */
    void BuildTree();

    ////////////////////////////////////////////////////// QUERIES  /////////////////////////////////////////////////////////

    // Search for the triangle nearest to the i_point along the i_direction.
    IntersectResult Intersect(const Ray &i_ray) const;

    bool IntersectTest(const Ray &i_ray) const;

  private:
    TriangleTree(TriangleTree &);
    TriangleTree &operator=(TriangleTree &);

    // Destroy the tree (if it is built).
    void _DestroyTree();

    BBox3D_f _ConstructBBox(size_t i_begin, size_t i_end) const;
    void _SwapTriangles(size_t i_index1, size_t i_index2);

    //////////// DATA MEMBERS ////////////
  private:
    // Internal list of triangles.
    std::vector<Triangle3D_f> m_triangles;
    std::vector<size_t> m_mesh_indices;
    std::vector<size_t> m_triangle_indices;

    std::vector<shared_ptr<TriangleMesh> > m_meshes;

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