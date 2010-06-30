#ifndef TRANSFORM_MAPPING_3D_TEST_H
#define TRANSFORM_MAPPING_3D_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Raytracer/Mappings/TransformMapping3D.h>
#include <Math/Transform.h>

class TransformMapping3DTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_TransformMapping3D_Map1()
      {
      intrusive_ptr<Mapping3D> mapping( new TransformMapping3D );

      DifferentialGeometry dg;
      dg.m_point = Point3D_d(10,1,1);
      dg.m_point_dx = dg.m_point+Point3D_d(0,1,0);
      dg.m_point_dy = dg.m_point+Point3D_d(0,1,-1);

      Point3D_d mapped;
      Vector3D_d dp_dx, dp_dy;
      mapping->Map(dg, 0, mapped, dp_dx, dp_dy);

      TS_ASSERT_EQUALS(mapped, dg.m_point);
      TS_ASSERT_EQUALS(dp_dx, Vector3D_d(dg.m_point_dx-dg.m_point));
      TS_ASSERT_EQUALS(dp_dy, Vector3D_d(dg.m_point_dy-dg.m_point));
      }

    void test_TransformMapping3D_Map2()
      {
      Transform t = MakeRotationX(0.5);
      intrusive_ptr<Mapping3D> mapping( new TransformMapping3D(t) );

      DifferentialGeometry dg;
      dg.m_point = Point3D_d(10,1,1);
      dg.m_point_dx = dg.m_point+Point3D_d(0,1,0);
      dg.m_point_dy = dg.m_point+Point3D_d(0,1,-1);

      Point3D_d mapped;
      Vector3D_d dp_dx, dp_dy;
      mapping->Map(dg, 0, mapped, dp_dx, dp_dy);

      TS_ASSERT_EQUALS(mapped, t(dg.m_point));
      TS_ASSERT_EQUALS(dp_dx, t(Vector3D_d(dg.m_point_dx-dg.m_point)));
      TS_ASSERT_EQUALS(dp_dy, t(Vector3D_d(dg.m_point_dy-dg.m_point)));
      }

  };

#endif // TRANSFORM_MAPPING_3D_TEST_H