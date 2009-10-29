#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Util.h>
#include <vector>

class UtilTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_SolveLinearSystem2x2()
      {
      double A[2][2]={{-1.5,2.0},{0.5,1.5}};
      double B[2]={1.2,5.0};
      double x[2];

      bool solved=MathRoutines::SolveLinearSystem2x2(A,B,x);
      TS_ASSERT(solved);

      double det=A[0][0]*A[1][1]-A[1][0]*A[0][1];
      double x0=(B[0]*A[1][1]-B[1]*A[0][1])/det;
      double x1=(A[0][0]*B[1]-A[1][0]*B[0])/det;
      TS_ASSERT_EQUALS(x[0],x0);
      TS_ASSERT_EQUALS(x[1],x1);
      }

    void test_SolveLinearSystem2x2Degenerated()
      {
      double A[2][2]={{-1.5,2.0},{3.0,-4.0}};
      double B[2]={1.2,5.0};
      double x[2];

      bool solved=MathRoutines::SolveLinearSystem2x2(A,B,x);
      TS_ASSERT(solved==false);
      }

    // Test when value is less than lower bound.
    void test_Clamp1()
      {
      double clamped = MathRoutines::Clamp(1.0, 2.0, 3.0);
      TS_ASSERT(clamped==2.0);
      }

    // Test when value is higher than upper bound.
    void test_Clamp2()
      {
      double clamped = MathRoutines::Clamp(5.0, 2.0, 3.0);
      TS_ASSERT(clamped==3.0);
      }

    // Test when value is inside the bounds.
    void test_Clamp3()
      {
      double clamped = MathRoutines::Clamp(2.2, 2.0, 3.0);
      TS_ASSERT(clamped==2.2);
      }

    void test_LinearInterpolate1()
      {
      double interpolated = MathRoutines::LinearInterpolate(0.0, 1.0, 2.0);
      TS_ASSERT(interpolated==1.0);
      }

    void test_LinearInterpolate2()
      {
      double interpolated = MathRoutines::LinearInterpolate(0.5, 1.0, -2.0);
      TS_ASSERT(interpolated==-0.5);
      }

    void test_LinearInterpolate3()
      {
      double interpolated = MathRoutines::LinearInterpolate(-1.0, 1.0, 3.0);
      TS_ASSERT(interpolated==-1.0);
      }

    void test_BinarySearchCDF1()
      {
      std::vector<double> cdf;
      cdf.push_back(1.0);

      TS_ASSERT_EQUALS(MathRoutines::BinarySearchCDF(cdf.begin(), cdf.end(), 0.4), cdf.begin()+0);
      }

    void test_BinarySearchCDF2()
      {
      std::vector<double> cdf;
      cdf.push_back(0.25);cdf.push_back(0.5);cdf.push_back(0.75);cdf.push_back(1.0);

      TS_ASSERT_EQUALS(MathRoutines::BinarySearchCDF(cdf.begin(), cdf.end(), 0.0), cdf.begin()+0);
      TS_ASSERT_EQUALS(MathRoutines::BinarySearchCDF(cdf.begin(), cdf.end(), 0.25), cdf.begin()+1);
      TS_ASSERT_EQUALS(MathRoutines::BinarySearchCDF(cdf.begin(), cdf.end(), 0.8), cdf.begin()+3);
      }

    // Tests for a special case when 0 is passed.
    void test_IsPowerOf2_ZeroCase()
      {
      TS_ASSERT_EQUALS(MathRoutines::IsPowerOf2(0), false);
      }

    void test_IsPowerOf2_PowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::IsPowerOf2(16), true);
      }

    void test_IsPowerOf2_NotPowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::IsPowerOf2(15), false);
      }

    // Tests for a special case when 0 is passed.
    void test_FloorLog2_ZeroCase()
      {
      TS_ASSERT_EQUALS(MathRoutines::FloorLog2(0),-1);
      }

    void test_FloorLog2_OneCase()
      {
      TS_ASSERT_EQUALS(MathRoutines::FloorLog2(1),0);
      }

    void test_FloorLog2_PowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::FloorLog2(16),4);
      }

    void test_FloorLog2_NotPowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::FloorLog2(15),3);
      }

    // Tests for a special case when 0 is passed.
    void test_CeilLog2_ZeroCase()
      {
      TS_ASSERT_EQUALS(MathRoutines::CeilLog2(0),-1);
      }

    void test_CeilLog2_OneCase()
      {
      TS_ASSERT_EQUALS(MathRoutines::CeilLog2(1),0);
      }

    void test_CeilLog2_PowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::CeilLog2(16),4);
      }

    void test_CeilLog2_NotPowerOf2Case()
      {
      TS_ASSERT_EQUALS(MathRoutines::CeilLog2(15),4);
      }

    void test_CoordinateSystem()
      {
      Vector3D_d e1(1.5,-0.5,1.0);
      e1.Normalize();

      Vector3D_d e2,e3;
      MathRoutines::CoordinateSystem(e1,e2,e3);

      TS_ASSERT(e2.IsNormalized());
      TS_ASSERT(e3.IsNormalized());

      TS_ASSERT_DELTA((e1^e2)[0],e3[0],(1e-10));
      TS_ASSERT_DELTA((e1^e2)[1],e3[1],(1e-10));
      TS_ASSERT_DELTA((e1^e2)[2],e3[2],(1e-10));

      TS_ASSERT_DELTA((e2^e3)[0],e1[0],(1e-10));
      TS_ASSERT_DELTA((e2^e3)[1],e1[1],(1e-10));
      TS_ASSERT_DELTA((e2^e3)[2],e1[2],(1e-10));
      }

    void test_SubtendedSolidAngle_PointInside()
      {
      BBox3D_d box(Point3D_d(1,2,3), Point3D_d(10,9,8));
      Point3D_d point(2,3,4);
      double angle = MathRoutines::SubtendedSolidAngle(point,box);
      TS_ASSERT_DELTA(angle, 4.0*M_PI, 1e-10);
      }

    void test_SubtendedSolidAngle_PointOnTheBoundaries()
      {
      BBox3D_d box(Point3D_d(1,2,3), Point3D_d(10,9,8));
      Point3D_d point(2,2,4);
      double angle = MathRoutines::SubtendedSolidAngle(point,box);
      TS_ASSERT_DELTA(angle, 4.0*M_PI, 1e-10);
      }

    void test_SubtendedSolidAngle_PointOutside1()
      {
      BBox3D_d box(Point3D_d(1,2,3), Point3D_d(10,9,8));
      Point3D_d point(-5,4,5);
      double angle = MathRoutines::SubtendedSolidAngle(point,box);

      double estimated = _EstimateSubtendedSolidAngle(point,box);
      TS_ASSERT_DELTA(angle, estimated, 1e-5)
      }

    void test_SubtendedSolidAngle_PointOutside2()
      {
      BBox3D_d box(Point3D_d(1,2,3), Point3D_d(10,9,8));
      Point3D_d point(-5,20,5);
      double angle = MathRoutines::SubtendedSolidAngle(point,box);

      double estimated = _EstimateSubtendedSolidAngle(point,box);
      TS_ASSERT_DELTA(angle, estimated, 1e-5)
      }

    void test_SubtendedSolidAngle_PointOutside3()
      {
      BBox3D_d box(Point3D_d(1,2,3), Point3D_d(10,9,8));
      Point3D_d point(-5,20,-2);
      double angle = MathRoutines::SubtendedSolidAngle(point,box);

      double estimated = _EstimateSubtendedSolidAngle(point,box);
      TS_ASSERT_DELTA(angle, estimated, 1e-5)
      }

  private:
    double _EstimateSubtendedSolidAngle(const Point3D_d &i_point, BBox3D_d i_bbox) const
      {
      double sum=0.0;

      size_t N=500;

      double dx=i_bbox.m_max[0]-i_bbox.m_min[0], dy=i_bbox.m_max[1]-i_bbox.m_min[1], dz=i_bbox.m_max[2]-i_bbox.m_min[2];
      double ds_xy = dx*dy/N/N, ds_xz = dx*dz/N/N, ds_yz = dy*dz/N/N;

      for(size_t i=0;i<N;++i)
        for(size_t j=0;j<N;++j)
          {
          Vector3D_d dir1 = Vector3D_d(i_bbox.m_min[0],i_bbox.m_min[1],i_bbox.m_min[2])+dy*(i+0.5)/N*Vector3D_d(0,1,0)+dz*(j+0.5)/N*Vector3D_d(0,0,1)-Vector3D_d(i_point);
          Vector3D_d dir2 = Vector3D_d(i_bbox.m_max[0],i_bbox.m_min[1],i_bbox.m_min[2])+dy*(i+0.5)/N*Vector3D_d(0,1,0)+dz*(j+0.5)/N*Vector3D_d(0,0,1)-Vector3D_d(i_point);

          double dist1_sqr = dir1.LengthSqr();
          double dist2_sqr = dir2.LengthSqr();

          sum += (fabs(dir1.Normalized()[0])/dist1_sqr + fabs(dir2.Normalized()[0])/dist2_sqr)*ds_yz;
          }

      for(size_t i=0;i<N;++i)
        for(size_t j=0;j<N;++j)
          {
          Vector3D_d dir1 = Vector3D_d(i_bbox.m_min[0],i_bbox.m_min[1],i_bbox.m_min[2])+dx*(i+0.5)/N*Vector3D_d(1,0,0)+dz*(j+0.5)/N*Vector3D_d(0,0,1)-Vector3D_d(i_point);
          Vector3D_d dir2 = Vector3D_d(i_bbox.m_min[0],i_bbox.m_max[1],i_bbox.m_min[2])+dx*(i+0.5)/N*Vector3D_d(1,0,0)+dz*(j+0.5)/N*Vector3D_d(0,0,1)-Vector3D_d(i_point);

          double dist1_sqr = dir1.LengthSqr();
          double dist2_sqr = dir2.LengthSqr();

          sum += (fabs(dir1.Normalized()[1])/dist1_sqr + fabs(dir2.Normalized()[1])/dist2_sqr)*ds_xz;
          }

      for(size_t i=0;i<N;++i)
        for(size_t j=0;j<N;++j)
          {
          Vector3D_d dir1 = Vector3D_d(i_bbox.m_min[0],i_bbox.m_min[1],i_bbox.m_min[2])+dx*(i+0.5)/N*Vector3D_d(1,0,0)+dy*(j+0.5)/N*Vector3D_d(0,1,0)-Vector3D_d(i_point);
          Vector3D_d dir2 = Vector3D_d(i_bbox.m_min[0],i_bbox.m_min[1],i_bbox.m_max[2])+dx*(i+0.5)/N*Vector3D_d(1,0,0)+dy*(j+0.5)/N*Vector3D_d(0,1,0)-Vector3D_d(i_point);

          double dist1_sqr = dir1.LengthSqr();
          double dist2_sqr = dir2.LengthSqr();

          sum += (fabs(dir1.Normalized()[2])/dist1_sqr + fabs(dir2.Normalized()[2])/dist2_sqr)*ds_xy;
          }

      return sum/2.0;
      }

  };

#endif // UTIL_TEST_H