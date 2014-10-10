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