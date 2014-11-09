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

#include "ImageEnvironmentalLight.h"
#include <Raytracer/Core/SpectrumRoutines.h>
#include <Math/CompressedDirection.h>
#include <Math/MathRoutines.h>
#include <tbb/tbb.h>

ImageEnvironmentalLight::ImageEnvironmentalLight(const BBox3D_d &i_world_bounds, const Transform &i_light_to_world,
                                                 const std::vector<std::vector<Spectrum_f>> &i_image, SpectrumCoef_d i_scale):
m_world_bounds(i_world_bounds), m_light_to_world(i_light_to_world), m_world_to_light(i_light_to_world.Inverted()), m_image(i_image), m_scale(i_scale)
  {
  ASSERT(i_image.size()>0 && i_image[0].size()>0);

  for(size_t i=1;i<i_image.size();++i)
    {
    ASSERT(i_image[i].size() == i_image[0].size());
    for(size_t j=1;j<i_image[i].size();++j)
      ASSERT(InRange(i_image[i][j], 0.f, FLT_INF));
    }

  m_height = m_image.size();
  m_width = m_image[0].size();

  // Increase the image size if needed.
  if (m_width<MIN_IMAGE_SIZE || m_height<MIN_IMAGE_SIZE)
    {
    _IncreaseSize((MIN_IMAGE_SIZE+m_width-1)/m_width, (MIN_IMAGE_SIZE+m_height-1)/m_height);
    m_width = m_image[0].size();
    m_height = m_image.size();
    }

  mp_image_map.reset(new MIPMap<Spectrum_f>(m_image, true, 9.0));
  _Initialize();
  }

ImageEnvironmentalLight::ImageEnvironmentalLight(const BBox3D_d &i_world_bounds, const Transform &i_light_to_world,
                                                 intrusive_ptr<const ImageSource<Spectrum_f>> ip_image_source, SpectrumCoef_d i_scale):
m_world_bounds(i_world_bounds), m_light_to_world(i_light_to_world), m_world_to_light(i_light_to_world.Inverted()), m_scale(i_scale)
  {
  ASSERT(ip_image_source);
  ASSERT(ip_image_source->GetHeight()>0 && ip_image_source->GetWidth()>0);

  m_image = ip_image_source->GetImage();
  m_height = m_image.size();
  m_width = m_image[0].size();

  // Increase the image size if needed.
  if (m_width<MIN_IMAGE_SIZE || m_height<MIN_IMAGE_SIZE)
    {
    _IncreaseSize((MIN_IMAGE_SIZE+m_height-1)/m_height, (MIN_IMAGE_SIZE+m_width-1)/m_width);
    m_width = m_image[0].size();
    m_height = m_image.size();
    }

  mp_image_map.reset(new MIPMap<Spectrum_f>(m_image, true, 9.0));
  _Initialize();
  }

void ImageEnvironmentalLight::_IncreaseSize(size_t i_height_factor, size_t i_width_factor)
  {
  ASSERT(i_width_factor>=1 && i_height_factor>=1);
  std::vector<std::vector<Spectrum_f>> tmp(m_image.size()*i_height_factor, std::vector<Spectrum_f>(m_image[0].size()*i_width_factor));
  for (size_t y=0; y<tmp.size(); ++y)
    for (size_t x=0; x<tmp[0].size(); ++x)
      tmp[y][x]=m_image[y/i_height_factor][x/i_width_factor];

  m_image.swap(tmp);
  }

void ImageEnvironmentalLight::_Initialize()
  {
  m_theta_coef = M_PI / m_height;
  m_phi_coef = 2.0*M_PI / m_width;

  m_CDF_cols.assign(m_height, std::vector<double>(m_width, 0.0));

  m_nodes_num=1;
  _Build(0, 0, Point2D_i(0,0), Point2D_i((int)m_width, (int)m_height), m_nodes_num);
  ASSERT(m_nodes_num < 2*(1<<MAX_TREE_DEPTH));

  // Release the memory.
  std::vector<std::vector<Spectrum_f>>().swap(m_image);

  // Precompute irradiance values and PDFs.
  _PrecomputeData();
  }

// Recursively builds tree by initializing the specified node and calling itself for the children.
void ImageEnvironmentalLight::_Build(size_t i_node_index, size_t i_depth, const Point2D_i &i_begin, const Point2D_i &i_end, size_t &io_next_free_node_index)
  {
  ASSERT(m_nodes_directions.size() == 4*i_node_index);
  m_nodes_directions.push_back(MathRoutines::SphericalDirection<double>(i_begin[0]*m_phi_coef, i_begin[1]*m_theta_coef));
  m_nodes_directions.push_back(MathRoutines::SphericalDirection<double>(i_end[0]*m_phi_coef, i_begin[1]*m_theta_coef));
  m_nodes_directions.push_back(MathRoutines::SphericalDirection<double>(i_end[0]*m_phi_coef, i_end[1]*m_theta_coef));
  m_nodes_directions.push_back(MathRoutines::SphericalDirection<double>(i_begin[0]*m_phi_coef, i_end[1]*m_theta_coef));

  m_nodes[i_node_index].m_image_begin = i_begin;
  m_nodes[i_node_index].m_image_end = i_end;

  // Create leaf if the rectangle is already a single pixel or if the maximum depth is reached.
  if ((i_begin[0]+1==i_end[0] && i_begin[1]+1==i_end[1]) || i_depth == MAX_TREE_DEPTH)
    {
    m_nodes[i_node_index].m_leaf = true;

    m_nodes[i_node_index].m_CDF_begin = m_CDF_rows.size();

    // Fill the CDF values for the leaf. The outer loop is by rows and the inner loop is by columns.
    // The inner loop computes CDFs for columns and the row's total radiance that is then used to compute the CDF for rows.
    double filter_width = 1.0 / std::max(m_width, m_height);
    double d_phi = 2.0*M_PI / m_width;
    double inv_height = 1.0 / m_height, inv_width = 1.0 / m_width;
    m_nodes[i_node_index].m_total_radiance = Spectrum_d();
    for(int y=i_begin[1];y<i_end[1];++y)
      {
      double y_normalized = (y + 0.5) * inv_height;
      double d_cos_theta = cos(y*m_theta_coef) - cos((y+1)*m_theta_coef);
      double pixel_solid_angle = d_phi*d_cos_theta;

      std::vector<double> &CDF_cols = m_CDF_cols[y];

      for(int x=i_begin[0];x<i_end[0];++x)
        {
        double x_normalized = (x + 0.5) * inv_width;
        Spectrum_d pixel_radiance = pixel_solid_angle*Convert<double>(m_image[y][x]);

        m_nodes[i_node_index].m_total_radiance += pixel_radiance;

        CDF_cols[x] = SpectrumRoutines::Luminance(pixel_radiance);
        if (x>i_begin[0]) CDF_cols[x] += CDF_cols[x-1];
        }

      double col_PDF_sum = CDF_cols[i_end[0]-1];

      // Normalize the CDF for columns.
      double inv_col_PDF_sum = col_PDF_sum > 0 ? 1.0/col_PDF_sum : 0.0;
      for(int x=i_begin[0];x<i_end[0];++x)
        CDF_cols[x] *= inv_col_PDF_sum;

      if (y>i_begin[1])
        m_CDF_rows.push_back(col_PDF_sum + m_CDF_rows.back());
      else
        m_CDF_rows.push_back(col_PDF_sum);
      }

    // Normalize the CDF for rows.
    double inv_row_PDF_sum = m_CDF_rows.back() > 0 ? 1.0/m_CDF_rows.back() : 0.0;
    for(size_t y=m_CDF_rows.size()-(i_end[1]-i_begin[1]);y<m_CDF_rows.size();++y)
      m_CDF_rows[y] *= inv_row_PDF_sum;
    
    return;
    }

  // Initialize internal node.
  m_nodes[i_node_index].m_leaf = false;

  unsigned char split_axis = (i_end[0]-i_begin[0])*m_phi_coef > (i_end[1]-i_begin[1])*m_theta_coef ? 0 : 1;

  // Change the split axis if there's nothing to split in this dimension.
  if (i_end[split_axis]-i_begin[split_axis]<2) split_axis = 1-split_axis;
  ASSERT(i_end[split_axis]-i_begin[split_axis]>1);

  m_nodes[i_node_index].m_split_axis = split_axis;
  m_nodes[i_node_index].m_split_coordinate = (i_begin[split_axis]+i_end[split_axis])/2;

  Point2D_i middle_point = split_axis==0 ? Point2D_i(m_nodes[i_node_index].m_split_coordinate, i_end[1]) : Point2D_i(i_end[0], m_nodes[i_node_index].m_split_coordinate);
  _Build(io_next_free_node_index++, i_depth+1, i_begin, middle_point, io_next_free_node_index);

  size_t right_child_index = io_next_free_node_index;
  m_nodes[i_node_index].m_right_child = right_child_index;
  middle_point = split_axis==0 ? Point2D_i(m_nodes[i_node_index].m_split_coordinate, i_begin[1]) : Point2D_i(i_begin[0], m_nodes[i_node_index].m_split_coordinate);
  _Build(io_next_free_node_index++, i_depth+1, middle_point, i_end, io_next_free_node_index);

  m_nodes[i_node_index].m_total_radiance = m_nodes[i_node_index+1].m_total_radiance + m_nodes[right_child_index].m_total_radiance;
  }

// Precomputes irradiance values for nodes and PDFs for light sampling.
void ImageEnvironmentalLight::_PrecomputeData()
  {
  m_nodes_hemispherical_PDF.assign((1<<16) * m_nodes_num, 0.f);
  m_irradiances.resize(1<<16);

  // We use CompressedDirection to discretize the sphere directions.
  // There are total of 2^16 discretized directions and when a normal is later passed to the sampling method the nearest discretized direction will be picked.
  tbb::parallel_for((size_t)0, (size_t)(1<<16), [&](size_t i)
    {
    Vector3D_d normal = CompressedDirection::FromID((unsigned short)i).ToVector3D<double>();

    Point2D_d normal_angles(MathRoutines::SphericalPhi(normal), MathRoutines::SphericalTheta(normal));
    m_irradiances[i] = _PrecomputeIrradiance(0, normal, normal_angles, &m_nodes_hemispherical_PDF[m_nodes_num*i]);
    m_nodes_hemispherical_PDF[m_nodes_num*i] = 1.f;
    });

  // Compute PDF for sampling without normal. The PDF is simply proportional to the node total luminance.
  m_nodes_spherical_PDF.assign(m_nodes_num, 0.f);
  for(size_t i=0;i<m_nodes_num;++i)
    if (m_nodes[i].m_leaf == false)
      {
      size_t left_index = i+1;
      size_t right_index = m_nodes[i].m_right_child;

      // Normalize PDF values.
      double children_pdf_sum = SpectrumRoutines::Luminance(m_nodes[left_index].m_total_radiance+m_nodes[right_index].m_total_radiance);
      double inv_children_pdf_sum = children_pdf_sum>0 ? 1.0/children_pdf_sum : 0.0;

      m_nodes_spherical_PDF[left_index] = (float) (SpectrumRoutines::Luminance(m_nodes[left_index].m_total_radiance) * inv_children_pdf_sum);
      m_nodes_spherical_PDF[right_index] = (float) (SpectrumRoutines::Luminance(m_nodes[right_index].m_total_radiance) * inv_children_pdf_sum);
      }
  m_nodes_spherical_PDF[0] = 1.f;
  }

// Precomputes irradiance and PDF values for the specified surface normal.
Spectrum_d ImageEnvironmentalLight::_PrecomputeIrradiance(size_t i_node_index, const Vector3D_d &i_normal, const Point2D_d i_normal_angles, float *op_nodes_PDF) const
  {
  // Compute dot products of the normal with the four corners
  double dots[4] = {
    m_nodes_directions[4*i_node_index]*i_normal,
    m_nodes_directions[4*i_node_index+1]*i_normal,
    m_nodes_directions[4*i_node_index+2]*i_normal,
    m_nodes_directions[4*i_node_index+3]*i_normal};

  const Node &node = m_nodes[i_node_index];
  // Check if the node is visible for the specified normal.
  if (dots[0] < 0.0 && dots[1] < 0.0 && dots[2] < 0.0 && dots[3] < 0.0)
    {
    // Even if all dot products are negative the rectangle may still be partially visible if any of the edges with constant theta angle intersects the plane defined by the normal.
    bool edge_visible = node.m_image_begin[0]*m_phi_coef<i_normal_angles[0] && node.m_image_end[0]*m_phi_coef>i_normal_angles[0] &&
      (fabs(node.m_image_begin[1]*m_theta_coef-i_normal_angles[1])<M_PI_2 || fabs(node.m_image_end[1]*m_theta_coef-i_normal_angles[1])<M_PI_2);

    if (edge_visible==false)
      return Spectrum_d();
    }

  if (m_nodes[i_node_index].m_leaf)
    {
    // Compute dot products of the normal with the centers of the four quadrants, they are just weighted sums of the dot products with corners.
    double dots2[4] = {
      0.0625*(dots[0]*9+dots[1]*3+dots[2]*1+dots[3]*3),
      0.0625*(dots[1]*9+dots[2]*3+dots[3]*1+dots[0]*3),
      0.0625*(dots[2]*9+dots[3]*3+dots[0]*1+dots[1]*3),
      0.0625*(dots[3]*9+dots[0]*3+dots[1]*1+dots[2]*3) };

    // We approximate the integral of radiance*cosine over the rectangle by multiplying the total radiance by an averaged cosine.
    // Compute average cosine by averaging dot products of the normal with centers of the four quadrants.
    double average_cos = (std::max(dots2[0],0.0)+std::max(dots2[1],0.0)+std::max(dots2[2],0.0)+std::max(dots2[3],0.0)) * 0.25;
    
    /*
    Due to the ad-hoc way we used to estimate cosine value it may differ from the correct one. Too low cosine values can lead to a big variance in certain cases.
    To avoid that we clamp it by 0.1 and thus allow only one order of magnitude of difference between max and min cosine values.
    This can cause over-estimation near the horizon but it is much better than underestimating.
    */
    average_cos = std::max(average_cos, 0.1);

    Spectrum_d irradiance = average_cos * m_nodes[i_node_index].m_total_radiance;
    op_nodes_PDF[i_node_index] = (float)SpectrumRoutines::Luminance(irradiance);

    return irradiance;
    }
  else
    {
    Spectrum_d irradiance;
    irradiance += _PrecomputeIrradiance(i_node_index+1, i_normal, i_normal_angles, op_nodes_PDF);
    irradiance += _PrecomputeIrradiance(m_nodes[i_node_index].m_right_child, i_normal, i_normal_angles, op_nodes_PDF);

    op_nodes_PDF[i_node_index] = (float)SpectrumRoutines::Luminance(irradiance);

    // Normalize PDF values.
    double children_pdf_sum = op_nodes_PDF[i_node_index+1]+op_nodes_PDF[m_nodes[i_node_index].m_right_child];
    double inv_children_pdf_sum = children_pdf_sum>0 ? 1.0/children_pdf_sum : 0.0;
    op_nodes_PDF[i_node_index+1] = (float) (op_nodes_PDF[i_node_index+1]*inv_children_pdf_sum);
    op_nodes_PDF[m_nodes[i_node_index].m_right_child] = (float) (op_nodes_PDF[m_nodes[i_node_index].m_right_child]*inv_children_pdf_sum);

    return irradiance;
    }
  }

Spectrum_d ImageEnvironmentalLight::Radiance(const RayDifferential &i_ray) const
  {
  // Since the light is an environment one only unbounded rays have non-black radiance.
  if (IsPositiveInf(i_ray.m_base_ray.m_max_t)==false)
    return Spectrum_d();

  Vector3D_d base_direction = m_world_to_light(i_ray.m_base_ray.m_direction);
  double phi = MathRoutines::SphericalPhi(base_direction) * INV_2PI;
  double theta = MathRoutines::SphericalTheta(base_direction) * INV_PI;
  Point2D_d w(phi, theta);

  if (i_ray.m_has_differentials==false)
    return m_scale * Convert<double>(mp_image_map->Evaluate(w, 0.0));

  Vector3D_d direction_dx = m_world_to_light(i_ray.m_direction_dx);
  Vector3D_d direction_dy = m_world_to_light(i_ray.m_direction_dy);

  double phi_dx = MathRoutines::SphericalPhi(direction_dx) * INV_2PI;
  double theta_dx = MathRoutines::SphericalTheta(direction_dx) * INV_PI;

  double phi_dy = MathRoutines::SphericalPhi(direction_dy) * INV_2PI;
  double theta_dy = MathRoutines::SphericalTheta(direction_dy) * INV_PI;

  Vector2D_d dw_dx(phi_dx-phi, theta_dx-theta);
  Vector2D_d dw_dy(phi_dy-phi, theta_dy-theta);

  if (dw_dx[0]>0.5) dw_dx[0] = dw_dx[0]-1.0;
  if (dw_dx[0]<-0.5) dw_dx[0] = dw_dx[0]+1.0;

  if (dw_dy[0]>0.5) dw_dy[0] = dw_dy[0]-1.0;
  if (dw_dy[0]<-0.5) dw_dy[0] = dw_dy[0]+1.0;

  if (w[1]<0.01 || w[1]>0.99)
    dw_dx=dw_dy=Vector2D_d(0.0,0.0);

  return m_scale * Convert<double>(mp_image_map->Evaluate(w, dw_dx, dw_dy));
  }

Spectrum_d ImageEnvironmentalLight::Power() const
  {
  double dx = m_world_bounds.m_max[0]-m_world_bounds.m_min[0];
  double dy = m_world_bounds.m_max[1]-m_world_bounds.m_min[1];
  double dz = m_world_bounds.m_max[2]-m_world_bounds.m_min[2];

  Spectrum_d irradiance_xy = m_irradiances[CompressedDirection(m_world_to_light(Vector3D_d(0,0,1))).GetID()] + m_irradiances[CompressedDirection(m_world_to_light(Vector3D_d(0,0,-1))).GetID()];
  Spectrum_d irradiance_xz = m_irradiances[CompressedDirection(m_world_to_light(Vector3D_d(0,1,0))).GetID()] + m_irradiances[CompressedDirection(m_world_to_light(Vector3D_d(0,-1,0))).GetID()];
  Spectrum_d irradiance_yz = m_irradiances[CompressedDirection(m_world_to_light(Vector3D_d(1,0,0))).GetID()] + m_irradiances[CompressedDirection(m_world_to_light(Vector3D_d(-1,0,0))).GetID()];

  return m_scale * (irradiance_xy*fabs(dx*dy) + irradiance_xz*fabs(dx*dz) + irradiance_yz*fabs(dy*dz));
  }

Spectrum_d ImageEnvironmentalLight::SampleLighting(const Point2D_d &i_sample, Vector3D_d &o_lighting_direction, double &o_pdf) const
  {
  Spectrum_d radiance = _LightingSample(i_sample, &m_nodes_spherical_PDF[0], o_lighting_direction, o_pdf);
  m_light_to_world(o_lighting_direction, o_lighting_direction);
  return m_scale * radiance;
  }

double ImageEnvironmentalLight::LightingPDF(const Vector3D_d &i_lighting_direction) const
  {
  return _LightingPDF(m_world_to_light(i_lighting_direction), &m_nodes_spherical_PDF[0]);
  }

Spectrum_d ImageEnvironmentalLight::SampleLighting(const Vector3D_d &i_normal, const Point2D_d &i_sample, Vector3D_d &o_lighting_direction, double &o_pdf) const
  {
  ASSERT(i_normal.IsNormalized());
  size_t direction_index = CompressedDirection(m_world_to_light(i_normal)).GetID();

  Spectrum_d radiance = _LightingSample(i_sample, &m_nodes_hemispherical_PDF[m_nodes_num*direction_index], o_lighting_direction, o_pdf);
  m_light_to_world(o_lighting_direction, o_lighting_direction);
  return m_scale * radiance;
  }

double ImageEnvironmentalLight::LightingPDF(const Vector3D_d &i_normal, const Vector3D_d &i_lighting_direction) const
  {
  ASSERT(i_normal.IsNormalized());
  size_t direction_index = CompressedDirection(m_world_to_light(i_normal)).GetID();

  return _LightingPDF(m_world_to_light(i_lighting_direction), &m_nodes_hemispherical_PDF[m_nodes_num*direction_index]);
  }

Spectrum_d ImageEnvironmentalLight::_LightingSample(Point2D_d i_sample, const float *ip_nodes_pdf, Vector3D_d &o_lighting_direction, double &o_pdf) const
  {
  ASSERT(m_nodes_num>0);
  size_t index=0;
  double leaf_pdf = 1.0;

  // Find the leaf to be sampled by going down from the root of the tree to one of the leaves.
  // We alternate the sample value to be used for determining the child to go to, i_sample[0] is used for X splits and i_sample[1] is used for Y splits.
  // this alternation preserves good stratification of the samples.
  while(m_nodes[index].m_leaf == false)
    {
    const Node &node = m_nodes[index];

    size_t left_index = index+1;
    size_t right_index = node.m_right_child;
    unsigned char split_axis = node.m_split_axis;

    ASSERT(i_sample[split_axis] >= 0.0 && i_sample[split_axis] < 1.0);
    if (i_sample[split_axis]<ip_nodes_pdf[left_index])
      {
      leaf_pdf *= ip_nodes_pdf[left_index];

      i_sample[split_axis] = std::min(1.0-DBL_EPS, i_sample[split_axis]/ip_nodes_pdf[left_index]);
      index = left_index;
      }
    else
      {
      leaf_pdf *= ip_nodes_pdf[right_index];

      i_sample[split_axis] = std::min(1.0-DBL_EPS, (i_sample[split_axis]-ip_nodes_pdf[left_index])/ip_nodes_pdf[right_index]);
      index = right_index;
      }
    };

  const Node &leaf = m_nodes[index];
  size_t width = leaf.m_image_end[0]-leaf.m_image_begin[0], height = leaf.m_image_end[1]-leaf.m_image_begin[1];

  // Binary search for the sampled row.
  const double *p_row_CDF = &m_CDF_rows[leaf.m_CDF_begin];
  ASSERT(fabs( *(p_row_CDF+height-1) - 1.0) < DBL_EPS);

  double row_pdf;
  size_t row = MathRoutines::BinarySearchCDF(p_row_CDF, p_row_CDF+height, i_sample[1], &row_pdf) - p_row_CDF;
  i_sample[1] = (i_sample[1] - (*(p_row_CDF+row)-row_pdf)) / row_pdf;
  ASSERT(i_sample[1]>=0.0 && i_sample[1]<1.0);

  // Binary search for the sampled column.
  const double *p_col_CDF = &m_CDF_cols [row+leaf.m_image_begin[1]] [leaf.m_image_begin[0]];
  ASSERT (fabs( *(p_col_CDF+width-1) - 1.0) < DBL_EPS);

  double col_pdf;
  size_t col = MathRoutines::BinarySearchCDF(p_col_CDF, p_col_CDF+width, i_sample[0], &col_pdf) - p_col_CDF;
  i_sample[0] = (i_sample[0] - (*(p_col_CDF+col)-col_pdf)) / col_pdf;
  ASSERT(i_sample[0]>=0.0 && i_sample[0]<1.0);

  col += leaf.m_image_begin[0];
  row += leaf.m_image_begin[1];
  ASSERT(col>=0 && col<m_width && row>=0 && row<m_height);

  double d_phi = 2.0*M_PI / m_width;
  double cos_theta1 = cos(row*m_theta_coef), cos_theta2 = cos((row+1)*m_theta_coef);
  double d_cos_theta = cos_theta1 - cos_theta2;
  double pixel_solid_angle = d_phi*d_cos_theta;

  o_pdf = leaf_pdf*row_pdf*col_pdf / pixel_solid_angle;

  double phi = (col+i_sample[0]) * m_phi_coef;
  double theta = acos(cos_theta1-i_sample[1]*d_cos_theta);

  o_lighting_direction = MathRoutines::SphericalDirection<double>(phi, theta);

  return Convert<double>(mp_image_map->Evaluate(Point2D_d(phi*INV_2PI, theta*INV_PI), 0.0));
  }

double ImageEnvironmentalLight::_LightingPDF(const Vector3D_d &i_lighting_direction, const float *ip_nodes_pdf) const
  {
  ASSERT(m_nodes_num>0);
  ASSERT(i_lighting_direction.IsNormalized());
  double phi = MathRoutines::SphericalPhi(i_lighting_direction);
  double theta = MathRoutines::SphericalTheta(i_lighting_direction);
  Point2D_i texel((int) (phi*m_width*INV_2PI), (int) (theta*m_height*INV_PI));

  if (texel[0] == m_width) texel[0] = m_width-1;
  if (texel[1] == m_height) texel[1] = m_height-1;

  size_t index=0;
  double leaf_pdf = 1.0;

  // Find the leaf to be sampled by going down from the root of the tree to one of the leaves.
  while(m_nodes[index].m_leaf == false)
    {
    const Node &node = m_nodes[index];

    size_t left_index = index+1;
    size_t right_index = node.m_right_child;
    unsigned char split_axis = node.m_split_axis;

    if ((size_t)texel[split_axis] < node.m_split_coordinate)
      {
      leaf_pdf *= ip_nodes_pdf[left_index];
      index = left_index;
      }
    else
      {
      leaf_pdf *= ip_nodes_pdf[right_index];
      index = right_index;
      }
    };

  const Node &leaf = m_nodes[index];

  size_t row = texel[1]-leaf.m_image_begin[1], col = texel[0]-leaf.m_image_begin[0];
  ASSERT(row>=0 && (int)row<leaf.m_image_end[1]-leaf.m_image_begin[1] && col>=0 && (int)col<leaf.m_image_end[0]-leaf.m_image_begin[0]);

  const double *p_row_CDF = &m_CDF_rows[leaf.m_CDF_begin];
  double row_pdf = row==0 ? *p_row_CDF : *(p_row_CDF+row) - *(p_row_CDF+row-1);

  const double *p_col_CDF = &m_CDF_cols [texel[1]] [leaf.m_image_begin[0]];
  double col_pdf = col==0 ? *p_col_CDF : *(p_col_CDF+col) - *(p_col_CDF+col-1);

  double d_phi = 2.0*M_PI / m_width;
  double cos_theta1 = cos(texel[1]*m_theta_coef), cos_theta2 = cos((texel[1]+1)*m_theta_coef);
  double d_cos_theta = cos_theta1 - cos_theta2;
  double pixel_solid_angle = d_phi*d_cos_theta;

  return leaf_pdf*row_pdf*col_pdf / pixel_solid_angle;
  }

Spectrum_d ImageEnvironmentalLight::SamplePhoton(const Point2D_d &i_position_sample, const Point2D_d &i_direction_sample, Ray &o_photon_ray, double &o_pdf) const
  {
  double direction_pdf;
  Vector3D_d direction;
  Spectrum_d irradiance = _LightingSample(i_direction_sample, &m_nodes_spherical_PDF[0], direction, direction_pdf);
  m_light_to_world(direction, direction);
  direction *= -1.0;

  // Construct a disk perpendicular to the sampled direction outside of the world bounds. The ray origin will be sampled on this disk.
  double x = fabs(m_world_bounds.m_max[0]-m_world_bounds.m_min[0]);
  double y = fabs(m_world_bounds.m_max[1]-m_world_bounds.m_min[1]);
  double z = fabs(m_world_bounds.m_max[2]-m_world_bounds.m_min[2]);
  double radius = 0.5*sqrt(x*x+y*y+z*z);
  Point3D_d center = (m_world_bounds.m_max+m_world_bounds.m_min)/2.0;

  Vector3D_d e2,e3;
  MathRoutines::CoordinateSystem(direction, e2, e3);

  Point2D_d local = SamplingRoutines::ConcentricDiskSampling(i_position_sample);
  o_photon_ray.m_origin = center + radius * (local[0] * e2 + local[1] * e3) - radius*direction;
  o_photon_ray.m_direction = direction;
  o_photon_ray.m_min_t = 0.0;
  o_photon_ray.m_max_t = DBL_INF;

  o_pdf = direction_pdf/(M_PI * radius * radius);

  return m_scale * irradiance;
  }

Spectrum_d ImageEnvironmentalLight::Irradiance(const Vector3D_d &i_normal) const
  {
  ASSERT(m_nodes_num>0);
  return m_scale * m_irradiances[CompressedDirection(m_world_to_light(i_normal)).GetID()];
  }

Spectrum_d ImageEnvironmentalLight::Fluence() const
  {
  ASSERT(m_nodes_num>0);
  return m_scale * m_nodes[0].m_total_radiance;
  }