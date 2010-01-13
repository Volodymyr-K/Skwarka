#include "Camera.h"

Camera::Camera(const Transform &i_camera2world, intrusive_ptr<Film> ip_film):
m_camera2world(i_camera2world), mp_film(ip_film)
  {
  ASSERT(ip_film!=NULL);
  }

intrusive_ptr<Film> Camera::GetFilm() const
  {
  return mp_film;
  }

void Camera::_TransformRay(const Ray &i_ray, Ray &o_ray) const
  {
  m_camera2world(i_ray,o_ray);
  }