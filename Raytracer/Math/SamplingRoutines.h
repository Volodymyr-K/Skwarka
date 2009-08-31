#ifndef SAMPLING_ROUTINES_H
#define SAMPLING_ROUTINES_H

#include <vector>
#include <Math\Geometry.h>
#include <Math\MultiThreadedRandom.h>

namespace SamplingRoutines
  {
  void ConcentricDiskSampling(const Point2D_d i_sample, Point2D_d &o_point);

  template<typename ValueIterator>
  void StratifiedSampling1D(ValueIterator i_begin, size_t i_samples_num, bool i_jitter_samples);

  template<typename Point2DIterator>
  void StratifiedSampling2D(Point2DIterator i_begin, size_t i_x_samples_num, size_t i_y_samples_num, bool i_jitter_samples);

  template<typename Point2DIterator>
  void LatinHypercubeSampling2D(Point2DIterator i_begin, size_t i_samples_num, bool i_jitter_samples);

  template<typename T>
  void Shuffle(std::vector<T> &io_values);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace SamplingRoutines
  {
  inline void ConcentricDiskSampling(const Point2D_d i_sample, Point2D_d &o_point)
    {
    double r, theta;

    // Map uniform random numbers to [-1,1]^2
    Point2D_d s = 2.0*i_sample - Point2D_d(1,1);

    // Map square to (r,theta)
    // Handle degeneracy at the origin
    if (s == Point2D_d(0,0))
      {
      o_point = Point2D_d(0,0);
      return;
      }
    if (s[0] >= -s[1])
      {
      if (s[0] > s[1])
        {
        // Handle first region of disk
        r = s[0];
        if (s[1] > 0.0)
          theta = s[1]/r;
        else
          theta = 8.0 + s[1]/r;
        }
      else
        {
        // Handle second region of disk
        r = s[1];
        theta = 2.0 - s[0]/r;
        }
      }
    else 
      {
      if (s[0] <= s[1])
        {
        // Handle third region of disk
        r = -s[0];
        theta = 4.0 - s[1]/r;
        }
      else
        {
        // Handle fourth region of disk
        r = -s[1];
        theta = 6.0 + s[0]/r;
        }
      }

    theta *= M_PI / 4.0;
    o_point = Point2D_d(r*cos(theta), r*sin(theta));
    }

  template<typename ValueIterator>
  void StratifiedSampling1D(ValueIterator i_begin, size_t i_samples_num, bool i_jitter_samples)
    {
    ASSERT(i_samples_num>0);

    ValueIterator it = i_begin;
    double inv_samples_num = 1.0/i_samples_num;
    for (size_t i = 0; i < i_samples_num; ++i)
      {
      double jitter = i_jitter_samples ? RandomDouble(1.0) : 0.5;
      *(it++) = (i + jitter) * inv_samples_num;
      }
    }

  template<typename Point2DIterator>
  void StratifiedSampling2D(Point2DIterator i_begin, size_t i_x_samples_num, size_t i_y_samples_num, bool i_jitter_samples)
    {
    ASSERT(i_x_samples_num>0 && i_y_samples_num>0);

    Point2DIterator it = i_begin;
    double inv_x_samples_num = 1.0/i_x_samples_num;
    double inv_y_samples_num = 1.0/i_y_samples_num;
    for (size_t y = 0; y < i_y_samples_num; ++y)
      for (size_t x = 0; x < i_x_samples_num; ++x)
        {
        double jx = i_jitter_samples ? RandomDouble(1.0) : 0.5;
        double jy = i_jitter_samples ? RandomDouble(1.0) : 0.5;
        *(it++) = Point2D_d((x + jx) * inv_x_samples_num, (y + jy) * inv_y_samples_num);
        }
    }

  template<typename Point2DIterator>
  void LatinHypercubeSampling2D(Point2DIterator i_begin, size_t i_samples_num, bool i_jitter_samples)
    {
    if (i_samples_num==0)
      return;

    // Generate LHS samples along diagonal
    double delta = 1.0 / i_samples_num;
    for (size_t i = 0; i < i_samples_num; ++i)
      {
      double jx = i_jitter_samples ? RandomDouble(1.0) : 0.5;
      double jy = i_jitter_samples ? RandomDouble(1.0) : 0.5;
      *(i_begin+i) = Point2D_d( (i + jx) * delta, (i + jy) * delta );
      }

    // Permute LHS samples in each dimension
    for (size_t i = 0; i < 2; ++i)
      for (size_t j = 0; j < i_samples_num; ++j)
        {
        size_t other = RandomUInt(i_samples_num);
        std::swap( (*(i_begin+j)) [i], (*(i_begin+other)) [i] );
        }
    }

  template<typename T>
  void Shuffle(std::vector<T> &io_values)
    {
    for (size_t i = 0; i < io_values.size(); ++i)
      {
      unsigned int other = RandomUInt(io_values.size());
      std::swap(io_values[i], io_values[other]);
      }
    }

  };


#endif // SAMPLING_ROUTINES_H