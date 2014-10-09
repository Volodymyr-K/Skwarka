#ifndef CORE_UTILS_TEST_H
#define CORE_UTILS_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/CoreUtils.h>

class CoreUtilsTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_CoreUtils_SetReflectedDifferentials_NoDifferentials()
      {
      RayDifferential incident( Ray(Point3D_d(0,0,0), Vector3D_d(1,2,3).Normalized()) );
      incident.m_has_differentials = false;

      double t = 10.0;
      DifferentialGeometry dg;
      dg.m_point = incident.m_base_ray(t);
      dg.m_geometric_normal = dg.m_shading_normal = Vector3D_d(0,0,-1);

      dg.m_point_dx = incident.m_origin_dx + incident.m_direction_dx*t;
      dg.m_normal_dx = Vector3D_d(0.1,0,-1).Normalized();

      dg.m_point_dy = incident.m_origin_dy + incident.m_direction_dy*t;
      dg.m_normal_dy = Vector3D_d(0,0.1,-1).Normalized();

      RayDifferential exitant;
      CoreUtils::SetReflectedDifferentials(incident, dg, exitant);

      TS_ASSERT(exitant.m_has_differentials == false);
      }

    void test_CoreUtils_SetReflectedDifferentials()
      {
      RayDifferential incident( Ray(Point3D_d(0,0,0), Vector3D_d(1,2,3).Normalized()) );

      incident.m_origin_dx = Point3D_d(1,0,0);
      incident.m_direction_dx = Vector3D_d(2,2,3).Normalized();

      incident.m_origin_dy = Point3D_d(0,1,0);
      incident.m_direction_dy = Vector3D_d(1,3,3).Normalized();

      incident.m_has_differentials = true;

      double t = 10.0;
      DifferentialGeometry dg;
      dg.m_point = incident.m_base_ray(t);
      dg.m_geometric_normal = dg.m_shading_normal = Vector3D_d(0,0,-1);

      dg.m_point_dx = incident.m_origin_dx + incident.m_direction_dx*t;
      dg.m_normal_dx = Vector3D_d(0.1,0,-1).Normalized();

      dg.m_point_dy = incident.m_origin_dy + incident.m_direction_dy*t;
      dg.m_normal_dy = Vector3D_d(0,0.1,-1).Normalized();

      RayDifferential exitant;
      CoreUtils::SetReflectedDifferentials(incident, dg, exitant);

      TS_ASSERT(exitant.m_has_differentials);
      TS_ASSERT_EQUALS(exitant.m_origin_dx, dg.m_point_dx);
      TS_ASSERT_EQUALS(exitant.m_origin_dy, dg.m_point_dy);

      CustomAssertDelta( (incident.m_direction_dx*(-1.0)+exitant.m_direction_dx).Normalized(), dg.m_normal_dx, 1e-5);
      CustomAssertDelta( (incident.m_direction_dy*(-1.0)+exitant.m_direction_dy).Normalized(), dg.m_normal_dy, 1e-5);
      }

    void test_CoreUtils_SetTransmittedDifferentials_NoDifferentials()
      {
      RayDifferential incident( Ray(Point3D_d(0,0,0), Vector3D_d(1,2,3).Normalized()) );
      incident.m_has_differentials = false;

      double t = 10.0;
      DifferentialGeometry dg;
      dg.m_point = incident.m_base_ray(t);
      dg.m_geometric_normal = dg.m_shading_normal = Vector3D_d(0,0,-1);

      dg.m_point_dx = incident.m_origin_dx + incident.m_direction_dx*t;
      dg.m_normal_dx = Vector3D_d(0.1,0,-1).Normalized();

      dg.m_point_dy = incident.m_origin_dy + incident.m_direction_dy*t;
      dg.m_normal_dy = Vector3D_d(0,0.1,-1).Normalized();

      RayDifferential exitant;
      CoreUtils::SetTransmittedDifferentials(incident, dg, 1.4, exitant);

      TS_ASSERT(exitant.m_has_differentials == false);
      }

    void test_CoreUtils_SetTransmittedDifferentials_TotalInternalReflection()
      {
      RayDifferential incident( Ray(Point3D_d(0,0,0), Vector3D_d(0,0,1).Normalized()) );

      incident.m_origin_dx = Point3D_d(1,0,0);
      incident.m_direction_dx = Vector3D_d(0.1,0,1).Normalized();

      incident.m_origin_dy = Point3D_d(0,1,0);
      incident.m_direction_dy = Vector3D_d(0,0.1,1).Normalized();

      incident.m_has_differentials = true;

      double t = 10.0;
      DifferentialGeometry dg;
      dg.m_point = incident.m_base_ray(t);
      dg.m_geometric_normal = dg.m_shading_normal = Vector3D_d(-2,-2,1).Normalized();

      dg.m_point_dx = incident.m_origin_dx + incident.m_direction_dx*t;
      dg.m_normal_dx = Vector3D_d(-2.1,-2,1).Normalized();

      dg.m_point_dy = incident.m_origin_dy + incident.m_direction_dy*t;
      dg.m_normal_dy = Vector3D_d(-2,-2.1,1).Normalized();

      RayDifferential exitant;
      CoreUtils::SetTransmittedDifferentials(incident, dg, 1.4, exitant);

      TS_ASSERT(exitant.m_has_differentials == false);
      }

    void test_CoreUtils_SetTransmittedDifferentials()
      {
      RayDifferential incident( Ray(Point3D_d(0,0,0), Vector3D_d(1,2,3).Normalized()) );

      incident.m_origin_dx = Point3D_d(1,0,0);
      incident.m_direction_dx = Vector3D_d(2,2,3).Normalized();

      incident.m_origin_dy = Point3D_d(0,1,0);
      incident.m_direction_dy = Vector3D_d(1,3,3).Normalized();

      incident.m_has_differentials = true;

      double t = 10.0;
      DifferentialGeometry dg;
      dg.m_point = incident.m_base_ray(t);
      dg.m_geometric_normal = dg.m_shading_normal = Vector3D_d(0,0,1);

      dg.m_point_dx = incident.m_origin_dx + incident.m_direction_dx*t;
      dg.m_normal_dx = Vector3D_d(0.1,0,1).Normalized();

      dg.m_point_dy = incident.m_origin_dy + incident.m_direction_dy*t;
      dg.m_normal_dy = Vector3D_d(0,0.1,1).Normalized();

      RayDifferential exitant;
      CoreUtils::SetTransmittedDifferentials(incident, dg, 1.4, exitant);

      TS_ASSERT(exitant.m_has_differentials);
      TS_ASSERT_EQUALS(exitant.m_origin_dx, dg.m_point_dx);
      TS_ASSERT_EQUALS(exitant.m_origin_dy, dg.m_point_dy);

      TS_ASSERT(exitant.m_direction_dx.IsNormalized());
      TS_ASSERT(exitant.m_direction_dy.IsNormalized());

      double cs_exitant_dx=fabs(exitant.m_direction_dx*dg.m_normal_dx);
      double cs_incident_dx=fabs(incident.m_direction_dx*dg.m_normal_dx);
      TS_ASSERT_DELTA( sqrt(1.0-cs_exitant_dx*cs_exitant_dx), sqrt(1.0-cs_incident_dx*cs_incident_dx)*1.4, 1e-5);

      double cs_exitant_dy=fabs(exitant.m_direction_dy*dg.m_normal_dy);
      double cs_incident_dy=fabs(incident.m_direction_dy*dg.m_normal_dy);
      TS_ASSERT_DELTA( sqrt(1.0-cs_exitant_dy*cs_exitant_dy), sqrt(1.0-cs_incident_dy*cs_incident_dy)*1.4, 1e-5);
      }
  };

#endif // CORE_UTILS_TEST_H