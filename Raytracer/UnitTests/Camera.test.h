#ifndef CAMERA_TEST_H
#define CAMERA_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Transform.h>
#include <Raytracer/Core/Camera.h>
#include <UnitTests/Mocks/CameraMock.h>
#include <UnitTests/Mocks/FilmFilterMock.h>
#include <Raytracer/Films/ImageFilm.h>

class CameraTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Camera_Constr()
      {
      Transform t = MakeLookAt(Point3D_d(1.0,2.0,3.0), Vector3D_d(-1.0,-0.5,0.5), Vector3D_d(0.0,0.0,1.0));
      shared_ptr<FilmFilter> p_filter(new FilmFilterMock(1.0,1.0));
      shared_ptr<Film> p_film(new ImageFilm(100,100,p_filter));
      CameraMock cam(t,p_film);

      TS_ASSERT(cam.GetFilm()==p_film);
      }
  };

#endif // CAMERA_TEST_H