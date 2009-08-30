#ifndef SAMPLING_ROUTINES_H
#define SAMPLING_ROUTINES_H

#include <vector>
#include <Math\Geometry.h>

namespace SamplingRoutines
  {
  template<typename T>
  void ConcentricSampleDisk(const Point2D<T> i_sample, Point2D<T> &o_point);

  //template<typename T, typename RandomGeneratorType = void>
  //void StratifiedSample2D(std::vector<Point2D<T> > o_samples, size_t m_x_samples, size_t m_y_samples, bool i_jitter_samples, RandomGeneratorType *ip_random_generator = NULL);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace SamplingRoutines
  {
  template<typename T>
  void ConcentricSampleDisk(const Point2D<T> i_sample, Point2D<T> &o_point)
    {
    double r, theta;

    // Map uniform random numbers to [-1,1]^2
    Point2D<T> s = 2.0*i_sample - Point2D<T>(1,1);

    // Map square to (r,theta)
    // Handle degeneracy at the origin
    if (s == Point2D<T>(0,0))
      {
      o_point = Point2D<T>(0,0);
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
    o_point = Point2D<T>(r*cos(theta), r*sin(theta));
    }
/*
  template<typename T, typename RandomGeneratorType = void>
  void StratifiedSample2D(std::vector<Point2D<T> > o_samples, size_t m_x_samples, size_t m_y_samples, bool i_jitter_samples, RandomGeneratorType *ip_random_generator)
    {
    ASSERT(m_x_samples>0 && m_y_samples>0);
    ASSERT(i_jitter_samples==false || ip_random_generator!=NULL);

    o_samples.clear();

    T inv_x_samples = 1.0/m_x_samples;
    T inv_y_samples = 1.0/m_y_samples;
    for (int y = 0; y < m_y_samples; ++y)
      for (int x = 0; x < m_x_samples; ++x)
        {
        T jx = i_jitter_samples ? (*ip_random_generator)(1.0) : 0.5f;
        T jy = i_jitter_samples ? (*ip_random_generator)(1.0) : 0.5f;
        o_samples.push_back( (x + jx) * inv_x_samples, (y + jy) * inv_y_samples );
        }
    }
*/
  };

#endif // SAMPLING_ROUTINES_H