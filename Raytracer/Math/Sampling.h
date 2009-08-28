#ifndef SAMPLING_H
#define SAMPLING_H

#include <Math\Geometry.h>

namespace Sampling
  {
  template<typename T>
  void ConcentricSampleDisk(const Point2D<T> i_sample, Point2D<T> &o_point);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Sampling
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
  };

#endif // SAMPLING_H