#ifndef SAMPLING_TEST_HELPER_H
#define SAMPLING_TEST_HELPER_H

/*
This file contains various helper functions that test that a given 1D or 2D samples sequence has a certain distribution.
For example there are functions to test that samples have uniform distribution or that the samples are stratified etc.

These functions are called from different test suites testing that a samples sequence has a certain property.
*/

#include <Math/Point2D.h>
#include <Math/MathRoutines.h>
#include <cmath>
#include <vector>
#include <algorithm>

namespace SamplingTestHelper
  {
  /*
  This method checks that the given 1D distribution of sample values has the same mean and variance as the truly uniform would have.
  Returns true if the test passes.
  */
  template<typename T>
  bool TestUniformDistribution1D(const std::vector<T> &i_values, T i_low, T i_high);

  /*
  This method checks that the given 2D distribution of sample values has the same mean and variance as the truly uniform would have.
  Returns true if the test passes.
  */
  template<typename T>
  bool TestUniformDistribution2D(const std::vector<Point2D<T> > &i_values, Point2D<T> i_low, Point2D<T> i_high);

  /*
  This method checks that the given 1D distribution of samples does not have regions of clumping samples.
  This test checks the same property that the stratified sampling has, that there is no segment of one strata's size with more than two samples inside.
  Return true if there are no clumping regions.

  The method takes vector of samples by value instead of a reference because it needs to change it, so this is equal to passing by a reference and then making a local copy.
  */
  template<typename T>
  bool TestSamplesClumping1D(std::vector<T> i_values, T i_low, T i_high);

  /*
  This method checks that the given 2D distribution of samples does not have regions of clumping samples.
  This test checks the same property that the stratified sampling has, that there is no segment of one strata's size with more than four samples inside.
  Return true if there are no clumping regions.

  The method takes vector of samples by value instead of a reference because it needs to change it, so this is equal to passing by a reference and then making a local copy.
  */
  template<typename T>
  bool TestSamplesClumping2D(std::vector<Point2D<T> > i_values, Point2D<T> i_low, Point2D<T> i_high, size_t i_x_num_samples, size_t i_y_num_samples);

  /*
  This method checks that the given 2D distribution of samples does not have regions of clumping samples.
  This test checks the same property that the Latin Hypercube Sampling has, that in each dimension there is no segment of one strata's size with more than two samples inside.
  The difference between this and TestSamplesClumping2D() method is that this method tests each dimension separately while TestSamplesClumping2D() looks for the
  clumping in 2D taking both dimensions into the account at the same time.

  Return true if there are no clumping regions.

  The method takes vector of samples by value instead of a reference because it needs to change it, so this is equal to passing by a reference and then making a local copy.
  */
  template<typename T>
  bool TestLatinHypercubeDistribution2D(std::vector<Point2D<T> > i_values, Point2D<T> i_low, Point2D<T> i_high);

  /*
  This method checks that the given 2D distribution of samples conforms to the (0,2)-sequence property.
  In other words for samples sequence with 2^(k+l) values there should be exactly one sample in each strata if the domain was uniformly divided into (2^k)*(2^l) stratas.
  The input vector length should be a power of 2.
  */
  template<typename T>
  bool TestLD02Distribution2D(const std::vector<Point2D<T> > &i_values, Point2D<T> i_low, Point2D<T> i_high);

  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace SamplingTestHelper
  {

  template<typename T>
  bool TestUniformDistribution1D(const std::vector<T> &i_values, T i_low, T i_high)
    {
    size_t N=i_values.size();
    double mean=0.0;
    for(size_t i=0;i<N;++i)
      mean+=i_values[i];

    mean/=N;

    double variance=0.0;
    for(size_t i=0;i<N;++i)
      variance+=(i_values[i]-mean)*(i_values[i]-mean);
    variance/=N;

    double delta=i_high-i_low;

    // These are empirical formulas.
    double mean_error=1.5*exp(0.12)*delta/sqrt((double)N);
    double variance_error=1.5*exp(-1.3)*delta*delta/sqrt((double)N);

    bool mean_in_range = true;
    if (fabs(mean-(i_low+i_high)/2.0) > mean_error)
      mean_in_range=false;

    bool variance_in_range = true;
    if (fabs(variance-delta*delta/12.0) > variance_error)
      variance_in_range=false;

    return mean_in_range && variance_in_range;
    }

  template<typename T>
  bool TestUniformDistribution2D(const std::vector<Point2D<T> > &i_values, Point2D<T> i_low, Point2D<T> i_high)
    {
    size_t N=i_values.size();
    std::vector<T> values_x, values_y;
    values_x.reserve(N);
    values_y.reserve(N);
    for(size_t i=0;i<N;++i)
      {
      values_x.push_back(i_values[i][0]);
      values_y.push_back(i_values[i][1]);
      }

    bool uniform_x = TestUniformDistribution1D(values_x,i_low[0],i_high[0]);
    bool uniform_y = TestUniformDistribution1D(values_y,i_low[1],i_high[1]);
    return uniform_x && uniform_y;
    }

  template<typename T>
  bool TestSamplesClumping1D(std::vector<T> i_values, T i_low, T i_high)
    {
    size_t N=i_values.size();
    double strata_size = (i_high-i_low)/N - DBL_EPS;

    std::sort(i_values.begin(), i_values.end());
    
    for(size_t i=0;i<N;++i)
      {
      double left=i_values[i];
      double right=left+strata_size;

      // Lower bound is used for both bounds to exclude the samples with the same value as "right".
      std::vector<T>::iterator it1=std::lower_bound(i_values.begin(), i_values.end(), left);
      std::vector<T>::iterator it2=std::lower_bound(i_values.begin(), i_values.end(), right);

      if (std::distance(it1,it2) > 2)
        return false;
      }

    return true;
    }

  // Internal helper class used for sorting Point2D objects by either X or Y coordinate.
  template<typename T, int D>
  class Point2DComparator
    {
    public:
      bool operator()(const Point2D<T> &i_left, const Point2D<T> &i_right) const
        {
        return i_left[D]<i_right[D];
        }
    };

  template<typename T>
  bool TestSamplesClumping2D(std::vector<Point2D<T> > i_values, Point2D<T> i_low, Point2D<T> i_high, size_t i_x_num_samples, size_t i_y_num_samples)
    {
    const size_t N = i_x_num_samples*i_y_num_samples;
    ASSERT(i_values.size()==N);
    if (i_values.size()!=N)
      return false;

    double strata_size_x = (i_high[0]-i_low[0])/i_x_num_samples - DBL_EPS;
    double strata_size_y = (i_high[1]-i_low[1])/i_y_num_samples - DBL_EPS;

    // Sort by X coordinate.
    std::sort(i_values.begin(), i_values.end(), Point2DComparator<T,0>());

    std::vector<Point2D<T> > clumped_by_x;
    for(size_t i=0;i<N;++i)
      {
      double left=i_values[i][0];
      double right=left+strata_size_x;

      // Lower bound is used for both bounds to exclude the samples with the same value as "right".
      std::vector<Point2D<T> >::iterator it1=std::lower_bound(i_values.begin(), i_values.end(), Point2D_d(left,0.0), Point2DComparator<T,0>());
      std::vector<Point2D<T> >::iterator it2=std::lower_bound(i_values.begin(), i_values.end(), Point2D_d(right,0.0), Point2DComparator<T,0>());

      // No need to look for clumping by Y if there is less than 5 points.
      if (std::distance(it1,it2)<5)
        continue;

      // Copy the points in the clumped X region into a separate vector and sort it by Y coordinate.
      clumped_by_x.assign(it1,it2);
      std::sort(clumped_by_x.begin(),clumped_by_x.end(),Point2DComparator<T,1>());

      // Search for 5 points the are clumped by Y coordinate as well.
      for(size_t j=0;j+4<clumped_by_x.size();++j)
        if (clumped_by_x[j+4][1]-clumped_by_x[j][1]<strata_size_y)
          return false;
      }

    return true;
    }

  template<typename T>
  bool TestLatinHypercubeDistribution2D(std::vector<Point2D<T> > i_values, Point2D<T> i_low, Point2D<T> i_high)
    {
    size_t N=i_values.size();
    std::vector<T> values_x, values_y;
    values_x.reserve(N);
    values_y.reserve(N);
    for(size_t i=0;i<N;++i)
      {
      values_x.push_back(i_values[i][0]);
      values_y.push_back(i_values[i][1]);
      }

    bool not_clumped_x = TestSamplesClumping1D(values_x,i_low[0],i_high[0]);
    bool not_clumped_y = TestSamplesClumping1D(values_y,i_low[1],i_high[1]);
    return not_clumped_x && not_clumped_y;
    }

  template<typename T>
  bool TestLD02Distribution2D(const std::vector<Point2D<T> > &i_values, Point2D<T> i_low, Point2D<T> i_high)
    {
    size_t N=i_values.size();
    ASSERT(MathRoutines::IsPowerOf2(N));
    size_t p = MathRoutines::CeilLog2(N);

    for(size_t i=0;i<=p;++i)
      if (_CheckLD02Stratification(i_values, i_low, i_high, 1<<i, 1<<(p-i))==false)
        return false;

    return true;
    }

  template<typename T>
  bool _CheckLD02Stratification(const std::vector<Point2D<T> > &i_values, Point2D<T> i_low, Point2D<T> i_high, size_t i_n, size_t i_m)
    {
    ASSERT(i_values.size() == i_n*i_m);
    std::vector<std::vector<bool> > hit(i_n, std::vector<bool>(i_m, false));

    for(size_t i=0;i<i_values.size();++i)
      {
      int index1 = (int)( i_n*(i_values[i][0]-i_low[0])/(i_high[0]-i_low[0]) );
      int index2 = (int)( i_m*(i_values[i][1]-i_low[1])/(i_high[1]-i_low[1]) );

      if (index1<0 || index2<0 || index1>=(int)i_n || index2>=(int)i_m)
        return false;

      if (hit[index1][index2])
        return false;
      else
        hit[index1][index2] = true;
      }

    return true;
    }

  };

#endif // SAMPLING_TEST_HELPER_H