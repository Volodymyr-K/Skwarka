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

#include "PhotonBeamAccelerator.h"
#include <Math/MathRoutines.h>
#include <tbb/tbb.h>
#include <numeric>
#include <algorithm>

PhotonLTEIntegrator::PhotonBeamAccelerator::PhotonBeamAccelerator(intrusive_ptr<const VolumeRegion> ip_volume_region, std::vector<PhotonBeam> &&i_beams) :
mp_volume_region(ip_volume_region), m_beams(std::move(i_beams)), mp_root(NULL), m_pool(100000*sizeof(PhotonBeamAccelerator::Node))
  {
  for (size_t i=0; i<m_beams.size();++i)
    {
    Vector3D_f e2, e3;
    MathRoutines::CoordinateSystem(m_beams[i].m_direction, e2, e3);
    float r1 = m_beams[i].m_radius_begin, r2 = m_beams[i].m_radius_end;

    // Construct bounding box of each beam
    BBox3D_f beam_bbox;
    Point3D_f center = m_beams[i].m_origin;

    beam_bbox.Unite(center + e2*r1 + e3*r1);
    beam_bbox.Unite(center + e2*r1 - e3*r1);
    beam_bbox.Unite(center - e2*r1 + e3*r1);
    beam_bbox.Unite(center - e2*r1 - e3*r1);

    double t = m_beams[i].m_distance;
    center = m_beams[i].m_origin + m_beams[i].m_direction * t;

    beam_bbox.Unite(center + e2*r2 + e3*r2);
    beam_bbox.Unite(center + e2*r2 - e3*r2);
    beam_bbox.Unite(center - e2*r2 + e3*r2);
    beam_bbox.Unite(center - e2*r2 - e3*r2);

    m_beams_bboxes.push_back(beam_bbox);
    }

  // Construct the resulting tree with all the beams.
  void *ptr = m_pool.Alloc(sizeof(Node));
  mp_root = new (ptr)Node(*this, 0, m_beams.size(), 0, 0);
  }

Spectrum_d PhotonLTEIntegrator::PhotonBeamAccelerator::_NodeIntersect(const PhotonBeamAccelerator::Node *ip_node, const Ray &i_ray,
                                                                      double i_step, double i_offset, MemoryPool *ip_pool) const
  {
  //++glob_cnt1;
  double invs[3];
  invs[0]=1.0/i_ray.m_direction[0];
  invs[1]=1.0/i_ray.m_direction[1];
  invs[2]=1.0/i_ray.m_direction[2];

  typedef const Node *NodePtr;
  NodePtr todo[2*MAX_TREE_DEPTH+1];
  todo[0]=ip_node;
  int todo_size=1;
  
  MemoryPoolAllocator<std::pair<float, Spectrum_f>> alloc(*ip_pool);
  std::vector<std::pair<float, Spectrum_f>, MemoryPoolAllocator<std::pair<float, Spectrum_f>>> intersections(alloc);

  while (todo_size>0)
    {
    ASSERT(todo_size<=2*MAX_TREE_DEPTH+1);
    const Node *p_node = todo[--todo_size];

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
      // Process all beams in the leaf
      for (size_t i=p_node->m_beams_begin; i<p_node->m_beams_end; ++i)
        {
        //glob_cnt2++;
        
        // Check whether the ray intersects the bbox of the beam.
        BBox3D_f bbox = m_beams_bboxes[i];
        double tNear1 = (bbox.m_min[0] - i_ray.m_origin[0]) * invs[0];
        double tFar1  = (bbox.m_max[0] - i_ray.m_origin[0]) * invs[0];
        if (tNear1 > tFar1) std::swap(tNear1, tFar1);

        double tNear2 = (bbox.m_min[1] - i_ray.m_origin[1]) * invs[1];
        double tFar2  = (bbox.m_max[1] - i_ray.m_origin[1]) * invs[1];
        if (tNear2 > tFar2) std::swap(tNear2, tFar2);

        double tNear3 = (bbox.m_min[2] - i_ray.m_origin[2]) * invs[2];
        double tFar3  = (bbox.m_max[2] - i_ray.m_origin[2]) * invs[2];
        if (tNear3 > tFar3) std::swap(tNear3, tFar3);

        if (
          i_ray.m_min_t > tFar1 || tNear1 > tFar2 || tNear2 > tFar1 || tNear3 > tFar1 ||
          i_ray.m_min_t > tFar2 || tNear1 > tFar3 || tNear2 > tFar3 || tNear3 > tFar2 ||
          i_ray.m_min_t > tFar3 || tNear1 > i_ray.m_max_t || tNear2 > i_ray.m_max_t || tNear3 > i_ray.m_max_t)
          continue;
        
        PhotonBeam beam = m_beams[i];
        Point3D_d beam_point = Convert<double>(beam.m_origin);
        Vector3D_d beam_direction = Convert<double>(beam.m_direction);

        Vector3D_d cross = i_ray.m_direction ^ beam_direction;
        double sin_theta_sqr = cross.LengthSqr();
        if (sin_theta_sqr < DBL_EPS) continue;
        double inv_sin_theta_sqr = 1.0 / sin_theta_sqr;

        Vector3D_d diff(i_ray.m_origin - beam_point);

        double t1 = -((diff^beam_direction) * cross) * inv_sin_theta_sqr;
        if (t1<i_ray.m_min_t || t1>i_ray.m_max_t)
          continue;

        double t2 = -((diff^i_ray.m_direction) * cross) * inv_sin_theta_sqr;
        if (t2<0.0 || t2>beam.m_distance)
          continue;
        
        double tmp = diff * cross, dist_coef = t2/beam.m_distance;
        double dist_sqr = (tmp*tmp) * inv_sin_theta_sqr;
        double width = MathRoutines::LinearInterpolate(dist_coef, beam.m_radius_begin, beam.m_radius_end);
        double width_sqr = width*width;
        if (dist_sqr>width_sqr)
          continue;

        Ray beam_ray(beam_point, beam_direction, 0.0, t2);
        Spectrum_f power(MathRoutines::LinearInterpolate(dist_coef, beam.m_power_begin, beam.m_power_end));

        double phase = mp_volume_region->Phase(i_ray(t1), beam_direction, -1.0*i_ray.m_direction);
        SpectrumCoef_d scattering = mp_volume_region->Scattering(i_ray(t1));

        double x = dist_sqr/width_sqr;
        double kernel = 15.0/16.0 * (1-x)*(1-x);
        Spectrum_f radiance = power * Convert<float>( scattering * (kernel * phase / (width * sqrt(sin_theta_sqr))) );
        intersections.push_back(std::make_pair((float)t1, radiance));
        //++glob_cnt3;
        }

      }
    }

  // Sort the found intersections by the distance
  std::sort(intersections.begin(), intersections.end(), [](const std::pair<float, Spectrum_f> &a, const std::pair<float, Spectrum_f> &b) {return a.first < b.first; });

  Spectrum_d radiance;
  SpectrumCoef_d ray_thickness;
  double previous_t=i_ray.m_min_t;
  for (size_t i=0; i<intersections.size(); ++i)
    {
    Ray camera_ray(i_ray.m_origin, i_ray.m_direction, previous_t, intersections[i].first);
    ray_thickness += mp_volume_region->OpticalThickness(camera_ray, i_step, i_offset);
    radiance += Convert<double>(intersections[i].second) * Exp(-1.0*ray_thickness);
    previous_t = intersections[i].first;
    }
  
  //if (glob_cnt2>0) printf("%lf %lf\n", 1.0*glob_cnt2/glob_cnt1, 1.0*glob_cnt3/glob_cnt1);
  return radiance;
  }

Spectrum_d PhotonLTEIntegrator::PhotonBeamAccelerator::Intersect(const Ray &i_ray, double i_step, double i_offset, MemoryPool *ip_pool) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());

  return _NodeIntersect(mp_root, i_ray, i_step, i_offset, ip_pool);
  }

BBox3D_f PhotonLTEIntegrator::PhotonBeamAccelerator::_ConstructBBox(size_t i_beams_begin, size_t i_beams_end) const
  {
  BBox3D_f bbox;
  for (size_t i=i_beams_begin; i!=i_beams_end; ++i)
    bbox.Unite(m_beams_bboxes[i]);
  return bbox;
  }

void PhotonLTEIntegrator::PhotonBeamAccelerator::_SwapBeams(size_t i_index1, size_t i_index2)
  {
  ASSERT(i_index1<m_beams.size());
  ASSERT(i_index2<m_beams.size());

  std::swap(m_beams[i_index1], m_beams[i_index2]);
  std::swap(m_beams_bboxes[i_index1], m_beams_bboxes[i_index2]);
  }

std::pair<unsigned char, double> PhotonLTEIntegrator::PhotonBeamAccelerator::_DetermineBestSplit(const BBox3D_f &i_node_bbox, size_t i_beams_begin, size_t i_beams_end,
                                                                                                 unsigned char i_middle_split_mask) const
  {
  ASSERT(i_middle_split_mask<(1<<3));
  size_t num_beams = i_beams_end-i_beams_begin;
  ASSERT(num_beams>0);

  double best_split_position[3], best_cost[3] ={ DBL_INF, DBL_INF, DBL_INF };

  // Run it in parallel threads and let TBB parallelize it. For small numbers of beams we run it in single thread by increasing the grainsize.
  size_t tbb_grain = num_beams>70 ? 1 : 6;
  tbb::parallel_for(tbb::blocked_range<size_t>(0, 3, tbb_grain), [&](const tbb::blocked_range<size_t> &i_range)
    {
    for (size_t split_axis = i_range.begin(); split_axis != i_range.end(); ++split_axis) if ((i_middle_split_mask&(1<<split_axis)) == 0)
      {
      /*
      * First precompute the needed information for all split positions.
      * The information for each split is the bounding boxes of the child and number of beams in each child.
      */
      BBox3D_d left_bboxes[MAX_SPLIT_TRIES], middle_bboxes[MAX_SPLIT_TRIES], right_bboxes[MAX_SPLIT_TRIES];
      size_t left_num[MAX_SPLIT_TRIES], middle_num[MAX_SPLIT_TRIES], right_num[MAX_SPLIT_TRIES];

      memset(left_num, 0, MAX_SPLIT_TRIES*sizeof(size_t));
      memset(middle_num, 0, MAX_SPLIT_TRIES*sizeof(size_t));
      memset(right_num, 0, MAX_SPLIT_TRIES*sizeof(size_t));

      size_t num_tries = std::min(MAX_SPLIT_TRIES, 2*(i_beams_end-i_beams_begin));
      double coef = num_tries/(i_node_bbox.m_max[split_axis]-i_node_bbox.m_min[split_axis]);
      for (size_t i = 0; i<num_beams; ++i)
        {
        const BBox3D_f &bbox = m_beams_bboxes[i_beams_begin+i];

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

        for (size_t j = rights_end; j<lefts_begin; ++j)
          {
          middle_bboxes[j].Unite(bbox);
          ++middle_num[j];
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

PhotonLTEIntegrator::PhotonBeamAccelerator::Node::Node(PhotonBeamAccelerator &i_accelerator, size_t i_beams_begin, size_t i_beams_end,
                                                       unsigned char i_middle_split_mask, size_t i_depth) :
m_beams_begin(i_beams_begin), m_beams_end(i_beams_end), m_bbox(i_accelerator._ConstructBBox(i_beams_begin, i_beams_end))
  {
  // Create leaf if there are less beams than MAX_BEAMS_IN_LEAF, if the maximum tree depth is reached or
  // if there's no effective split possible (this is the case if the node has three middle splits on its path back to the root).
  if (i_beams_end-i_beams_begin <= MAX_BEAMS_IN_LEAF || i_middle_split_mask == (1<<3)-1 || i_depth>=MAX_TREE_DEPTH)
    {
    SetType(true, 0);
    return;
    }

  std::pair<unsigned char, double> split = i_accelerator._DetermineBestSplit(m_bbox, i_beams_begin, i_beams_end, i_middle_split_mask);
  unsigned char split_axis = split.first;
  double split_coord = split.second;

  SetType(false, split_axis);

  size_t beams_middle_begin=i_beams_begin;
  size_t beams_right_begin=i_beams_end;
  size_t i=i_beams_begin;

  // Partition the beams in three regions:
  // 1. The beams lying below the splitting plane.
  // 2. The beams intersecting the splitting plane.
  // 3. The beams lying above the splitting plane.
  while (i<beams_right_begin)
    {
    const BBox3D_f &bbox = i_accelerator.m_beams_bboxes[i];

    if (bbox.m_max[split_axis] < split_coord)
      {
      // First sub-region.
      i_accelerator._SwapBeams(beams_middle_begin, i);
      ++beams_middle_begin;
      ++i;
      }
    else
      if (bbox.m_min[split_axis] > split_coord)
        {
        // Third sub-region.
        --beams_right_begin;
        i_accelerator._SwapBeams(i, beams_right_begin);
        }
      else
        // Second sub-region.
        ++i;
    }

  // Create left child node.
  if (i_beams_begin<beams_middle_begin)
    {
    void * ptr = i_accelerator.m_pool.Alloc(sizeof(Node));
    m_children[0] = new (ptr)Node(i_accelerator, i_beams_begin, beams_middle_begin, i_middle_split_mask, i_depth+1);
    }
  else
    m_children[0] = NULL;

  // Create middle child node.
  if (beams_middle_begin<beams_right_begin)
    {
    void * ptr = i_accelerator.m_pool.Alloc(sizeof(Node));
    m_children[1] = new (ptr)Node(i_accelerator, beams_middle_begin, beams_right_begin, i_middle_split_mask | (1<<split_axis), i_depth+1);
    }
  else
    m_children[1] = NULL;

  // Create right child node.
  if (beams_right_begin<i_beams_end)
    {
    void * ptr = i_accelerator.m_pool.Alloc(sizeof(Node));
    m_children[2] = new (ptr)Node(i_accelerator, beams_right_begin, i_beams_end, i_middle_split_mask, i_depth+1);
    }
  else
    m_children[2] = NULL;
  }