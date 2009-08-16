#include "Camera.h"

Camera::Camera(const Transform &i_camera2world, shared_ptr<Film> ip_film):
m_camera2world(i_camera2world), mp_film(ip_film)
  {
  }

shared_ptr<Film> Camera::GetFilm() const
  {
  return mp_film;
  }

void Camera::TransformRay(Ray &io_ray) const
  {
  m_camera2world(io_ray,io_ray);
  }

PerspectiveCamera::PerspectiveCamera(const Transform &i_camera2world, shared_ptr<Film> ip_film, const double &i_lens_radius, const double &i_focal_distance, const double &i_x_view_angle):
  Camera(i_camera2world, ip_film), m_lens_radius(i_lens_radius), m_focal_distance(i_focal_distance), m_x_view_angle(i_x_view_angle)
  {
  if (m_x_view_angle<0.0)
    {
    Log::Warning("Negative angle passed to perspective camera. Taking absolute value.");
    m_x_view_angle=-m_x_view_angle;
    }

  m_x_tan=tan(m_x_view_angle/2.0);
  m_y_tan=m_x_tan*((double)GetFilm()->GetYResolution())/GetFilm()->GetXResolution();
  }

double PerspectiveCamera::GenerateRay(const Point2D_d &i_image_point, const Point2D_d &i_lens_point, Ray &o_ray) const
  {
  o_ray.m_origin=Point3D_d();
  
  double x = m_x_tan * (i_image_point[0]-(double)GetFilm()->GetXResolution()*0.5)/(double)GetFilm()->GetXResolution();
  double y = m_y_tan * (i_image_point[1]-(double)GetFilm()->GetYResolution()*0.5)/(double)GetFilm()->GetYResolution();
  o_ray.m_direction=Vector3D_d(x,y,1.0);
  o_ray.m_direction.Normalize();

  TransformRay(o_ray);
  return 1.0;
  }