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

#ifndef PLANAR_MAPPING_2D_TEST_H
#define PLANAR_MAPPING_2D_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Raytracer/Mappings/PlanarMapping2D.h>

class PlanarMapping2DTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_PlanarMapping2D_Map()
      {
      Vector3D_d u(1, 0.5, 0.3), v(0, 1, 1.5);
      double u_offset = 0.5, v_offset = 0.6;
      intrusive_ptr<Mapping2D> p_mapping(new PlanarMapping2D(u, v, u_offset, v_offset));

      DifferentialGeometry dg;
      dg.m_point = Point3D_d(10, 1, 1);

      Vector3D_d dx = Vector3D_d(1, 1, 0.5), dy = Vector3D_d(1, 1, 0.5);
      dg.m_point_dx = dg.m_point+dx;
      dg.m_point_dy = dg.m_point+dy;

      Point2D_d mapped;
      Vector2D_d dp_dx, dp_dy;
      p_mapping->Map(dg, 0, mapped, dp_dx, dp_dy);

      TS_ASSERT_DELTA(mapped[0], Vector3D_d(dg.m_point) * u + u_offset, DBL_EPS);
      TS_ASSERT_DELTA(mapped[1], Vector3D_d(dg.m_point) * v + v_offset, DBL_EPS);

      TS_ASSERT_DELTA(dp_dx[0], dx*u, DBL_EPS);
      TS_ASSERT_DELTA(dp_dx[1], dx*v, DBL_EPS);

      TS_ASSERT_DELTA(dp_dy[0], dy*u, DBL_EPS);
      TS_ASSERT_DELTA(dp_dy[1], dy*v, DBL_EPS);
      }

  };

#endif // PLANAR_MAPPING_2D_TEST_H