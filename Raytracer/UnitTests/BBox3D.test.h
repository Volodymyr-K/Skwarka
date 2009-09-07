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

    void testBBoxDefaultConstr()
      {
      BBox3D_d b1;
      TS_ASSERT_EQUALS(b1.m_min, Point3D_d(0.0,0.0,0.0));
      TS_ASSERT_EQUALS(b1.m_max, Point3D_d(0.0,0.0,0.0));
      }

    void testBBoxDefaultConstrWithCorners()
      {
      // The min corner is intentionally greater than the max corner to test that BBox does not swap them.
      Point3D_d p_min(1.0,2.5,3.5);
      Point3D_d p_max(-1.0,-2.0,-3.0);
      BBox3D_d b1(p_min,p_max);
      TS_ASSERT_EQUALS(b1.m_min, p_min);
      TS_ASSERT_EQUALS(b1.m_max, p_max);
      }

    void testBBoxVolume()
      {
      double volume = m_bbox.Volume();
      TS_ASSERT_EQUALS(volume, 2.0*4.5*6.5);
      }

    void testBBoxNegativeVolume()
      {
      Point3D_d p_min(1.0,-2.0,-3.0);
      Point3D_d p_max(-1.0,2.5,3.5);
      BBox3D_d b1(p_min,p_max);

      double volume = b1.Volume();
      TS_ASSERT_EQUALS(volume, -2.0*4.5*6.5);
      }

    void testBBoxPointInsideCompletely()
      {
      Point3D_d p_test(0.1,0.2,0.3);
      TS_ASSERT(m_bbox.Inside(p_test));
      }

    void testBBoxPointInsideOnFace()
      {
      Point3D_d p_test(0.0,0.0,-3.0);
      TS_ASSERT(m_bbox.Inside(p_test));
      }

    void testBBoxPointInsideOnEdge()
      {
      Point3D_d p_test(0.0,-2.0,-3.0);
      TS_ASSERT(m_bbox.Inside(p_test));
      }

    void testBBoxPointInsideOnVertex()
      {
      Point3D_d p_test(-1.0,-2.0,3.5);
      TS_ASSERT(m_bbox.Inside(p_test));
      }

    void testBBoxPointNotInside()
      {
      Point3D_d p_test(10.1,0.2,0.3);
      TS_ASSERT(m_bbox.Inside(p_test)==false);
      }

    void testBBoxTriangleInside()
      {
      Point3D_d p1(0.1,0.2,0.3);
      Point3D_d p2(0.1,1.2,0.3);
      Point3D_d p3(-1.0,-0.1,3.3);
      Triangle3D_d triangle(p1,p2,p3);

      TS_ASSERT(m_bbox.Inside(triangle));
      }

    void testBBoxTriangleInsidePartially()
      {
      Point3D_d p1(10.1,0.2,0.3);
      Point3D_d p2(0.1,1.2,0.3);
      Point3D_d p3(-0.7,-0.1,3.3);
      Triangle3D_d triangle(p1,p2,p3);

      TS_ASSERT(m_bbox.Inside(triangle)==false);
      }

    void testBBoxTriangleNotInside()
      {
      Point3D_d p1(10.1,0.2,0.3);
      Point3D_d p2(10.1,1.2,0.3);
      Point3D_d p3(10.7,-0.1,3.3);
      Triangle3D_d triangle(p1,p2,p3);

      TS_ASSERT(m_bbox.Inside(triangle)==false);
      }

    // Ray origins outside of the box.
    void testBBoxRayIntersect1()
      {
      Point3D_d origin(-2.0,-2.0,-2.0);
      Vector3D_d direction=Vector3D_d(1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);

      TS_ASSERT(m_bbox.Intersect(ray));
      }

    // Ray origins inside the box.
    void testBBoxRayIntersect2()
      {
      Point3D_d origin(0.0,0.0,0.0);
      Vector3D_d direction=Vector3D_d(1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);

      TS_ASSERT(m_bbox.Intersect(ray));
      }

    // Ray origins in the vertex of the box and point outside.
    void testBBoxRayIntersect3()
      {
      Point3D_d origin=m_bbox.m_max;
      Vector3D_d direction=Vector3D_d(1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);

      TS_ASSERT(m_bbox.Intersect(ray));
      }

    // Ray only touches a vertex of the box but does not intersect the interior.
    void testBBoxRayIntersect4()
      {
      Point3D_d origin=m_bbox.m_max+Point3D_d(0.5,-0.5,-0.5);
      Vector3D_d direction=Vector3D_d(-1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);

      TS_ASSERT(m_bbox.Intersect(ray));
      }

    // Ray origins outside of the box and points in the opposite direction.
    void testBBoxRayNotIntersect1()
      {
      Point3D_d origin(-2.0,-2.0,-2.0);
      Vector3D_d direction=Vector3D_d(-1.0,-1.0,-1.0).Normalized();
      Ray ray(origin, direction);

      TS_ASSERT(m_bbox.Intersect(ray)==false);
      }

    // Ray origins outside of the box, points towards the box but the m_min_t parameter is beyond the box.
    void testBBoxRayNotIntersect2()
      {
      Point3D_d origin(-2.0,-2.0,-2.0);
      Vector3D_d direction=Vector3D_d(1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);
      ray.m_min_t=10.0;

      TS_ASSERT(m_bbox.Intersect(ray)==false);
      }

    // Ray origins outside of the box, points towards the box but the m_max_t parameter is before the box.
    void testBBoxRayNotIntersect3()
      {
      Point3D_d origin(-2.0,-2.0,-2.0);
      Vector3D_d direction=Vector3D_d(1.0,1.0,1.0).Normalized();
      Ray ray(origin, direction);
      ray.m_max_t=0.5;

      TS_ASSERT(m_bbox.Intersect(ray)==false);
      }

    private:
      BBox3D_d m_bbox;
  };

#endif // BBOX3D_TEST_H