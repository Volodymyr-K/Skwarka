#ifndef SAMPLING_ROUTINES_H
#define SAMPLING_ROUTINES_H

#include <vector>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <Math/RandomGenerator.h>
#include <Math/ThreadSafeRandom.h>

/**
* This namespace contains helper routines for sampling and samples generation.
* These routines are mostly used for Monte Carlo integration algorithms.
*/
namespace SamplingRoutines
  {
  /**
  * Maps 2D sample in [0;1]x[0;1] to a unit radius disk.
  * The "concentric" algorithm belongs to Peter Shirley and prevents the area distortion (elongation and/or compression).
  * @param i_sample Input 2D sample in [0;1]x[0;1].
  * @return Resulting 2D point in the unit radius disk.
  */
  Point2D_d ConcentricDiskSampling(const Point2D_d &i_sample);

  /**
  * Maps 2D sample in [0;1]x[0;1] to a hemisphere point uniformly.
  * The hemisphere is considered to be centered at origin above the XY plane.
  * @param i_sample Input 2D sample in [0;1]x[0;1].
  * @return Resulting 3D vector pointing to a point on the hemisphere surface.
  */
  Vector3D_d UniformHemisphereSampling(const Point2D_d &i_sample);

  /**
  * Returns PDF value for hemisphere uniform sampling.
  * The returned value is constant over the hemisphere.
  */
  double UniformHemispherePDF();

  /**
  * Maps 2D sample in [0;1]x[0;1] to a sphere point uniformly.
  * The sphere is considered to be centered at origin.
  * @param i_sample Input 2D sample in [0;1]x[0;1].
  * @return Resulting 3D vector pointing to a point on the sphere surface.
  */
  Vector3D_d UniformSphereSampling(const Point2D_d &i_sample);

  /**
  * Returns PDF value for sphere uniform sampling.
  * The returned value is constant over the sphere.
  */
  double UniformSpherePDF();

  /**
  * Maps 2D sample in [0;1]x[0;1] to a hemisphere point with theta cosine distribution.
  * The hemisphere is considered to be centered at origin above the XY plane.
  * @param i_sample Input 2D sample in [0;1]x[0;1].
  * @return Resulting 3D vector pointing to a point on the hemisphere surface.
  */
  Vector3D_d CosineHemisphereSampling(const Point2D_d &i_sample);

  /**
  * Returns PDF value for hemisphere cosine sampling for the specified theta angle cosine.
  * @param i_cos_theta Cosine of the theta angle. Should be in [0;1] range.
  * @return PDF value.
  */
  double CosineHemispherePDF(double i_cos_theta);

  /**
  * Maps 2D sample in [0;1]x[0;1] to a direction lying inside of the cone with specified spread angle.
  * The axis of the cone is equal to Z axis.
  * @param i_sample Input 2D sample in [0;1]x[0;1].
  * @param i_cos_theta_max Cosine of the cone spread angle. Should be in [0;1) range.
  * @return Resulting 3D vector inside the cone. Should be normalized.
  */
  Vector3D_d UniformConeSampling(const Point2D_d &i_sample, double i_cos_theta_max);

  /**
  * Returns PDF value for uniform cone sampling for the specified specified spread angle.
  * @param i_cos_theta_max Cosine of the cone spread angle. Should be in [0;1) range.
  * @return PDF value.
  */
  double UniformConePDF(double i_cos_theta_max);

  /**
  * Maps 2D sample in [0;1]x[0;1] to a triangle point uniformly.
  * The method returns three barycentric coordinates so it does not actually depend on the vertices coordinates.
  * @param i_sample Input 2D sample in [0;1]x[0;1].
  * @param[out] o_b1 First barycentric coordinates.
  * @param[out] o_b2 Second barycentric coordinates.
  */
  void UniformTriangleSampling(const Point2D_d &i_sample, double &o_b1, double &o_b2);

  /**
  * Fills the specified range with stratified 1D values in [0;1] range. ValueIterator is a random-access iterator type.
  * @param i_begin Begin iterator of the range to be filled with the values.
  * @param i_samples_num Number of samples.
  * @param i_jitter_samples If true the samples will be randomly moved inside their stratas.
  * @param ip_rng Random number generator. If NULL, thread-safe global implementation will be used.
  */
  template<typename ValueIterator>
  void StratifiedSampling1D(ValueIterator i_begin, size_t i_samples_num, bool i_jitter_samples, RandomGenerator<double> *ip_rng = NULL);

  /**
  * Fills the specified range with stratified 2D values in [0;1]x[0;1] range. Point2DIterator is a random-access iterator type.
  * @param i_begin Begin iterator of the range to be filled with the values.
  * @param i_x_samples_num Number of samples in x direction.
  * @param i_y_samples_num Number of samples in y direction.
  * @param i_jitter_samples If true the samples will be randomly moved inside their stratas.
  * @param ip_rng Random number generator. If NULL, thread-safe global implementation will be used.
  */
  template<typename Point2DIterator>
  void StratifiedSampling2D(Point2DIterator i_begin, size_t i_x_samples_num, size_t i_y_samples_num, bool i_jitter_samples, RandomGenerator<double> *ip_rng = NULL);

  /**
  * Fills the specified range with 2D values produced by the LatinHypecube algorithm. Point2DIterator is a random-access iterator type.
  * @param i_begin Begin iterator of the range to be filled with the values.
  * @param i_samples_num Number of samples.
  * @param i_jitter_samples If true the samples will be randomly moved inside their stratas.
  * @param ip_rng Random number generator. If NULL, thread-safe global implementation will be used.
  */
  template<typename Point2DIterator>
  void LatinHypercubeSampling2D(Point2DIterator i_begin, size_t i_samples_num, bool i_jitter_samples, RandomGenerator<double> *ip_rng = NULL);

  /**
  * Randomly shuffles values in the specified range.
  * @param i_begin Begin iterator of the range to be shuffled.
  * @param i_samples_num Number of values in the range.
  * @param ip_rng Random number generator. If NULL, thread-safe global implementation will be used.
  */
  template<typename ValueIterator>
  void Shuffle(ValueIterator i_begin, size_t i_samples_num, RandomGenerator<double> *ip_rng = NULL);

  /**
  * Computes weighting coefficient for multiple importance sampling with two PDFs.
  */
  double BalanceHeuristic(size_t i_samples_num1, double i_pdf1, size_t i_samples_num2, double i_pdf2);

  /**
  * Computes weighting coefficient for multiple importance sampling with two PDFs. The exponent value is hard-coded as 2 (as recommended by Veach).
  */
  double PowerHeuristic(size_t i_samples_num1, double i_pdf1, size_t i_samples_num2, double i_pdf2);

  /**
  * Lanczos filter function (a windowed form of the sinc filter).
  * @param i_x Point at which the filter is to be evaluated. Should be normalized, i.e. should be in [-1;1] range (otherwise 0.0 is returned).
  * @param i_tau Filter width.
  */
  double Lanczos(double i_x, double i_tau);

  /**
  * Generates low-discrepancy sample values with a radical inverse function in base 2 (this is what the VanDerCorput sequence is).
  * The function also scrambles the sample values using the specified scramble value.
  * Scrambled value are still well distributed (given they are scrambled with the same scramble value).
  * @param i_n Index of the sample value.
  * @param i_scramble Scramble value, can take any possible value.
  * @return Sample value. Should be in [0;1] range.
  */
  double VanDerCorput(unsigned int i_n, unsigned int i_scramble);

  /**
  * Generates low-discrepancy sample values with Sobol' function.
  * The function also scrambles the sample values using the specified scramble value.
  * Scrambled value are still well distributed (given they are scrambled with the same scramble value).
  * @param i_n Index of the sample value.
  * @param i_scramble Scramble value, can take any possible value.
  * @return Sample value. Should be in [0;1] range.
  */
  double Sobol2(unsigned int i_n, unsigned int i_scramble);

  /**
  * Generates low-discrepancy sample values with a radical inverse function in the specified base.
  * @param i_n Index of the sample value.
  * @param i_base Base value. Should be greater or equal than 2.
  * @return Sample value. Should be in [0;1] range.
  */
  double RadicalInverse(unsigned int i_n, unsigned int i_base);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace SamplingRoutines
  {
  inline Point2D_d ConcentricDiskSampling(const Point2D_d &i_sample)
    {
    ASSERT(i_sample[0]>=0.0 && i_sample[0]<1.0 && i_sample[1]>=0.0 && i_sample[1]<1.0);
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

  inline Vector3D_d UniformHemisphereSampling(const Point2D_d &i_sample)
    {
    ASSERT(i_sample[0]>=0.0 && i_sample[0]<1.0 && i_sample[1]>=0.0 && i_sample[1]<1.0);

    double z = i_sample[0];
    double r = sqrt(std::max(0.0, 1.0 - z*z));
    double phi = 2.0 * M_PI * i_sample[1];
    double x = r * cos(phi);
    double y = r * sin(phi);
    return Vector3D_d(x, y, z);
    }

  inline double UniformHemispherePDF()
    {
    return INV_2PI;
    }

  inline Vector3D_d UniformSphereSampling(const Point2D_d &i_sample)
    {
    ASSERT(i_sample[0]>=0.0 && i_sample[0]<1.0 && i_sample[1]>=0.0 && i_sample[1]<1.0);

    double z = 1.0 - 2.0*i_sample[0];
    double r = sqrt(std::max(0.0, 1.0 - z*z));
    double phi = 2.0 * M_PI * i_sample[1];
    double x = r * cos(phi);
    double y = r * sin(phi);
    return Vector3D_d(x, y, z);
    }

  inline double UniformSpherePDF()
    {
    return 1.0 / (4.0*M_PI);
    }

  inline Vector3D_d CosineHemisphereSampling(const Point2D_d &i_sample)
    {
    ASSERT(i_sample[0]>=0.0 && i_sample[0]<1.0 && i_sample[1]>=0.0 && i_sample[1]<1.0);

    // The simplest way to cosine sample the hemisphere is to sample the disk uniformly and then project the point onto the hemisphere.
    Point2D_d disk_point = ConcentricDiskSampling(i_sample);

    double z = sqrt(std::max(0.0, 1.0 - disk_point[0]* disk_point[0] - disk_point[1]* disk_point[1]));
    return Vector3D_d(disk_point[0], disk_point[1], z);
    }

  inline double CosineHemispherePDF(double i_cos_theta)
    {
    ASSERT(i_cos_theta>=0.0 && i_cos_theta<=1.0);

    return i_cos_theta * INV_PI;
    }

  inline Vector3D_d UniformConeSampling(const Point2D_d &i_sample, double i_cos_theta_max)
    {
    ASSERT(i_cos_theta_max >= 0.0 && i_cos_theta_max < 1.0);

    double cos_theta = (1.0 - i_sample[0]) + i_sample[0] * i_cos_theta_max;
    double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
    double phi = i_sample[1] * 2.0 * M_PI;

    return Vector3D_d(sin_theta*cos(phi), sin_theta*sin(phi), cos_theta);
    }

  inline double UniformConePDF(double i_cos_theta_max)
    {
    ASSERT(i_cos_theta_max >= 0.0 && i_cos_theta_max < 1.0);
    return 1.0 / (2.0 * M_PI * (1.0 - i_cos_theta_max));
    }

  inline void UniformTriangleSampling(const Point2D_d &i_sample, double &o_b1, double &o_b2)
    {
    ASSERT(i_sample[0]>=0.0 && i_sample[0]<1.0 && i_sample[1]>=0.0 && i_sample[1]<1.0);
    double tmp = sqrt(i_sample[0]);
    o_b1 = 1.0 - tmp;
    o_b2 = i_sample[1] * tmp;
    }

  template<typename ValueIterator>
  void StratifiedSampling1D(ValueIterator i_begin, size_t i_samples_num, bool i_jitter_samples, RandomGenerator<double> *ip_rng)
    {
    if(i_samples_num==0)
      return;

    ValueIterator it = i_begin;
    double inv_samples_num = 1.0/i_samples_num;

    if (i_jitter_samples)
      {
      if (ip_rng)
        for (size_t i = 0; i < i_samples_num; ++i)
          *(it++) = (i + (*ip_rng)(1.0)) * inv_samples_num;
      else
        for (size_t i = 0; i < i_samples_num; ++i)
          *(it++) = (i + RandomDouble(1.0)) * inv_samples_num;
      }
    else
      for (size_t i = 0; i < i_samples_num; ++i)
        *(it++) = (i + 0.5) * inv_samples_num;
    }

  template<typename Point2DIterator>
  void StratifiedSampling2D(Point2DIterator i_begin, size_t i_x_samples_num, size_t i_y_samples_num, bool i_jitter_samples, RandomGenerator<double> *ip_rng)
    {
    if(i_x_samples_num==0 || i_y_samples_num==0)
      return;

    Point2DIterator it = i_begin;
    double inv_x_samples_num = 1.0/i_x_samples_num;
    double inv_y_samples_num = 1.0/i_y_samples_num;

    if (i_jitter_samples)
      {
      if (ip_rng)
        for (size_t y = 0; y < i_y_samples_num; ++y)
          for (size_t x = 0; x < i_x_samples_num; ++x)
            *(it++) = Point2D_d((x + (*ip_rng)(1.0)) * inv_x_samples_num, (y + (*ip_rng)(1.0)) * inv_y_samples_num);
      else
        for (size_t y = 0; y < i_y_samples_num; ++y)
          for (size_t x = 0; x < i_x_samples_num; ++x)
            *(it++) = Point2D_d((x + RandomDouble(1.0)) * inv_x_samples_num, (y + RandomDouble(1.0)) * inv_y_samples_num);
      }
    else
      for (size_t y = 0; y < i_y_samples_num; ++y)
        for (size_t x = 0; x < i_x_samples_num; ++x)
          *(it++) = Point2D_d((x + 0.5) * inv_x_samples_num, (y + 0.5) * inv_y_samples_num);
    }

  template<typename Point2DIterator>
  void LatinHypercubeSampling2D(Point2DIterator i_begin, size_t i_samples_num, bool i_jitter_samples, RandomGenerator<double> *ip_rng)
    {
    if (i_samples_num==0)
      return;

    double delta = 1.0 / i_samples_num;
    if (ip_rng)
      {
      // Generate LHS samples along diagonal.
      if (i_jitter_samples)
        for (size_t i = 0; i < i_samples_num; ++i)
          *(i_begin+i) = Point2D_d( (i + (*ip_rng)(1.0)) * delta, (i + (*ip_rng)(1.0)) * delta );
      else
        for (size_t i = 0; i < i_samples_num; ++i)
          *(i_begin+i) = Point2D_d( (i + 0.5) * delta, (i + 0.5) * delta );

      // Permute LHS samples in both dimensions.
      for (size_t j = 0; j < i_samples_num; ++j)
        {
        size_t other = (size_t) (*ip_rng)(i_samples_num);
        std::swap( (*(i_begin+j)) [0], (*(i_begin+other)) [0] );

        other = (size_t) (*ip_rng)(i_samples_num);
        std::swap( (*(i_begin+j)) [1], (*(i_begin+other)) [1] );
        }
      }
    else
      {
      // Generate LHS samples along diagonal.
      if (i_jitter_samples)
        for (size_t i = 0; i < i_samples_num; ++i)
          *(i_begin+i) = Point2D_d( (i + RandomDouble(1.0)) * delta, (i + RandomDouble(1.0)) * delta );
      else
        for (size_t i = 0; i < i_samples_num; ++i)
          *(i_begin+i) = Point2D_d( (i + 0.5) * delta, (i + 0.5) * delta );

      // Permute LHS samples in both dimensions.
      for (size_t j = 0; j < i_samples_num; ++j)
        {
        size_t other = (size_t) RandomInt((int)i_samples_num);
        std::swap( (*(i_begin+j)) [0], (*(i_begin+other)) [0] );

        other = RandomInt((int)i_samples_num);
        std::swap( (*(i_begin+j)) [1], (*(i_begin+other)) [1] );
        }
      }
    }

  template<typename ValueIterator>
  void Shuffle(ValueIterator i_begin, size_t i_samples_num, RandomGenerator<double> *ip_rng)
    {
    if (ip_rng)
      for (size_t i = 0; i < i_samples_num; ++i)
        {
        unsigned int other = (unsigned int) (*ip_rng)(i_samples_num);
        std::swap(*(i_begin+i), *(i_begin+other));
        }
    else
      for (size_t i = 0; i < i_samples_num; ++i)
        {
        unsigned int other = (unsigned int) RandomInt((int)i_samples_num);
        std::swap(*(i_begin+i), *(i_begin+other));
        }
    }

  inline double BalanceHeuristic(size_t i_samples_num1, double i_pdf1, size_t i_samples_num2, double i_pdf2)
    {
    ASSERT(i_samples_num1>=0 && i_pdf1>=0.0);
    ASSERT(i_samples_num2>=0 && i_pdf2>=0.0);
    return (i_samples_num1 * i_pdf1) / (i_samples_num1 * i_pdf1 + i_samples_num2 * i_pdf2);
    }

  inline double PowerHeuristic(size_t i_samples_num1, double i_pdf1, size_t i_samples_num2, double i_pdf2)
    {
    ASSERT(i_samples_num1>=0 && i_pdf1>=0.0);
    ASSERT(i_samples_num2>=0 && i_pdf2>=0.0);
    double f = i_samples_num1 * i_pdf1, g = i_samples_num2 * i_pdf2;
    return (f*f) / (f*f + g*g);
    }

  inline double Lanczos(double i_x, double i_tau)
    {
    i_x = fabs(i_x);
    if (i_x < 1e-5) return 1.0;
    if (i_x > 1.0) return 0.0;
    i_x *= M_PI;
    double s = sin(i_x * i_tau) / (i_x * i_tau);
    double lanczos = sin(i_x) / i_x;
    return s * lanczos;
    }

  inline double VanDerCorput(unsigned int i_n, unsigned int i_scramble)
    {
    ASSERT(sizeof(unsigned int)==4); // Better use a static assert but don't have a framework yet.

    i_n = (i_n << 16) | (i_n >> 16);
    i_n = ((i_n & 0x00ff00ff) << 8) | ((i_n & 0xff00ff00) >> 8);
    i_n = ((i_n & 0x0f0f0f0f) << 4) | ((i_n & 0xf0f0f0f0) >> 4);
    i_n = ((i_n & 0x33333333) << 2) | ((i_n & 0xcccccccc) >> 2);
    i_n = ((i_n & 0x55555555) << 1) | ((i_n & 0xaaaaaaaa) >> 1);
    i_n ^= i_scramble;

    return (double)i_n / (double)0x100000000LL;
    }

  inline double Sobol2(unsigned int i_n, unsigned int i_scramble)
    {
    ASSERT(sizeof(unsigned int)==4); // Better use a static assert but don't have a framework yet.

    for (unsigned int v = 1 << 31; i_n != 0; i_n >>= 1, v ^= v >> 1)
      if (i_n & 0x1) i_scramble ^= v;

    return (double)i_scramble / (double)0x100000000LL;
    }

  inline double RadicalInverse(unsigned int i_n, unsigned int i_base)
    {
    ASSERT(i_base>=2);

    double ret = 0.0;
    double inv_base = 1.0/i_base, inv = inv_base;
    while (i_n > 0)
      {
      // Compute next digit of radical inverse.
      int d_i = (i_n % i_base);
      ret += d_i * inv;
      i_n /= i_base;
      inv *= inv_base;
      }

    return ret;
    }

  };

#endif // SAMPLING_ROUTINES_H