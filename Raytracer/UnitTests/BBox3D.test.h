#ifndef BBOX3D_TEST_H
#define BBOX3D_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/BBox3D.h>

class BBox3DTestSuite : public CxxTest::TestSuite
  {
  public:
    // Most of the tests from this suite operate with the same sample BBox3D, so it is initialized in the setUp() method once.
    void setUp()
      {
      // Change these coordinates carefully as many tests below depends on the values.
      Point3D_d p_min(-1.0,-2.0,-3.0);
      Point3D_d p_max(1.0,2.5,3.5);
      m_bbox=BBox3D_d(p_min,p_max);
      } 

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_BBox3D_DefaultConstr()
      {
      BBox3D_d b1;
      TS_ASSERT_EQUALS(b1.m_min, Point3D_d(DBL_INF, DBL_INF, DBL_INF));
      TS_ASSERT_EQUALS(b1.m_max, Point3D_d(-DBL_INF, -DBL_INF, -DBL_INF));
      }

    void test_BBox3D_DefaultConstrWithCorners()
      {
      // The min corner is intentionally greater than the max corner to test that BBox does not swap them.
      Point3D_d p_min(1.0,2.5,3.5);
      Point3D_d p_max(-1.0,-2.0,-3.0);
      BBox3D_d b1(p_min,p_max);
      TS_ASSERT_EQUALS(b1.m_min, p_min);
      TS_ASSERT_EQUALS(b1.m_max, p_max);
      }

    void test_BBox3D_Volume()
      {
      double volume = m_bbox.Volume();
      TS_ASSERT_EQUALS(volume, 2.0*4.5*6.5);
      }

    void test_BBox3D_NegativeVolume()
      {
      Point3D_d p_min(1.0,-2.0,-3.0);
      Point3D_d p_max(-1.0,2.5,3.5);
      BBox3D_d b1(p_min,p_max);

      double volume = b1.Volume();
      TS_ASSERT_EQUALS(volume, -2.0*4.5*6.5);
      }

    void test_BBox3D_Area()
      {
      double volume = m_bbox.Area();
      double dx=m_bbox.m_max[0]-m_bbox.m_min[0], dy=m_bbox.m_max[1]-m_bbox.m_min[1], dz=m_bbox.m_max[2]-m_bbox.m_min[2];
      TS_ASSERT_EQUALS(volume, 2.0*(dx*dy+dx*dz+dy*dz));
      }

    void test_BBox3D_PointInsideCompletely()
      {
      Point3D_d p_test(0.1,0.2,0.3);
      TS_ASSERT(m_bbox.Inside(p_test));
      }

    void test_BBox3D_PointInsideOnFace()
      {
      Point3D_d p_test(0.0,0.0,-3.0);
      TS_ASSERT(m_bbox.Inside(p_test));
      }

    void test_BBox3D_PointInsideOnEdge()
      {
      Point3D_d p_test(0.0,-2.0,-3.0);
      TS_ASSERT(m_bbox.Inside(p_test));
      }

    void test_BBox3D_PointInsideOnVertex()
      {
      Point3D_d p_test(-1.0,-2.0,3.5);
      TS_ASSERT(m_bbox.Inside(p_test));
      }

    void test_BBox3D_PointNotInside()
      {
      Point3D_d p_test(10.1,0.2,0.3);
      TS_ASSERT(m_bbox.Inside(p_test)==false);
      }

    void test_BBox3D_TriangleInside()
      {
      Point3D_d p1(0.1,0.2,0.3);
      Point3D_d p2(0.1,1.2,0.3);
      Point3D_d p3(-1.0,-0.1,3.3);
      Triangle3D_d triangle(p1,p2,p3);

      TS_ASSERT(m_bbox.Inside(triangle));
      }

    void test_BBox3D_TriangleInsidePartially()
      {
      Point3D_d p1(10.1,0.2,0.3);
      Point3D_d p2(0.1,1.2,0.3);
      Point3D_d p3(-0.7,-0.1,3.3);
      Triangle3D_d triangle(p1,p2,p3);

      TS_ASSERT(m_bbox.Inside(triangle)==false);
      }

    void test_BBox3D_TriangleNotInside()
      {
      Point3D_d p1(10.1,0.2,0.3);
      Point3D_d p2(10.1,1.2,0.3);
      Point3D_d p3(10.7,-0.1,3.3);
      Triangle3D_d triangle(p1,p2,p3);

      TS_ASSERT(m_bbox.Inside(triangle)==false);
      }

    // Ray origins outside of the box.
    void test_BBox3D_RayIntersect1()
      {
      Point3D_d origin(-2.0,-2.0,-2.0);
      Vector3D_d direction=Vector3D_d(1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);

      double t0,t1;
      TS_ASSERT(m_bbox.Intersect(ray, &t0, &t1));
      TS_ASSERT_EQUALS(ray(t0), Point3D_d(-1.0,-1.0,-1.0));
      TS_ASSERT_EQUALS(ray(t1),Point3D_d(1.0,1.0,1.0));
      }

    // Ray origins inside the box.
    void test_BBox3D_RayIntersect2()
      {
      Point3D_d origin(0.0,0.0,0.0);
      Vector3D_d direction=Vector3D_d(1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);

      double t0,t1;
      TS_ASSERT(m_bbox.Intersect(ray, &t0, &t1));
      TS_ASSERT_EQUALS(ray(t0), origin);
      TS_ASSERT_EQUALS(ray(t1), Point3D_d(1.0,1.0,1.0));
      }

    // Ray origins in the vertex of the box and point outside.
    void test_BBox3D_RayIntersect3()
      {
      Point3D_d origin=m_bbox.m_max;
      Vector3D_d direction=Vector3D_d(1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);

      double t0,t1;
      TS_ASSERT(m_bbox.Intersect(ray, &t0, &t1));
      TS_ASSERT_EQUALS(ray(t0), origin);
      TS_ASSERT_EQUALS(ray(t1), origin);
      }

    // Ray only touches a vertex of the box but does not intersect the interior.
    void test_BBox3D_RayIntersect4()
      {
      Point3D_d origin=m_bbox.m_max+Point3D_d(0.5,-0.5,-0.5);
      Vector3D_d direction=Vector3D_d(-1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);

      double t0,t1;
      TS_ASSERT(m_bbox.Intersect(ray, &t0, &t1));
      TS_ASSERT_EQUALS(ray(t0), m_bbox.m_max);
      TS_ASSERT_EQUALS(ray(t1), m_bbox.m_max);
      }

    // Ray origins outside of the box and points in the opposite direction.
    void test_BBox3D_RayNotIntersect1()
      {
      Point3D_d origin(-2.0,-2.0,-2.0);
      Vector3D_d direction=Vector3D_d(-1.0,-1.0,-1.0).Normalized();
      Ray ray(origin, direction);

      TS_ASSERT(m_bbox.Intersect(ray)==false);
      }

    // Ray origins outside of the box, points towards the box but the m_min_t parameter is beyond the box.
    void test_BBox3D_RayNotIntersect2()
      {
      Point3D_d origin(-2.0,-2.0,-2.0);
      Vector3D_d direction=Vector3D_d(1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);
      ray.m_min_t=10.0;

      TS_ASSERT(m_bbox.Intersect(ray)==false);
      }

    // Ray origins outside of the box, points towards the box but the m_max_t parameter is before the box.
    void test_BBox3D_RayNotIntersect3()
      {
      Point3D_d origin(-2.0,-2.0,-2.0);
      Vector3D_d direction=Vector3D_d(1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);
      ray.m_max_t=0.5;

      TS_ASSERT(m_bbox.Intersect(ray)==false);
      }

    void test_BBox3D_UnionWithPoint1()
      {
      BBox3D_d bbox(Point3D_d(-1,-1,-1), Point3D_d(1,1,1));
      Point3D_d point(2,2,2);
      BBox3D_d united = Union(bbox, point);

      TS_ASSERT_EQUALS(united.m_min, Point3D_d(-1,-1,-1));
      TS_ASSERT_EQUALS(united.m_max, Point3D_d(2,2,2));
      }

    void test_BBox3D_UnionWithPoint2()
      {
      BBox3D_d bbox(Point3D_d(-1,-1,-1), Point3D_d(1,1,1));
      Point3D_d point(0.5,0.5,0.5);
      BBox3D_d united = Union(bbox, point);

      TS_ASSERT_EQUALS(united.m_min, Point3D_d(-1,-1,-1));
      TS_ASSERT_EQUALS(united.m_max, Point3D_d(1,1,1));
      }

    // Tests an important way of how the Union() method can be used to find a union of a set of points.
    // Since the bbox is initialized with infinity values it should collapse to a point with the first call.
    void test_BBox3D_UnionWithPoint_InfinityValues()
      {
      BBox3D_d bbox;
      Point3D_d point(0.5,0.5,0.5);
      BBox3D_d united = Union(bbox, point);

      TS_ASSERT_EQUALS(united.m_min, point);
      TS_ASSERT_EQUALS(united.m_max, point);
      }

    void test_BBox3D_UniteWithPoint()
      {
      BBox3D_d bbox(Point3D_d(-1,-1,-1), Point3D_d(1,1,1));
      Point3D_d point(2,2,2);
      bbox.Unite(point);

      TS_ASSERT_EQUALS(bbox.m_min, Point3D_d(-1,-1,-1));
      TS_ASSERT_EQUALS(bbox.m_max, Point3D_d(2,2,2));
      }

    void test_BBox3D_UnionWithBox1()
      {
      BBox3D_d bbox1(Point3D_d(-1,-1,-1), Point3D_d(1,1,1));
      BBox3D_d bbox2(Point3D_d(0,0,0), Point3D_d(2,2,2));
      BBox3D_d united = Union(bbox1, bbox2);

      TS_ASSERT_EQUALS(united.m_min, Point3D_d(-1,-1,-1));
      TS_ASSERT_EQUALS(united.m_max, Point3D_d(2,2,2));
      }

    void test_BBox3D_UnionWithBox2()
      {
      BBox3D_d bbox1(Point3D_d(-1,-1,-1), Point3D_d(2,2,2));
      BBox3D_d bbox2(Point3D_d(0,0,0), Point3D_d(1,1,1));
      BBox3D_d united = Union(bbox1, bbox2);

      TS_ASSERT_EQUALS(united.m_min, Point3D_d(-1,-1,-1));
      TS_ASSERT_EQUALS(united.m_max, Point3D_d(2,2,2));
      }

    // Tests an important way of how the Union() method can be used to find a union of a set of bounding boxes.
    // Since the bbox is initialized with infinity values it should collapse to a finite bounding box with the first call.
    void test_BBox3D_UnionWithBox_InfinityValues()
      {
      BBox3D_d bbox1;
      BBox3D_d bbox2(Point3D_d(0,0,0), Point3D_d(1,1,1));
      BBox3D_d united = Union(bbox1, bbox2);

      TS_ASSERT_EQUALS(united.m_min, bbox2.m_min);
      TS_ASSERT_EQUALS(united.m_max, bbox2.m_max);
      }

    void test_BBox3D_UniteWithBox()
      {
      BBox3D_d bbox1(Point3D_d(-1,-1,-1), Point3D_d(1,1,1));
      BBox3D_d bbox2(Point3D_d(0,0,0), Point3D_d(2,2,2));
      bbox1.Unite(bbox2);

      TS_ASSERT_EQUALS(bbox1.m_min, Point3D_d(-1,-1,-1));
      TS_ASSERT_EQUALS(bbox1.m_max, Point3D_d(2,2,2));
      }

    void test_BBox3D_Conversion()
      {
      BBox3D<int> bbox_int = Convert<int>(m_bbox);
      TS_ASSERT_EQUALS(bbox_int.m_min, Point3D<int>(-1,-2,-3));
      TS_ASSERT_EQUALS(bbox_int.m_max, Point3D<int>(1,2,3));
      }

    private:
      BBox3D_d m_bbox;
  };

#endif // BBOX3D_TEST_H