#ifndef CAMERA_H
#define CAMERA_H

#include <Common\Common.h>
#include <Math\Geometry.h>
#include "Film.h"

class Camera
  {
  public:
    Camera(const Transform &i_camera2world, shared_ptr<Film> ip_film);

    virtual double GenerateRay(const Point2D_d &i_image_point, const Point2D_d &i_lens_uv, Ray &o_ray) const = 0;

    shared_ptr<Film> GetFilm() const;

    virtual ~Camera() {}

  protected:
    void _TransformRay(Ray &io_ray) const;

  private:
    Transform m_camera2world;
    shared_ptr<Film> mp_film;
  };

#endif // CAMERA_H