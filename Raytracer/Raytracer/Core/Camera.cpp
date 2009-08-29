#include "Camera.h"

Camera::Camera(const Transform &i_camera2world, shared_ptr<Film> ip_film):
m_camera2world(i_camera2world), mp_film(ip_film)
  {
  }

shared_ptr<Film> Camera::GetFilm() const
  {
  return mp_film;
  }

void Camera::_TransformRay(Ray &io_ray) const
  {
  m_camera2world(io_ray,io_ray);
  }