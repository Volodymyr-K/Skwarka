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