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

#ifndef CAMERA_MOCK_H
#define CAMERA_MOCK_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/Core/Film.h>

/*
Camera mock implementation.
Always generates the same ray pointing along the camera direction.
*/
class CameraMock: public Camera
  {
  public:
    CameraMock(const Transform &i_camera2world, intrusive_ptr<Film> ip_film): Camera(i_camera2world, ip_film)
      {
      }

    double GenerateRay(const Point2D_d &i_image_point, const Point2D_d &i_lens_uv, Ray &o_ray) const
      {
      ASSERT(i_lens_uv[0]>=0.0 && i_lens_uv[0]<1.0 && i_lens_uv[1]>=0.0 && i_lens_uv[1]<1.0);

      o_ray.m_origin=Point3D_d();
      o_ray.m_direction=Vector3D_d(0.0,0.0,1.0);

      _TransformRay(o_ray, o_ray);
      return 1.0;
      }
  };

#endif // CAMERA_MOCK_H