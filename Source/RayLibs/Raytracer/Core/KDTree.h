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

#ifndef KD_TREE_H
#define KD_TREE_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <vector>
#include <utility>

/**
* A kd-tree implementation for 3D points.
* The tree supports two built-in operations: getting a point nearest to a specified one and getting N points nearest to a specified one (where N is configurable as well).
* The tree also supports any other operation which is defined by a template Processor parameter (see Lookup() method).
* For example, if TPoint3D has additional associated info about the normal it is possible to lookup for the nearest point with the normal in acceptable range of directions.
* The two built-in operations are actually implemented via specific built-in Processors.
*
* The template parameter is a 3D point type. The type must support operator[] which must return floating point type.
*/
template<typename TPoint3D>
class KDTree
  {
  public:
    // Encapsulates 3D point and squared distance to the input point.
    // This class is used as a DTO in GetNearestPoints() method.
    struct NearestPoint;

  public:
    /**
    * Creates KDTree for the specified set of points.
    */
    KDTree(const std::vector<TPoint3D> &i_points);

    /**
    * Creates KDTree for the specified set of points.
    * This version of constructor is more efficient because it takes advantage of the parameter rvalue. It moves the content of the vector instead of copying it.
    */
    KDTree(std::vector<TPoint3D> &&i_points);

    /**
    * Returns number of points stored in the tree.
    */
    size_t GetNumberOfPoints() const;

    /**
    * Returns constant reference to a vector containing all points in the tree.
    */
    const std::vector<TPoint3D> &GetAllPoints() const;

    /**
    * Performs generic lookup operation on the tree for the specified 3D point.
    * The template parameter defines the callback class which is called on each point within the specified range. The LookupProc type must define the following method:
    * void operator()(const TPoint3D &i_node_point, double i_distance_sqr, double &io_max_distance_sqr)
    * where the first parameter is a point (from the tree) being processed, the second parameter is a squared distance to that point from the input point and
    * the third parameter is the squared radius to search points within.
    * The callback may decrease the third parameter (which is an input-output reference) to decrease the search radius. It should not increase it however.
    * @param i_point Input point to perform lookup for.
    * @param i_proc The callback object.
    * @param i_max_distance Maximum distance from i_point to search points within.
    */
    template<typename LookupProc>
    void Lookup(const Point3D_d &i_point, LookupProc &i_proc, double i_max_distance = DBL_INF) const;

    /**
    * Returns point nearest to the specified one.
    * If there's no point within the specified search radius (i_max_distance parameter) the method returns NULL.
    * @param i_point Input point to perform lookup for.
    * @param i_max_distance Maximum distance from i_point to search points within.
    * @return Pointer to the nearest point or NULL if there are no points within the specified radius.
    */
    const TPoint3D *GetNearestPoint(const Point3D_d &i_point, double i_max_distance = DBL_INF) const;

    /**
    * Returns point nearest to the specified one with the custom filter.
    * The custom filter must define "bool operator()(const TPoint3D &i_point) const" method. Only points for which this method returns true will be considered.
    * If there's no point within the specified search radius (i_max_distance parameter) the method returns NULL.
    * @param i_point Input point to perform lookup for.
    * @param i_filter Custom points filter.
    * @param i_max_distance Maximum distance from i_point to search points within.
    * @return Pointer to the nearest point or NULL if there are no points within the specified radius.
    */
    template<typename PointsFilter>
    const TPoint3D *GetNearestPoint(const Point3D_d &i_point, const PointsFilter &i_filter, double i_max_distance = DBL_INF) const;

    /**
    * Finds first N points nearest to the specified one.
    * @param i_point Input point to perform lookup for.
    * @param i_points_to_lookup Number of nearest points to lookup.
    * @param[out] op_nearest_points Pointer to the allocated array where the resulting points will be written to. Should not be NULL.
    * @param i_max_distance Maximum distance from i_point to search points within.
    * @return Number of points found.
    */
    size_t GetNearestPoints(const Point3D_d &i_point, size_t i_points_to_lookup, NearestPoint *op_nearest_points, double i_max_distance = DBL_INF) const;

    /**
    * Finds first N points nearest to the specified one with the custom filter.
    * The custom filter must define "bool operator()(const TPoint3D &i_point) const" method. Only points for which this method returns true will be considered.
    * @param i_point Input point to perform lookup for.
    * @param i_points_to_lookup Number of nearest points to lookup.
    * @param[out] op_nearest_points Pointer to the allocated array where the resulting points will be written to. Should not be NULL.
    * @param i_filter Custom points filter.
    * @param i_max_distance Maximum distance from i_point to search points within.
    * @return Number of points found.
    */
    template<typename PointsFilter>
    size_t GetNearestPoints(const Point3D_d &i_point, size_t i_points_to_lookup, NearestPoint *op_nearest_points, const PointsFilter &i_filter, double i_max_distance = DBL_INF) const;

  private:
    struct Node;
    struct PointsComparator;

    template<typename PointsFilter>
    class NearestPointProc;

    template<typename PointsFilter>
    class NearestPointsProc;

  private:
    /**
    * Recursively builds the tree.
    * The method initializes node with the specified i_node_index and recursively splits the input range of points [i_begin;i_end) into two subregions.
    */
    void _Build(size_t i_begin, size_t i_end);

    /**
    * Private helper method that performs the generic lookup operation.
    */
    template<typename LookupProc>
    void _Lookup(size_t i_node_index, const Point3D_d &i_point, LookupProc &i_proc, double &io_max_distance_sqr) const;

  private:
    /**
    * Represents 3D points of the tree. Each element of the vector corresponds to m_nodes vector's element with the same index.
    */
    std::vector<TPoint3D> m_points;

    /**
    * Represents nodes of the tree. Each element of the vector corresponds to m_points vector's element with the same index.
    */
    std::vector<Node> m_nodes;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////// NearestPoint /////////////////////////////////////////////////
template<typename TPoint3D>
struct KDTree<TPoint3D>::NearestPoint
  {
  const TPoint3D *mp_point;
  double m_distance_sqr;

  NearestPoint()
    {
    }

  NearestPoint(const TPoint3D *ip_point, double i_distance_sqr): mp_point(ip_point), m_distance_sqr(i_distance_sqr)
    {
    }

  bool operator<(const NearestPoint &i_other) const
    {
    return m_distance_sqr == i_other.m_distance_sqr ? (mp_point < i_other.mp_point) : (m_distance_sqr < i_other.m_distance_sqr);
    }
  };

//////////////////////////////////////////////// Node /////////////////////////////////////////////////////

/**
* Represents a node of kd-tree.
* Each internal node can have up to two children. Since all nodes are stored in a continuous array the left child (if present) is always located immediately after the parent node.
* The right node index is stored in the parent node.
*/
template<typename TPoint3D>
struct KDTree<TPoint3D>::Node
  {
  // Coordinate of the split the node represents.
  float m_split_coordinate;

  union
    {
    // Only the highest 29 bits are used to represent the right child index.
    unsigned int m_right_child;

    // Only lowest three bits are used.
    // 0th and 1st bits are used for split axis index (0 for X, 1 for Y, 2 for Z and 3 if the node is a leaf)
    // 2nd bit is enabled if the node has left child and disabled otherwise.
    unsigned char m_flags;
    };

  void SetRightChild(unsigned int i_right_child)
    {
    ASSERT(i_right_child < (1<<29));
    m_right_child = (i_right_child<<3) + (m_right_child&7);
    }

  size_t GetRightChild() const
    {
    return m_right_child >> 3;
    }

  void SetHasLeftChild(bool i_has_left_node)
    {
    m_flags = (unsigned char)( (m_flags & (~4)) + (i_has_left_node?4:0) );
    }

  bool HasLeftChild() const
    {
    return (m_flags & 4) != 0;
    }

  void SetSplitAxis(unsigned char i_split_axis)
    {
    ASSERT(i_split_axis < 3);
    m_flags = (unsigned char)( (m_flags & (~3)) + i_split_axis );
    }

  unsigned char GetSplitAxis() const
    {
    return m_flags & 3;
    }

  void SetLeaf()
    {
    m_flags = (unsigned char)( (m_flags & (~3)) + 3 );
    }

  bool IsLeaf() const
    {
    return (m_flags & 3) == 3;
    }

  };

////////////////////////////////////////// PointsComparator ///////////////////////////////////////////////

/**
* The helper class is used to sort 3D points by a specified axis (X, Y or Z).
*/
template<typename TPoint3D>
struct KDTree<TPoint3D>::PointsComparator
  {
  unsigned char m_split_axis;

  PointsComparator(unsigned char i_split_axis): m_split_axis(i_split_axis)
    {
    ASSERT(i_split_axis<3);
    }

  bool operator()(const TPoint3D &point1, const TPoint3D &point2) const
    {
    return point1[m_split_axis] < point2[m_split_axis];
    }
  };

//////////////////////////////////////// DefaultPointsFilter /////////////////////////////////////////////

/**
* Void filter implementation that returns always returns true.
*/
template<typename TPoint3D>
class DefaultPointsFilter
  {
  public:
    DefaultPointsFilter()
      {
      }

    bool operator()(const TPoint3D &i_point) const
      {
      return true;
      }
  };

///////////////////////////////////////// NearestPointProc //////////////////////////////////////////////

/**
* Used to find 3D point nearest to the specified one.
*/
template<typename TPoint3D>
template<typename PointsFilter>
class KDTree<TPoint3D>::NearestPointProc
  {
  public:
    NearestPointProc(const PointsFilter &i_filter): m_filter(i_filter)
      {
      mp_nearest_point = NULL;
      }

    // Since the tree only calls this method for points that are definitely within the search radius we don't need to check if the point is closer than the current one, 
    // it definitely is. We just need to update the search radius.
    void operator()(const TPoint3D &i_node_point, double i_distance_sqr, double &io_max_distance_sqr)
      {
      if (m_filter(i_node_point)==false)
        return;

      ASSERT(i_distance_sqr <= io_max_distance_sqr);
      mp_nearest_point = &i_node_point;
      io_max_distance_sqr = i_distance_sqr;
      }

    const TPoint3D *GetNearestPoint() const
      {
      return mp_nearest_point;
      }

  private:
    const TPoint3D *mp_nearest_point;
    PointsFilter m_filter;
  };

///////////////////////////////////////// NearestPointsProc //////////////////////////////////////////////

/**
* Used to find N closest points.
* The class uses binary heap to effectively update N nearest points when a closer point is found.
*/
template<typename TPoint3D>
template<typename PointsFilter>
class KDTree<TPoint3D>::NearestPointsProc
  {
  public:
    NearestPointsProc(const Point3D_d &i_point, size_t i_points_to_lookup, NearestPoint *ip_nearest_points, const PointsFilter &i_filter):
    m_point(i_point), m_points_to_lookup(i_points_to_lookup), mp_nearest_points(ip_nearest_points), m_filter(i_filter)
      {
      ASSERT(i_points_to_lookup>0);
      ASSERT(ip_nearest_points);
      m_current_size = 0;
      }

    /*
    Since the tree only calls this method for points that are definitely within the search radius we don't need to check if the point is closer than at least one of
    the points found so far, it definitely is.
    */
    void operator()(const TPoint3D &i_node_point, double i_distance_sqr, double &io_max_distance_sqr)
      {
      if (m_filter(i_node_point)==false)
        return;

      // If we have not found requested number of points yet we just add them to unordered vector.
      if (m_current_size < m_points_to_lookup)
        {
        // Add point to unordered array of points.
        mp_nearest_points[m_current_size++] = NearestPoint(&i_node_point,i_distance_sqr);

        // If we found enough points we make a heap from them.
        if (m_current_size == m_points_to_lookup)
          {
          std::make_heap(mp_nearest_points, mp_nearest_points+m_current_size);
          io_max_distance_sqr = mp_nearest_points[0].m_distance_sqr;
          }
        }
      else
        {
        // Remove most distant point from heap and add new point.
        std::pop_heap(mp_nearest_points, mp_nearest_points+m_current_size);
        mp_nearest_points[m_points_to_lookup-1] = NearestPoint(&i_node_point,i_distance_sqr);
        std::push_heap(mp_nearest_points, mp_nearest_points+m_current_size);
        io_max_distance_sqr = mp_nearest_points[0].m_distance_sqr;
        }
      }

    size_t GetCurrentSize() const
      {
      return m_current_size;
      }

  private:
    Point3D_d m_point;

    NearestPoint *mp_nearest_points;
    PointsFilter m_filter;
    size_t m_points_to_lookup, m_current_size;
  };

////////////////////////////////////////////// KDTree ////////////////////////////////////////////////////

template<typename TPoint3D>
KDTree<TPoint3D>::KDTree(const std::vector<TPoint3D> &i_points): m_points(i_points)
  {
  size_t points_num = m_points.size();
  m_nodes.assign(points_num, Node());

  if (points_num == 0)
    return;

  _Build(0, points_num);
  }

template<typename TPoint3D>
KDTree<TPoint3D>::KDTree(std::vector<TPoint3D> &&i_points) : m_points(std::move(i_points))
  {
  size_t points_num = m_points.size();
  m_nodes.assign(points_num, Node());

  if (points_num == 0)
    return;

  _Build(0, points_num);
  }

template<typename TPoint3D>
size_t KDTree<TPoint3D>::GetNumberOfPoints() const
  {
  return m_points.size();
  }

template<typename TPoint3D>
const std::vector<TPoint3D> &KDTree<TPoint3D>::GetAllPoints() const
  {
  return m_points;
  }

template<typename TPoint3D>
void KDTree<TPoint3D>::_Build(size_t i_begin, size_t i_end)
  {
  ASSERT(i_begin<i_end);

  // If the points range has only point we make a leaf.
  if (i_begin + 1 == i_end)
    {
    m_nodes[i_begin].SetLeaf();
    return;
    }

  BBox3D_d bbox;
  for (size_t i=i_begin;i<i_end;++i)
    bbox.Unite(Point3D_d(m_points[i][0], m_points[i][1], m_points[i][2]));

  // We split by the longest axis.
  int split_axis = 0;
  if (bbox.m_max[1]-bbox.m_min[1] > bbox.m_max[split_axis]-bbox.m_min[split_axis]) split_axis=1;
  if (bbox.m_max[2]-bbox.m_min[2] > bbox.m_max[split_axis]-bbox.m_min[split_axis]) split_axis=2;

  // Split the input range in two halves by its median element.
  size_t split_index = (i_begin+i_end)/2;
  std::nth_element(m_points.begin() + i_begin, m_points.begin() + split_index, m_points.begin() + i_end, PointsComparator(split_axis));

  // Put the median element at the beginning of the range so that it corresponds to this node.
  std::swap(m_points[i_begin], m_points[split_index]);

  m_nodes[i_begin].SetSplitAxis(split_axis);
  m_nodes[i_begin].m_split_coordinate = (float)(m_points[i_begin])[split_axis];

  if (i_begin < split_index)
    {
    m_nodes[i_begin].SetHasLeftChild(true);
    _Build(i_begin+1, split_index+1);
    }
  else
    m_nodes[i_begin].SetHasLeftChild(false);

  if (split_index+1 < i_end)
    {
    m_nodes[i_begin].SetRightChild((unsigned int)split_index+1);
    _Build(split_index+1, i_end);
    }
  else
    m_nodes[i_begin].SetRightChild((1<<29)-1); // Initialize with the maximum value which means that the right child is not present.
  }

template<typename TPoint3D>
template<typename LookupProc>
void KDTree<TPoint3D>::Lookup(const Point3D_d &i_point, LookupProc &i_proc, double i_max_distance = DBL_INF) const
  {
  ASSERT(i_max_distance>=0);
  double max_dist_sqr = i_max_distance*i_max_distance;
  _Lookup(0, i_point, i_proc, max_dist_sqr);
  }

template<typename TPoint3D>
const TPoint3D *KDTree<TPoint3D>::GetNearestPoint(const Point3D_d &i_point, double i_max_distance = DBL_INF) const
  {
  ASSERT(i_max_distance>=0);

  DefaultPointsFilter<TPoint3D> default_filer;
  NearestPointProc<DefaultPointsFilter<TPoint3D>> proc(default_filer);
  this->Lookup(i_point, proc, i_max_distance);
  return proc.GetNearestPoint();
  }

template<typename TPoint3D>
template<typename PointsFilter>
const TPoint3D *KDTree<TPoint3D>::GetNearestPoint(const Point3D_d &i_point, const PointsFilter &i_filter, double i_max_distance = DBL_INF) const
  {
  ASSERT(i_max_distance>=0);
  NearestPointProc<PointsFilter> proc(i_filter);
  this->Lookup(i_point, proc, i_max_distance);
  return proc.GetNearestPoint();
  }

template<typename TPoint3D>
size_t KDTree<TPoint3D>::GetNearestPoints(const Point3D_d &i_point, size_t i_points_to_lookup,
                                        NearestPoint *op_nearest_points, double i_max_distance = DBL_INF) const
  {
  ASSERT(i_max_distance>=0);
  ASSERT(op_nearest_points);
  if (i_points_to_lookup == 0)
    return 0;

  NearestPointsProc<DefaultPointsFilter<TPoint3D>> proc(i_point, i_points_to_lookup, op_nearest_points, DefaultPointsFilter<TPoint3D>());
  this->Lookup(i_point, proc, i_max_distance);

  return proc.GetCurrentSize();
  }

template<typename TPoint3D>
template<typename PointsFilter>
size_t KDTree<TPoint3D>::GetNearestPoints(const Point3D_d &i_point, size_t i_points_to_lookup, NearestPoint *op_nearest_points, const PointsFilter &i_filter, double i_max_distance = DBL_INF) const
  {
  ASSERT(i_max_distance>=0);
  ASSERT(op_nearest_points);
  if (i_points_to_lookup == 0)
    return 0;

  NearestPointsProc<PointsFilter> proc(i_point, i_points_to_lookup, op_nearest_points, i_filter);
  this->Lookup(i_point, proc, i_max_distance);

  return proc.GetCurrentSize();
  }

template<typename TPoint3D>
template<typename LookupProc>
void KDTree<TPoint3D>::_Lookup(size_t i_node_index, const Point3D_d &i_point, LookupProc &i_proc, double &io_max_distance_sqr) const
  {
  ASSERT(io_max_distance_sqr >= 0.0);
  if (m_nodes.size()==0)
    return;

  const Node *p_node = &m_nodes[i_node_index];
  const TPoint3D &node_point = m_points[i_node_index];

  // Process this node's point if it is within the range.
  double dx=node_point[0]-i_point[0], dy=node_point[1]-i_point[1], dz=node_point[2]-i_point[2];
  double dist_sqr = dx*dx+dy*dy+dz*dz;
  if (dist_sqr < io_max_distance_sqr)
    i_proc(node_point, dist_sqr, io_max_distance_sqr);

  if (p_node->IsLeaf() == false)
    {
    unsigned char axis = p_node->GetSplitAxis();
    ASSERT(axis<3);

    double dist_sqr = (i_point[axis] - p_node->m_split_coordinate) * (i_point[axis] - p_node->m_split_coordinate);
    if (i_point[axis] <= p_node->m_split_coordinate)
      {
      // The left child (if present) is always located immediately after the parent node.
      if (p_node->HasLeftChild())
        _Lookup(i_node_index+1, i_point, i_proc, io_max_distance_sqr);

      if (dist_sqr < io_max_distance_sqr && p_node->GetRightChild() < m_nodes.size())
        _Lookup(p_node->GetRightChild(), i_point, i_proc, io_max_distance_sqr);
      }
    else
      {
      if (p_node->GetRightChild() < m_nodes.size())
        _Lookup(p_node->GetRightChild(), i_point, i_proc, io_max_distance_sqr);

      if (dist_sqr < io_max_distance_sqr && p_node->HasLeftChild())
        _Lookup(i_node_index+1, i_point, i_proc, io_max_distance_sqr);
      }
    }
  }

#endif //KD_TREE_H