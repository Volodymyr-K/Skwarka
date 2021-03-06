/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RAY_TEST_H
#define RAY_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Ray.h>

class RayTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Ray_DefaultConstr()
      {
      Ray r;
      TS_ASSERT_EQUALS(r.m_origin, Point3D_d());
      TS_ASSERT_EQUALS(r.m_direction, Vector3D_d());
      TS_ASSERT_EQUALS(r.m_min_t, 0.0);

      //This is a known bug in CxxTest that TS_ASSERT_EQUALS enters an infinite loop with infinite values.
      //So, TS_ASSERT is used instead.
      TS_ASSERT(r.m_max_t==DBL_INF);
      }

    void test_Ray_ConstrWithCoords()
      {
      Point3D_d origin(1.1,2.1,-3.1);
      Vector3D_d direction(-1.5,0.0,3.1);
      Ray r(origin, direction, -100.0, -10.0);

      TS_ASSERT_EQUALS(r.m_origin, origin);
      TS_ASSERT_EQUALS(r.m_direction, direction);
      TS_ASSERT_EQUALS(r.m_min_t, -100.0);
      TS_ASSERT_EQUALS(r.m_max_t, -10.0);
      }

    void test_Ray_Operator()
      {
      Point3D_d origin(1.1,2.1,-3.1);
      Vector3D_d direction(-1.5,0.0,3.1);
      Ray r(origin, direction, 0.0, DBL_INF);
      Point3D_d point = r(2.0);

      TS_ASSERT_EQUALS(point, origin+direction*2.0);
      }

    void test_Ray_DifferentialDefaultConstr()
      {
      RayDifferential rd;
      TS_ASSERT(rd.m_has_differentials == false);
      TS_ASSERT_EQUALS(rd.m_specular_depth, 0);
      }

    void test_Ray_RayDifferentialMutualConstructors()
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