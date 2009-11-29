#ifndef MIP_MAP_H
#define MIP_MAP_H

#include <Common/Common.h>
#include <Raytracer/Core/BlockedArray.h>
#include <Math/Point2D.h>
#include <Math/Util.h>
#include <Math/SamplingRoutines.h>
#include <vector>

/**
* The MIP-map implementation with trilinear filtering and isotropic filtering.
* The MIP-map (multum in parvo, meaning "much in a small space") is basically a collection of copies of an original image
* with each copy being two times smaller than the previous one (in each dimension).
* The MIP-map supports trilinear filtering which is an isotropic filter and anisotropic filtering (EWA filter).
*
* The template parameter corresponds to the values type.
*/
template <typename T>
class MIPMap: public ReferenceCounted
  {
  public:
    /**
    * Constructs MIPMap for the given original image (2D array).
    * @param i_values 2D array of image values. All inner vectors should have the same size. Should have at least one row and at least one column.
    * @param i_repeat Sets whether to wrap the texture on its edges. If false, the value is considered zero (black) beyond the image.
    * @param i_max_anisotropy Specified maximum anisotropy allowed (ratio of major ellipse axis to its minor axis). Should be greater or equal than 1.0.
    */
    MIPMap(const std::vector<std::vector<T> > &i_values, bool i_repeat = true, double i_max_anisotropy = 8.0);
    
    /**
    * Returns filtered value of image at the specified point using trilinear filter.
    * @param i_point Point at which the image is to be filtered. The range [0;1]x[0;1] corresponds to the points inside the image.
    * @param i_width Filter width in normalized coordinates space.
    */
    T Evaluate(const Point2D_d &i_point, double i_width);

    /**
    * Returns filtered value of image at the specified point using anisotropic filter.
    * @param i_point Point at which the image is to be filtered. The range [0;1]x[0;1] corresponds to the points inside the image.
    * @param i_dxy_1 First ellipse axis in normalized coordinates space.
    * @param i_dxy_2 Second ellipse axis in normalized coordinates space.
    */
    T Evaluate(const Point2D_d &i_point, Vector2D_d i_dxy_1, Vector2D_d i_dxy_2);

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
    * Private method that resamples original input image so that its width and height are both powers of 2.
    */
    void _ResampleImage(const std::vector<std::vector<T> > &i_values, std::vector<std::vector<T> > &o_resampled_values) const;

    /**
    * Private method that computes EWA filter weights. This method is called once in the constructor.
    */
    void _InitializeEWA();

    /**
    * Private method that computes resampling weights used for image resampling.
    */
    std::vector<ResampleWeight> _ComputeResampleWeights(size_t i_old_size, size_t i_new_size) const;

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

/**
* A private inner structure that contains resampling weights.
* Each new sample is computed as a weighted sum of four consecutive original samples.
*/
template <typename T>
struct MIPMap<T>::ResampleWeight
  {
  int m_first_texel;
  double m_weights[4];
  };

template <typename T>
MIPMap<T>::MIPMap(const std::vector<std::vector<T> > &i_values, bool i_repeat, double i_max_anisotropy): m_repeat(i_repeat)
  {
  ASSERT(i_max_anisotropy>=1.0);
  m_max_anisotropy = std::max(i_max_anisotropy, 1.0);

  // Resample original image so that it's width and height are both powers of 2.
  std::vector<std::vector<T> > resampled_values;
  _ResampleImage(i_values, resampled_values);

  m_height = resampled_values.size();
  m_width = resampled_values[0].size();
  m_num_levels = MathRoutines::FloorLog2( (unsigned int)std::max(m_width,m_height) )+1;
  ASSERT(m_num_levels>0);
  
  m_levels.push_back(new BlockedArray<T>(resampled_values));
  for(size_t level=1;level<m_num_levels;++level)
    {
    size_t size_y = std::max(m_levels.back()->GetSizeU()/2, (size_t)1);
    size_t size_x = std::max(m_levels.back()->GetSizeV()/2, (size_t)1);

    // Each sample for a layer is computed as an average of four samples from the previous layer.
    const BlockedArray<T> *p_previous_level = m_levels[level-1];
    BlockedArray<T> *p_level = new BlockedArray<T>(size_y, size_x);
    for (size_t y=0;y<size_y;++y)
      for (size_t x=0;x<size_x;++x)
        {
        T sum = p_previous_level->Get(2*y, 2*x);
        size_t texels = 1;

        // We need to be careful when either width or height of a previous layer is equal to one.
        if (2*x+1<p_previous_level->GetSizeV())
          {
          sum += p_previous_level->Get(2*y, 2*x+1);
          ++texels;
          }

        if (2*y+1<p_previous_level->GetSizeU())
          {
          sum += p_previous_level->Get(2*y+1, 2*x);
          ++texels;
          }

        if (2*x+1<p_previous_level->GetSizeV() && 2*y+1<p_previous_level->GetSizeU())
          {
          sum += p_previous_level->Get(2*y+1, 2*x+1);
          ++texels;
          }

        p_level->Get(y,x) = sum / texels;
        }

    m_levels.push_back(p_level);
    }

  _InitializeEWA();
  }

template <typename T>
MIPMap<T>::~MIPMap()
  {
  for(size_t i=0;i<m_levels.size();++i)
    delete m_levels[i];
  }

template <typename T>
void MIPMap<T>::_ResampleImage(const std::vector<std::vector<T> > &i_values, std::vector<std::vector<T> > &o_resampled_values) const
  {
  ASSERT(i_values.size()>0 && i_values[0].size()>0);

  size_t size_x = i_values[0].size(), size_y = i_values.size();
  for(size_t i=1;i<size_y;++i) ASSERT(i_values[i].size() == i_values[0].size());

  // Compute new width and height.
  size_t rounded_size_x = MathRoutines::RoundUpPow2((unsigned int)size_x), rounded_size_y = MathRoutines::RoundUpPow2((unsigned int)size_y);

  if (size_x==rounded_size_x && size_y==rounded_size_y)
    {
    o_resampled_values=i_values;
    return;
    }

  o_resampled_values.assign(rounded_size_y, std::vector<T>(rounded_size_x));

  // First, we resample the image in X dimension (width). We can resample the dimensions separately because the used filter (Lanczos) is separable.
  std::vector<ResampleWeight> weights = _ComputeResampleWeights(size_x, rounded_size_x);

  for (size_t y=0;y<size_y;++y)
    {
    for (size_t x=0;x<rounded_size_x;++x)
      {
      double weigths_sum=0.0;
      o_resampled_values[y][x] = T();
      for (unsigned char j = 0; j < 4; ++j)
        {
        int original_x = weights[x].m_first_texel + j;
        if (m_repeat)
          original_x = MathRoutines::Mod<int>(original_x, (int)size_x);

        if (original_x >= 0 && original_x < (int)size_x)
          {
          weigths_sum += weights[x].m_weights[j];
          o_resampled_values[y][x] += weights[x].m_weights[j] * i_values[y][original_x];
          }
        }

      // Normalize filter weights for texel resampling.
      ASSERT(weigths_sum > 0.0);
      o_resampled_values[y][x] /= weigths_sum;
      }
    }

  // Now, we resample the image in Y dimension (height).
  weights = _ComputeResampleWeights(size_y, rounded_size_y);

  std::vector<T> tmp(rounded_size_y);
  for (size_t x=0;x<rounded_size_x;++x)
    {
    for (size_t y=0;y<rounded_size_y;++y)
      {
      double weigths_sum=0.0;
      tmp[y] = T();
      for (unsigned char j = 0; j < 4; ++j)
        {
        int original_y = weights[y].m_first_texel + j;
        if (m_repeat)
          original_y = MathRoutines::Mod<int>(original_y, (int)size_y);

        if (original_y >= 0 && original_y < (int)size_y)
          {
          weigths_sum += weights[y].m_weights[j];
          tmp[y] += weights[y].m_weights[j] * o_resampled_values[original_y][x];
          }
        }

      // Normalize filter weights for texel resampling.
      ASSERT(weigths_sum > 0.0);
      tmp[y] /= weigths_sum;
      }

    for (size_t y=0;y<rounded_size_y;++y)
      o_resampled_values[y][x] = tmp[y];
    }
  }

template <typename T>
void MIPMap<T>::_InitializeEWA()
  {
  for (size_t i=0;i<EWA_WEIGHTS_NUM;++i)
    {
    const double alpha = 2.0;
    double r2 = double(i) / double(EWA_WEIGHTS_NUM - 1);
    m_EWA_weights.push_back( exp(-alpha * r2) - exp(-alpha) );
    }
  }

template <typename T>
std::vector<typename MIPMap<T>::ResampleWeight> MIPMap<T>::_ComputeResampleWeights(size_t i_old_size, size_t i_new_size) const
  {
  ASSERT(i_new_size >= i_old_size);

  double filter_width = 2.0;
  std::vector<ResampleWeight> weights(i_new_size);

  if (i_old_size==i_new_size)
    for (size_t i=0;i<i_new_size;++i)
      {
      weights[i].m_first_texel = (int)i;
      weights[i].m_weights[0] = 1.0;
      weights[i].m_weights[1] = weights[i].m_weights[2] = weights[i].m_weights[3] = 0.0;
      }
  else
    for (size_t i=0;i<i_new_size;++i)
      {
      // Compute image resampling weights for i-th texel.
      double center = (i + 0.5) * i_old_size / i_new_size;
      weights[i].m_first_texel = (int)floor((center - filter_width) + 0.5);
      for (unsigned char j=0;j<4;++j)
        {
        double pos = weights[i].m_first_texel + j + 0.5;
        weights[i].m_weights[j] = SamplingRoutines::Lanczos((pos - center) / filter_width, 2.0);
        }
      }

  return weights;
  }

template <typename T>
const T &MIPMap<T>::_GetTexel(size_t i_level, int i_x, int i_y) const
  {
  ASSERT(i_level < m_num_levels);

  const BlockedArray<T> &level = *m_levels[i_level];

  if (m_repeat)
    {
    i_y = MathRoutines::Mod(i_y, (int)level.GetSizeU()); // This can be optimized by using the fact that the size is always a power of 2.
    i_x = MathRoutines::Mod(i_x, (int)level.GetSizeV());
    }
  else
    if (i_y<0 || i_y>=(int)level.GetSizeU() || i_x<0 || i_x>=(int)level.GetSizeV())
      {
      static const T black = T();
      return black;
      }

  return level.Get(i_y,i_x);
  }

template <typename T>
T MIPMap<T>::Evaluate(const Point2D_d &i_point, double i_width)
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
    return _GetTexel(m_num_levels-1, 0, 0);
  else
    {
    double delta = level - (size_t)level;
    return (1.0-delta) * _Interpolate((size_t)level, i_point) + delta * _Interpolate((size_t)level+1, i_point);
    }
  }

template <typename T>
T MIPMap<T>::Evaluate(const Point2D_d &i_point, Vector2D_d i_dxy_1, Vector2D_d i_dxy_2)
  {
  // Compute ellipse minor and major axes.
  if (i_dxy_1.LengthSqr() < i_dxy_2.LengthSqr())
    std::swap(i_dxy_1, i_dxy_2);

  double major_length = sqrt(i_dxy_1.LengthSqr());
  double minor_length = sqrt(i_dxy_2.LengthSqr());

  if (minor_length < DBL_EPS)
    return _Interpolate(0, i_point);

  // Clamp ellipse eccentricity if too large.
  if (minor_length * m_max_anisotropy < major_length) {
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
    return _GetTexel(m_num_levels-1, 0, 0);
  else
    {
    // Choose level of detail for EWA lookup and perform EWA filtering.
    double delta = level - (size_t)level;
    return (1.0-delta) * _EWA((size_t)level, i_point, i_dxy_1, i_dxy_2) + delta * _EWA((size_t)level+1, i_point, i_dxy_1, i_dxy_2);
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

  return
    (1.0-dx)*(1.0-dy) * _GetTexel(i_level, x  , y  ) +
    (1.0-dx)*dy       * _GetTexel(i_level, x  , y+1) +
    dx*(1.0-dy)       * _GetTexel(i_level, x+1, y  ) +
    dx*dy             * _GetTexel(i_level, x+1, y+1);
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

      // Compute squared radius and filter texel if inside ellipse.
      double r_sqr = A*xx*xx + B*xx*yy + C*yy*yy;
      if (r_sqr < 1.0)
        {
        ASSERT(r_sqr>=0.0);
        double weight = m_EWA_weights[std::min((size_t)floor(r_sqr * EWA_WEIGHTS_NUM), EWA_WEIGHTS_NUM-1)];
        num += _GetTexel(i_level, x, y) * weight;
        den += weight;
        }
      }
    }

  ASSERT(den > 0.0);
  return num / den;
  }

#endif // MIP_MAP_H