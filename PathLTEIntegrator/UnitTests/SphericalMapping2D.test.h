#ifndef SPHERICAL_MAPPING_2D_TEST_H
#define SPHERICAL_MAPPING_2D_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/Common.h>
#include <Raytracer/Mappings/SphericalMapping2D.h>

class SphericalMapping2DTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_SphericalMapping2D_Map()
      {
      Point3D_d center(1,2,3);
      Vector3D_d z_axis(1,0,0), x_axis(0,1,0);
      intrusive_ptr<Mapping2D> mapping( new SphericalMapping2D(center, z_axis, x_axis) );

      DifferentialGeometry dg;
      dg.m_point = center+Point3D_d(10,1,1);
      dg.m_point_dx = center+Point3D_d(0,1,0);
      dg.m_point_dy = center+Point3D_d(0,1,-1);

      Point2D_d mapped;
      Vector2D_d dp_dx, dp_dy;
      mapping->Map(dg,  0,mapped, dp_dx, dp_dy);
      TS_ASSERT_DELTA(mapped[0], acos(10/sqrt(102.0))*INV_PI, (1e-10));
      TS_ASSERT_DELTA(mapped[1], 0.125, (1e-10));
      
      TS_ASSERT_DELTA(dp_dx[0], 0.5-mapped[0], (1e-10));
      TS_ASSERT_DELTA(dp_dx[1], 0.0-mapped[1], (1e-10));

      TS_ASSERT_DELTA(dp_dy[0], 0.5-mapped[0], (1e-10));
      TS_ASSERT_DELTA(dp_dy[1], -0.125-mapped[1], (1e-10));
      }

  };

#endif // SPHERICAL_MAPPING_2D_TEST_H