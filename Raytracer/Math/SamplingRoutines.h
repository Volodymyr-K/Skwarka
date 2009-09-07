#ifndef SAMPLING_ROUTINES_H
#define SAMPLING_ROUTINES_H

#include <vector>
#include <Math/Geometry.h>
#include <Math/ThreadSafeRandom.h>

/**
* This namespace contains helper routines for sampling and samples generation.
* These routines are mostly used for Monte Carlo integration algorithms.
*/
namespace SamplingRoutines
  {
  /**
  Maps 2D sample in [0;1]^2 to a unit radius disk.
  The "concentric" algorithm belongs to Peter Shirley and prevents the area distortion (elongation and/or compression).
  @param i_sample Input 2D sample in [0;1]^2.
  @return Resulting 2D point in the unit radius disk.
  */
  Point2D_d ConcentricDiskSampling(const Point2D_d i_sample);

  /**
  Fills the specified range with stratified 1D values in [0;1] range. ValueIterator is a random-access iterator type.
  @param i_begin Begin iterator of the range to be filled with the values.
  @param i_samples_num Number of samples.
  @param i_jitter_samples If true the samples will be randomly moved inside their stratas.
  */
  template<typename ValueIterator>
  void StratifiedSampling1D(ValueIterator i_begin, size_t i_samples_num, bool i_jitter_samples);

  /**
  Fills the specified range with stratified 2D values in [0;1]^2 range. Point2DIterator is a random-access iterator type.
  @param i_begin Begin iterator of the range to be filled with the values.
  @param i_x_samples_num Number of samples in x direction.
  @param i_y_samples_num Number of samples in y direction.
  @param i_jitter_samples If true the samples will be randomly moved inside their stratas.
  */
  template<typename Point2DIterator>
  void StratifiedSampling2D(Point2DIterator i_begin, size_t i_x_samples_num, size_t i_y_samples_num, bool i_jitter_samples);

  /**
  Fills the specified range with 2D values produced by the LatinHypecube algorithm. Point2DIterator is a random-access iterator type.
  @param i_begin Begin iterator of the range to be filled with the values.
  @param i_samples_num Number of samples.
  @param i_jitter_samples If true the samples will be randomly moved inside their stratas.
  */
  template<typename Point2DIterator>
  void LatinHypercubeSampling2D(Point2DIterator i_begin, size_t i_samples_num, bool i_jitter_samples);

  /**
  Randomly shuffles values in the specified vector.
  */
  template<typename T>
  void Shuffle(std::vector<T> &io_values);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace SamplingRoutines
  {
  inline Point2D_d ConcentricDiskSampling(const Point2D_d i_sample)
    {
    ASSERT(i_sample[0]>=0.0 && i_sample[0]<=1.0 && i_sample[1]>=0.0 && i_sample[1]<=1.0);
    double r, theta;

    // Map uniform random numbers to [-1,1]^2
    Point2D_d s = 2.0*i_sample - Point2D_d(1,1);

    // Map square to (r,theta)
    // Handle degeneracy at the origin.
    if (s == Point2D_d(0,0))
      return Point2D_d(0,0);

    if (s[0] >= -s[1])
      {
      if (s[0] > s[1])
        {
        // Handle first region of disk.
        r = s[0];
        if (s[1] > 0.0)
          theta = s[1]/r;
        else
          theta = 8.0 + s[1]/r;
        }
      else
        {
        // Handle second region of disk.
        r = s[1];
        theta = 2.0 - s[0]/r;
        }
      }
    else 
      {
      if (s[0] <= s[1])
        {
        // Handle third region of disk.
        r = -s[0];
        theta = 4.0 - s[1]/r;
        }
      else
        {
        // Handle fourth region of disk.
        r = -s[1];
        theta = 6.0 + s[0]/r;
        }
      }

    theta *= M_PI / 4.0;
    return Point2D_d(r*cos(theta), r*sin(theta));
    }

  template<typename ValueIterator>
  void StratifiedSampling1D(ValueIterator i_begin, size_t i_samples_num, bool i_jitter_samples)
    {
    if(i_samples_num==0)
      return;

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
    if(i_x_samples_num==0 || i_y_samples_num==0)
      return;

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

    // Generate LHS samples along diagonal.
    double delta = 1.0 / i_samples_num;
    for (size_t i = 0; i < i_samples_num; ++i)
      {
      double jx = i_jitter_samples ? RandomDouble(1.0) : 0.5;
      double jy = i_jitter_samples ? RandomDouble(1.0) : 0.5;
      *(i_begin+i) = Point2D_d( (i + jx) * delta, (i + jy) * delta );
      }

    // Permute LHS samples in each dimension.
    for (size_t i = 0; i < 2; ++i)
      for (size_t j = 0; j < i_samples_num; ++j)
        {
        size_t other = RandomInt(i_samples_num);
        std::swap( (*(i_begin+j)) [i], (*(i_begin+other)) [i] );
        }
    }

  template<typename T>
  void Shuffle(std::vector<T> &io_values)
    {
    for (size_t i = 0; i < io_values.size(); ++i)
      {
      unsigned int other = RandomInt(io_values.size());
      std::swap(io_values[i], io_values[other]);
      }
    }

  };


#endif // SAMPLING_ROUTINES_H