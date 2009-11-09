#ifndef UV_MAPPING_2D_TEST_H
#define UV_MAPPING_2D_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/Common.h>
#include <Raytracer/Mappings/UVMapping2D.h>

class UVMapping2DTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_UVMapping2D_Map1()
      {
      intrusive_ptr<Mapping2D> mapping( new UVMapping2D() );

      DifferentialGeometry dg;
      dg.m_uv = Point2D_d(1,2);
      dg.m_duv_dx = Vector2D_d(3,4);
      dg.m_duv_dy = Vector2D_d(5,6);

      Point2D_d mapped;
      Vector2D_d dp_dx, dp_dy;
      mapping->Map(dg, mapped, dp_dx, dp_dy);
      TS_ASSERT_EQUALS(mapped, dg.m_uv);
      TS_ASSERT_EQUALS(dp_dx, dg.m_duv_dx);
      TS_ASSERT_EQUALS(dp_dy, dg.m_duv_dy);
      }

    void test_UVMapping2D_Map2()
      {
      double u_scale=0.5, v_scale=2.0;
      Vector2D_d translation(1,-1);
      intrusive_ptr<Mapping2D> mapping( new UVMapping2D(u_scale,v_scale,translation) );

      DifferentialGeometry dg;
      dg.m_uv = Point2D_d(1,2);
      dg.m_duv_dx = Vector2D_d(3,4);
      dg.m_duv_dy = Vector2D_d(5,6);

      Point2D_d mapped;
      Vector2D_d dp_dx, dp_dy;
      mapping->Map(dg, mapped, dp_dx, dp_dy);
      TS_ASSERT_EQUALS(mapped, Point2D_d(dg.m_uv[0]*u_scale,dg.m_uv[1]*v_scale)+translation);
      TS_ASSERT_EQUALS(dp_dx, Vector2D_d(dg.m_duv_dx[0]*u_scale,dg.m_duv_dx[1]*v_scale)+translation);
      TS_ASSERT_EQUALS(dp_dy, Vector2D_d(dg.m_duv_dy[0]*u_scale,dg.m_duv_dy[1]*v_scale)+translation);
      }

  };

#endif // UV_MAPPING_2D_TEST_H