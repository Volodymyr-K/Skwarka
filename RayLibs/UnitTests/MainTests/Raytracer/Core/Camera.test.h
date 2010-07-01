#ifndef CAMERA_TEST_H
#define CAMERA_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Transform.h>
#include <Raytracer/Core/Camera.h>
#include "Mocks/CameraMock.h"
#include "Mocks/FilmMock.h"

class CameraTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Camera_Constr()
      {
      Transform t = MakeLookAt(Point3D_d(1.0,2.0,3.0), Vector3D_d(-1.0,-0.5,0.5), Vector3D_d(0.0,0.0,1.0));;
      intrusive_ptr<Film> p_film(new FilmMock(100,100));
      CameraMock cam(t,p_film);

      Point3D_d test_point(1,2,3);
      TS_ASSERT_EQUALS(cam.GetCamera2WorldTransform()(test_point), t(test_point));

      TS_ASSERT_EQUALS(cam.GetFilm(), p_film);
      }
  };

#endif // CAMERA_TEST_H