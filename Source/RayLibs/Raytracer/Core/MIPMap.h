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

#ifndef MIP_MAP_H
#define MIP_MAP_H

#include <Common/Common.h>
#include <Math/BlockedArray.h>
#include <Math/Point2D.h>
#include <Math/MathRoutines.h>
#include <Math/SamplingRoutines.h>
#include "ImageSource.h"
#include <tbb/tbb.h>
#include <vector>

/**
* The MIP-map implementation with trilinear filtering and isotropic filtering.
* The MIP-map (multum in parvo, meaning "much in a small space") is basically a collection of copies of an original image
* with each copy being two times smaller than the previous one (in each dimension).
* The MIP-map supports trilinear filtering which is an isotropic filter and anisotropic filtering (EWA filter).
* The implementation supports non-power-of-two image size.
*
* The template parameter corresponds to the values type.
*/
template <typename T>
class MIPMap: public ReferenceCounted
  {
  public:
    /**
    * Constructs MIPMap for the given original image (2D array).
    * @param i_values 2D array of the image values. All inner vectors should have the same size. Should have at least one row and at least one column.
    * @param i_repeat Sets whether to wrap the texture on its edges. If false, the value is considered zero (black) beyond the image.
    * @param i_max_anisotropy Maximum anisotropy allowed (ratio of the major ellipse axis to its minor axis). Should be greater or equal than 1.0.
    */
    MIPMap(const std::vector<std::vector<T>> &i_values, bool i_repeat, double i_max_anisotropy);

    /**
    * Constructs MIPMap from the specified image source.
    * @param ip_image_source ImageSource implementation that defines image for the MIPMap. The image defined by the ImageSource should not be empty.
    * @param i_repeat Sets whether to wrap the texture on its edges. If false, the value is considered zero (black) beyond the image.
    * @param i_max_anisotropy Maximum anisotropy allowed (ratio of the major ellipse axis to its minor axis). Should be greater or equal than 1.0.
    */
    MIPMap(intrusive_ptr<const ImageSource<T>> ip_image_source, bool i_repeat, double i_max_anisotropy);
    
    /**
    * Returns filtered value of image at the specified point using trilinear filter.
    * @param i_point Point at which the image is to be filtered. The range [0;1]x[0;1] corresponds to the points inside the image.
    * @param i_width Filter width in normalized coordinates space. Please note that the actual filter's extent is twice the width.
    */
    T Evaluate(const Point2D_d &i_point, double i_width) const;

    /**
    * Returns filtered value of image at the specified point using anisotropic filter.
    * @param i_point Point at which the image is to be filtered. The range [0;1]x[0;1] corresponds to the points inside the image.
    * @param i_dxy_1 First ellipse axis in normalized coordinates space.
    * @param i_dxy_2 Second ellipse axis in normalized coordinates space.
    */
    T Evaluate(const Point2D_d &i_point, Vector2D_d i_dxy_1, Vector2D_d i_dxy_2) const;

    ~MIPMap();

  public:
    // Not implemented, not a value type.
    MIPMap(const MIPMap&);
    MIPMap &operator=(const MIPMap&);

  private:
    // Internal types.
    struct ResampleWeight;

  private:
    /**
    * This method is called from constructors to initialize the class instance.
    * It resamples the input image, builds the MIPMap levels and initializes EWA weights.
    */
    void _Initialize(const std::vector<std::vector<T>> &i_values, double i_max_anisotropy);

    /**
    * Private method that returns image value at the specified point at the specified level.
    */
    const T &_GetTexel(size_t i_level, int i_x, int i_y) const;

    /**
    * Private method that interpolates image values using four nearest values from the map at the specified level.
    */
    T _Interpolate(size_t i_level, Point2D_d i_point) const;
    
    /**
    * Private method that filters image values using EWA filter at the specified point at the specified level.
    */
    T _EWA(size_t i_level, Point2D_d i_point, Vector2D_d i_dxy_1, Vector2D_d i_dxy_2) const;

    /**
    * Private method that computes EWA filter weights. This method is called once in the constructor.
    */
    void _InitializeEWA();

  private:
    size_t m_width, m_height, m_num_levels;
    double m_max_anisotropy;
    bool m_repeat;

    /**
    * Levels of the MIP-map. 0-th level corresponds to the original image and the highest level has 1x1 size.
    */
    std::vector<BlockedArray<T> *> m_levels;

    // Number of EWA filter weights.
    static const size_t EWA_WEIGHTS_NUM = 128;

    std::vector<double> m_EWA_weights;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
MIPMap<T>::MIPMap(const std::vector<std::vector<T>> &i_values, bool i_repeat, double i_max_anisotropy):
m_repeat(i_repeat)
  {
  _Initialize(i_values, i_max_anisotropy);
  }

template <typename T>
MIPMap<T>::MIPMap(intrusive_ptr<const ImageSource<T>> ip_image_source, bool i_repeat, double i_max_anisotropy):
m_repeat(i_repeat)
  {
  ASSERT(ip_image_source);
  ASSERT(ip_image_source->GetHeight()>0 && ip_image_source->GetWidth()>0);

  _Initialize(ip_image_source->GetImage(), i_max_anisotropy);
  }

template <typename T>
void MIPMap<T>::_Initialize(const std::vector<std::vector<T>> &i_values, double i_max_anisotropy)
  {
  ASSERT(i_max_anisotropy>=1.0);
  m_max_anisotropy = std::max(i_max_anisotropy, 1.0);

  size_t size_x = i_values[0].size(), size_y = i_values.size();
  m_height = size_y;
  m_width = size_x;

  BlockedArray<T> *p_image = new BlockedArray<T>(size_y, size_x);
  for (size_t y=0; y<size_y; ++y)
    for (size_t x=0; x<size_x; ++x)
      p_image->Get(y, x) = i_values[y][x];

  m_levels.push_back(p_image);
  for(size_t level=1;size_y>1 || size_x>1;++level)
    {
    size_t prev_size_x = size_x, prev_size_y = size_y;
    double inv_prev_size_x = 1.0/prev_size_x, inv_prev_size_y = 1.0/prev_size_y;

    // The size of the new level is the ceiling of the half-size of the previous layer
    size_y = (prev_size_y+1)/2;
    size_x = (prev_size_x+1)/2;

    // Each sample for a layer is computed as a weighted average of nine samples from the previous layer.
    const BlockedArray<T> *p_previous_level = m_levels[level-1];
    BlockedArray<T> *p_level = new BlockedArray<T>(size_y, size_x);

    // Set default weights for the case when the old size is even.
    double y_weights[3] = { 0.0, 0.5, 0.5 }, x_weights[3] = { 0.0, 0.5, 0.5 };
    for (size_t y=0; y<size_y; ++y)
      {
      // If the old size is odd, use polyphase filter weights (as described by Stefan Guthe and Paul Heckbert 2003).
      if (prev_size_y&1)
        {
        y_weights[0] = y * inv_prev_size_y;
        y_weights[1] = size_y * inv_prev_size_y;
        y_weights[2] = (size_y-y-1) * inv_prev_size_y;
        }

      for (size_t x=0; x<size_x; ++x)
        {
        if (prev_size_x&1)
          {
          x_weights[0] = x * inv_prev_size_x;
          x_weights[1] = size_x * inv_prev_size_x;
          x_weights[2] = (size_x-x-1) * inv_prev_size_x;
          }

        T sum = T();
        double weights_sum = 0.0; // used only for verification in debug mode
        for (int dy = -1; dy <= 1; ++dy) if (2*y+dy>=0 && 2*y+dy<prev_size_y)
          for (int dx = -1; dx <= 1; ++dx) if (2*x+dx>=0 && 2*x+dx<prev_size_x)
            {
            double weight = y_weights[1+dy]*x_weights[1+dx];
            sum += static_cast<T>(p_previous_level->Get(2*y+dy, 2*x+dx) * weight);
            weights_sum += weight;
            }

        ASSERT(fabs(weights_sum-1.0) < (1e-10));
        p_level->Get(y, x) = sum;
        }
      }

    m_levels.push_back(p_level);
    }

  m_num_levels = m_levels.size();
  _InitializeEWA();
  }

template <typename T>
MIPMap<T>::~MIPMap()
  {
  for(size_t i=0;i<m_levels.size();++i)
    delete m_levels[i];
  }

template <typename T>
void MIPMap<T>::_InitializeEWA()
  {
  m_EWA_weights.assign(EWA_WEIGHTS_NUM, 0.0);

  for (size_t i=0;i<EWA_WEIGHTS_NUM;++i)
    {
    const double alpha = 2.0;
    double r2 = double(i) / double(EWA_WEIGHTS_NUM - 1);
    m_EWA_weights[i] = exp(-alpha * r2) - exp(-alpha);
    }
  }

template <typename T>
const T &MIPMap<T>::_GetTexel(size_t i_level, int i_x, int i_y) const
  {
  ASSERT(i_level < m_num_levels);

  const BlockedArray<T> &level = *m_levels[i_level];

  if (m_repeat)
    {
    i_y = MathRoutines::Mod(i_y, (int)level.GetSizeU());
    i_x = MathRoutines::Mod(i_x, (int)level.GetSizeV());
    }
  else
    if (i_y<0 || i_y>=(int)level.GetSizeU() || i_x<0 || i_x>=(int)level.GetSizeV())
      {
      static const T black = T();
      return black;
      }

  ASSERT(i_x>=0 && i_y>=0);
  return level.Get((size_t)i_y, (size_t)i_x);
  }

template <typename T>
T MIPMap<T>::Evaluate(const Point2D_d &i_point, double i_width) const
  {
  // Compute MIPMap level for trilinear filtering.
  double level = 0.0;
  if (i_width > 1e-10)
    if (i_width > 1.0)
      level = m_num_levels - 1;
    else
      level = m_num_levels - 1 + MathRoutines::Log2(i_width);

  ASSERT(level>=0.0 && level<=m_num_levels-1);

  // Perform trilinear interpolation at appropriate MIPMap level. 
  if (level <= 0)
    return _Interpolate(0, i_point);
  else if (level >= m_num_levels-1)
    {
    if (m_repeat)
      return _GetTexel(m_num_levels-1, 0, 0);
    else
      // We just return the highest layer's value if the point is inside the [0;1]x[0;1] range.
      // This way we don't filter the values though.
      if (i_point[0]>=0.0 && i_point[0]<=1.0 && i_point[1]>=0.0 && i_point[1]<=1.0)
        return _GetTexel(m_num_levels-1, 0, 0);
      else
        return T();
    }
  else
    {
    double delta = level - (size_t)level;
    return (1.0-delta) * _Interpolate((size_t)level, i_point) + delta * _Interpolate((size_t)level+1, i_point);
    }
  }

template <typename T>
T MIPMap<T>::Evaluate(const Point2D_d &i_point, Vector2D_d i_dxy_1, Vector2D_d i_dxy_2) const
  {
  // Compute ellipse minor and major axes.
  if (i_dxy_1.LengthSqr() < i_dxy_2.LengthSqr())
    std::swap(i_dxy_1, i_dxy_2);

  double major_length = sqrt(i_dxy_1.LengthSqr());
  double minor_length = sqrt(i_dxy_2.LengthSqr());

  if (minor_length < DBL_EPS)
    return _Interpolate(0, i_point);

  // Clamp ellipse eccentricity if too large.
  if (minor_length * m_max_anisotropy < major_length)
    {
    double scale = major_length / (minor_length * m_max_anisotropy);
    i_dxy_2 *= scale;
    minor_length *= scale;
    }

  // Choose level of detail for EWA lookup and perform EWA filtering.
  double level = 0.0;
  if (minor_length > 1e-10)
    if (minor_length > 1.0)
      level = m_num_levels - 1;
    else
      level = std::max(0.0, m_num_levels - 1 + MathRoutines::Log2(minor_length));

  if (level >= m_num_levels-1)
    {
    if (m_repeat)
      return _GetTexel(m_num_levels-1, 0, 0);
    else
      // We just return the highest layer's value if the point is inside the [0;1]x[0;1] range.
      // This way we don't filter the values though.
      if (i_point[0]>=0.0 && i_point[0]<=1.0 && i_point[1]>=0.0 && i_point[1]<=1.0)
        return _GetTexel(m_num_levels-1, 0, 0);
      else
        return T();
    }
  else
    {
    // Choose level of detail for EWA lookup and perform EWA filtering.
    double delta = level - (size_t)level;
    return static_cast<T>(
      (1.0-delta) * _EWA((size_t)level, i_point, i_dxy_1, i_dxy_2) + delta * _EWA((size_t)level+1, i_point, i_dxy_1, i_dxy_2)
      );
    }
  }

template <typename T>
T MIPMap<T>::_Interpolate(size_t i_level, Point2D_d i_point) const
  {
  ASSERT(i_level>=0 && i_level<m_num_levels);
  BlockedArray<T> &level = *m_levels[i_level];

  i_point[0] = i_point[0] * level.GetSizeV() - 0.5;
  i_point[1] = i_point[1] * level.GetSizeU() - 0.5;
  int x = (int)floor(i_point[0]), y = (int)floor(i_point[1]);
  double dx = i_point[0] - x, dy = i_point[1] - y;
  ASSERT(dx>=0.0 && dy>=0.0);

  return static_cast<T>(
    (1.0-dx)*(1.0-dy) * _GetTexel(i_level, x  , y  ) +
    (1.0-dx)*dy       * _GetTexel(i_level, x  , y+1) +
    dx*(1.0-dy)       * _GetTexel(i_level, x+1, y  ) +
    dx*dy             * _GetTexel(i_level, x+1, y+1)
    );
  }

template <typename T>
T MIPMap<T>::_EWA(size_t i_level, Point2D_d i_point, Vector2D_d i_dxy_1, Vector2D_d i_dxy_2) const
  {
  ASSERT(i_level>=0 && i_level<m_num_levels);
  BlockedArray<T> &level = *m_levels[i_level];

  // Convert EWA coordinates to appropriate scale for level.
  i_point[0] = i_point[0] * level.GetSizeV() - 0.5;
  i_point[1] = i_point[1] * level.GetSizeU() - 0.5;

  i_dxy_1[0] *= level.GetSizeV();
  i_dxy_1[1] *= level.GetSizeU();

  i_dxy_2[0] *= level.GetSizeV();
  i_dxy_2[1] *= level.GetSizeU();

  // Compute ellipse coefficients to bound EWA filter region.
  double A = i_dxy_1[1]*i_dxy_1[1] + i_dxy_2[1]*i_dxy_2[1] + 1.0;
  double B = -2.0 * (i_dxy_1[0]*i_dxy_1[1] + i_dxy_2[0]*i_dxy_2[1]);
  double C = i_dxy_1[0]*i_dxy_1[0] + i_dxy_2[0]*i_dxy_2[0] + 1.0;
  double inv_F = 1.0 / (A*C - B*B*0.25);
  A *= inv_F;
  B *= inv_F;
  C *= inv_F;

  // Compute the ellipse's bounding box in texture space.
  double det = 4.0*A*C - B*B;
  double inv_det = 1.0 / det; // This can be optimized to avoid this div using the fact that inv_det == 1.0/(4.0*inv_F)

  double x_sqrt = sqrt(det * C), y_sqrt = sqrt(A * det);
  int x1 = (int) ceil(i_point[0] - 2.0 * inv_det * x_sqrt);
  int x2 = (int)floor(i_point[0] + 2.0 * inv_det * x_sqrt);
  int y1 = (int) ceil(i_point[1] - 2.0 * inv_det * y_sqrt);
  int y2 = (int)floor(i_point[1] + 2.0 * inv_det * y_sqrt);

  // Scan over ellipse bound and compute quadratic equation.
  T num = T();
  double den = 0.0;
  for (int y=y1;y<=y2;++y)
    {
    double yy = y - i_point[1];

    for (int x=x1;x<=x2;++x)
      {
      double xx = x - i_point[0];

      // Compute squared radius and filter texel if inside the ellipse.
      double r_sqr = A*xx*xx + B*xx*yy + C*yy*yy;
      if (r_sqr < 1.0)
        {
        ASSERT(r_sqr>=0.0);
        double weight = m_EWA_weights[std::min((size_t)floor(r_sqr * EWA_WEIGHTS_NUM), EWA_WEIGHTS_NUM-1)];
        num += static_cast<T>( _GetTexel(i_level, x, y) * weight );
        den += weight;
        }
      }
    }

  ASSERT(den > 0.0);
  return static_cast<T>( num / den );
  }

#endif // MIP_MAP_H