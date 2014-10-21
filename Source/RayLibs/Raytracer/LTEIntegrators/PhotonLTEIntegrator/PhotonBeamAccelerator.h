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

#ifndef PHOTON_BEAM_ACCELERATOR_H
#define PHOTON_BEAM_ACCELERATOR_H

#include "../PhotonLTEIntegrator.h"
#include "PhotonInternalTypes.h"
#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/VolumeRegion.h>
#include <vector>

class PhotonLTEIntegrator::PhotonBeamAccelerator
  {
  public:
    PhotonBeamAccelerator(intrusive_ptr<const VolumeRegion> ip_volume_region, std::vector<PhotonBeam> &&i_beams);

    Spectrum_d Intersect(const Ray &i_ray, double i_step, double i_offset, MemoryPool *ip_pool) const;

  private:
    struct Node;

  private:
    // Not implemented, not a value type.
    PhotonBeamAccelerator();
    PhotonBeamAccelerator(PhotonBeamAccelerator &);
    PhotonBeamAccelerator &operator=(PhotonBeamAccelerator &);

    BBox3D_f _ConstructBBox(size_t i_beams_begin, size_t i_beams_end) const;

    void _SwapBeams(size_t i_index1, size_t i_index2);

    /**
    * Computes best split axis and split position for the specified internal node.
    * The best split axis and split position are the ones that have the minimum cost function. The cost function is the cost of node traversal assuming its children are all leaves.
    * The method tries many splits positions that are distributed uniformly over the node's extent.
    */
    std::pair<unsigned char, double> _DetermineBestSplit(const BBox3D_f &i_node_bbox, size_t i_beams_begin, size_t i_beams_end, unsigned char i_middle_split_mask) const;

    Spectrum_d _NodeIntersect(const PhotonBeamAccelerator::Node *ip_node, const Ray &i_ray,
                              double i_step, double i_offset1, MemoryPool *ip_pool) const;

  private:
    intrusive_ptr<const VolumeRegion> mp_volume_region;

    std::vector<PhotonBeam> m_beams;

    // Root node of the tree.
    Node *mp_root;

    // Memory pool that is used for allocating the nodes.
    MemoryPool m_pool;

    // Bounding boxes of the beams.
    std::vector<BBox3D_f> m_beams_bboxes;

    // Maximum number of beams in leaves. The actual number of beams may be greater for middle children if an effective split is not possible.
    static const size_t MAX_BEAMS_IN_LEAF = 4;

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
* Contains bounding box of the beams associated with the node, pointers to children,
* begin and end iterators of the associated beams and the flags bitset.
*/
struct PhotonLTEIntegrator::PhotonBeamAccelerator::Node
  {
  // Bounding box of the beams associated with the node.
  BBox3D_f m_bbox;

  // Pointers to children (NULL if not present).
  Node *m_children[3];

  // Begin and end iterators of the associated beams in the tree (see PhotonBeamAccelerator::m_beams vector).
  size_t m_beams_begin, m_beams_end;

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
  * @param i_accelerator PhotonBeamAccelerator instance the node belongs to.
  * @param i_beams_begin Begin iterator of the corresponding beams.
  * @param i_beams_end End iterator of the corresponding beams.
  * @param i_middle_split_mask The bitset that defines what middle splits have been done in the ancestor nodes.
  * @param i_depth Depth of the node (0 for root).
  */
  Node(PhotonBeamAccelerator &i_accelerator, size_t i_beams_begin, size_t i_beams_end, unsigned char i_middle_split_mask, size_t i_depth);
  };

inline void PhotonLTEIntegrator::PhotonBeamAccelerator::Node::SetType(bool i_is_leaf, unsigned char i_split_axis)
  {
  if (i_is_leaf)
    m_flags = m_flags & (~3);
  else
    {
    ASSERT(i_split_axis >=0 && i_split_axis <= 2);
    m_flags = (unsigned char)((m_flags & (~3)) + i_split_axis + 1);
    }
  }

inline bool PhotonLTEIntegrator::PhotonBeamAccelerator::Node::IsLeaf() const
  {
  return (m_flags & 3) == 0;
  }

inline unsigned char PhotonLTEIntegrator::PhotonBeamAccelerator::Node::GetSplitAxis() const
  {
  ASSERT(IsLeaf()==false);
  return (m_flags&3)-1;
  }

#endif // PHOTON_BEAM_ACCELERATOR_H