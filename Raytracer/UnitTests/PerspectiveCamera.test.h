#ifndef PERSPECTIVE_CAMERA_TEST_H
#define PERSPECTIVE_CAMERA_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Transform.h>
#include <Raytracer/Cameras/PerspectiveCamera.h>
#include <Math/ThreadSafeRandom.h>
#include "Mocks/FilmMock.h"

class PerspectiveCameraTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      m_origin = Point3D_d(1.0,0.0,0.0);
      m_direction = Vector3D_d(0.0,1.0,0.0);
      m_transformation = MakeLookAt(m_origin, m_direction, Vector3D_d(0.0,0.0,1.0));
      mp_film=intrusive_ptr<Film>(new FilmMock(100,50));

      m_x_view_angle = 2.0*M_PI_3;
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_PerspectiveCamera_RayDirectionNormalization()
      {
      PerspectiveCamera cam(m_transformation, mp_film, 0.0, 0.0, m_x_view_angle);

      double x_res = cam.GetFilm()->GetXResolution();
      double y_res = cam.GetFilm()->GetYResolution();

      Ray ray;
      cam.GenerateRay(Point2D_d(RandomDouble(x_res),RandomDouble(y_res)),Point2D_d(RandomDouble(1.0),RandomDouble(1.0)),ray);
      TS_ASSERT(ray.m_direction.IsNormalized());
      }

    void test_PerspectiveCamera_RayBounds()
      {
      PerspectiveCamera cam(m_transformation, mp_film, 0.0, 0.0, m_x_view_angle);

      double x_res = cam.GetFilm()->GetXResolution();
      double y_res = cam.GetFilm()->GetYResolution();

      Ray ray;
      ray.m_min_t=ray.m_max_t=-10.0;
      cam.GenerateRay(Point2D_d(RandomDouble(x_res),RandomDouble(y_res)),Point2D_d(RandomDouble(1.0),RandomDouble(1.0)),ray);
      TS_ASSERT(ray.m_direction.Length()>0.0 && IsPositiveInf(ray.m_max_t));
      }

    // Tests that center image point spawns the same direction as the "look at" direction and the origin of the ray is the camera's position.
    void test_PerspectiveCamera_PinHoleCameraTransformation()
      {
      PerspectiveCamera cam(m_transformation, mp_film, 0.0, 0.0, m_x_view_angle);
      double x_res = cam.GetFilm()->GetXResolution();
      double y_res = cam.GetFilm()->GetYResolution();

      Ray ray;
      cam.GenerateRay(Point2D_d(x_res/2.0,y_res/2.0),Point2D_d(0.0,0.0),ray);
      TS_ASSERT_EQUALS(ray.m_direction,m_direction);
      TS_ASSERT_EQUALS(ray(ray.m_min_t),m_origin);
      }

    // Tests the range of rays directions for pinhole camera (the camera with zero lens radius).
    void test_PerspectiveCamera_PinHoleCameraRange()
      {
      PerspectiveCamera cam(m_transformation, mp_film, 0.0, 0.0, m_x_view_angle);

      double x_res = cam.GetFilm()->GetXResolution();
      double y_res = cam.GetFilm()->GetYResolution();

      Ray ray1,ray2;

      // Test x view angle.
      cam.GenerateRay(Point2D_d(0.0,y_res/2.0),Point2D_d(0.0,0.0),ray1);
      cam.GenerateRay(Point2D_d(x_res,y_res/2.0),Point2D_d(0.0,0.0),ray2);
      
      double x_view_angle=acos(ray1.m_direction*ray2.m_direction);
      TS_ASSERT_DELTA(m_x_view_angle,x_view_angle,(1e-10));

      // Test y view angle.
      cam.GenerateRay(Point2D_d(x_res/2.0,0.0),Point2D_d(0.0,0.0),ray1);
      cam.GenerateRay(Point2D_d(x_res/2.0,y_res),Point2D_d(0.0,0.0),ray2);

      double y_view_angle=acos(ray1.m_direction*ray2.m_direction);
      double theoretical_y_view_angle=2.0*atan(tan(m_x_view_angle/2.0)*(y_res/x_res));
      TS_ASSERT_DELTA(theoretical_y_view_angle,y_view_angle,(1e-10));
      }

    // Test that a ray with a randomly generated lens UVs goes through the focal point.
    void test_PerspectiveCamera_DepthOfField()
      {
      double lens_radius = 1.0;
      double focal_distance = 10.0;
      PerspectiveCamera cam(m_transformation, mp_film, lens_radius, focal_distance, m_x_view_angle);

      double x_res = cam.GetFilm()->GetXResolution();
      double y_res = cam.GetFilm()->GetYResolution();

      Ray ray;
      cam.GenerateRay(Point2D_d(x_res/2.0,y_res/2.0),Point2D_d(RandomDouble(1.0),RandomDouble(1.0)),ray);
      
      Point3D_d focal_point = m_origin+m_direction*focal_distance;
      Vector3D_d direction = Vector3D_d(focal_point-ray.m_origin).Normalized();
      CustomAssertDelta(direction, ray.m_direction, (1e-10));
      }

  private:
    Point3D_d m_origin;
    Vector3D_d m_direction;
    Transform m_transformation;
    intrusive_ptr<Film> mp_film;

    double m_x_view_angle;
  };

#endif // PERSPECTIVE_CAMERA_TEST_H