#ifndef RAY_TEST_H
#define RAY_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Ray.h>

class RayTestSuite : public CxxTest::TestSuite
  {
  public:
    void testRayDefaultConstr()
      {
      Ray r;
      TS_ASSERT_EQUALS(r.m_origin, Point3D_d());
      TS_ASSERT_EQUALS(r.m_direction, Vector3D_d());
      TS_ASSERT_EQUALS(r.m_min_t, 0.0);

      //This is a known bug in CxxTest that TS_ASSERT_EQUALS enters an infinite loop with infinite values.
      //So, TS_ASSERT is used instead.
      TS_ASSERT(r.m_max_t==DBL_INF);
      }

    void testRayConstrWithCoords()
      {
      Point3D_d origin(1.1,2.1,-3.1);
      Vector3D_d direction(-1.5,0.0,3.1);
      Ray r(origin, direction, -100.0, -10.0);

      TS_ASSERT_EQUALS(r.m_origin, origin);
      TS_ASSERT_EQUALS(r.m_direction, direction);
      TS_ASSERT_EQUALS(r.m_min_t, -100.0);
      TS_ASSERT_EQUALS(r.m_max_t, -10.0);
      }

    void testRayOperator()
      {
      Point3D_d origin(1.1,2.1,-3.1);
      Vector3D_d direction(-1.5,0.0,3.1);
      Ray r(origin, direction, 0.0, DBL_INF);
      Point3D_d point = r(2.0);

      TS_ASSERT_EQUALS(point, origin+direction*2.0);
      }

    void testRayDifferentialDefaultConstr()
      {
      RayDifferential rd;
      TS_ASSERT(rd.m_has_differentials==false);
      }

    void testRayAndRayDifferentialMutualConstructors()
      {
      Point3D_d origin(1.1,2.1,-3.1);
      Vector3D_d direction(-1.5,0.0,3.1);
      Ray r(origin, direction, -100.0, -10.0);

      // To RayDifferential and back to Ray.
      RayDifferential rd(r);
      Ray r2(rd);

      TS_ASSERT_EQUALS(r2.m_origin, origin);
      TS_ASSERT_EQUALS(r2.m_direction, direction);
      TS_ASSERT_EQUALS(r2.m_min_t, -100.0);
      TS_ASSERT_EQUALS(r2.m_max_t, -10.0);
      }
  };

#endif // RAY_TEST_H