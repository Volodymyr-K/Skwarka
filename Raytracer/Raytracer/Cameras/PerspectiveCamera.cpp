#include "PerspectiveCamera.h"
#include <Math/SamplingRoutines.h>

PerspectiveCamera::PerspectiveCamera(const Transform &i_camera2world, intrusive_ptr<Film> ip_film, const double &i_lens_radius, const double &i_focal_distance, const double &i_x_view_angle):
Camera(i_camera2world, ip_film), m_lens_radius(i_lens_radius), m_focal_distance(i_focal_distance), m_x_view_angle(i_x_view_angle)
  {
  ASSERT(m_x_view_angle>=0.0);
  if (m_x_view_angle<0.0)
    m_x_view_angle=-m_x_view_angle;

  ASSERT(m_lens_radius>=0.0);
  if (m_lens_radius<0.0)
    m_lens_radius=0.0;

  m_x_tan=tan(m_x_view_angle/2.0);
  m_y_tan=m_x_tan*((double)GetFilm()->GetYResolution())/GetFilm()->GetXResolution();
  }

double PerspectiveCamera::GenerateRay(const Point2D_d &i_image_point, const Point2D_d &i_lens_uv, Ray &o_ray) const
  {
  ASSERT(i_lens_uv[0]>=0.0 && i_lens_uv[0]<1.0 && i_lens_uv[1]>=0.0 && i_lens_uv[1]<1.0);

  o_ray.m_origin=Point3D_d();

  double x = 2.0 * m_x_tan * (i_image_point[0]-(double)GetFilm()->GetXResolution()*0.5)/(double)GetFilm()->GetXResolution();
  double y = 2.0 * m_y_tan * (i_image_point[1]-(double)GetFilm()->GetYResolution()*0.5)/(double)GetFilm()->GetYResolution();

  o_ray.m_direction=Vector3D_d(x,y,1.0);
  if (m_lens_radius > 0.0)
    {
    // Sample point on the lens.
    Point2D_d lens_point;
    lens_point = SamplingRoutines::ConcentricDiskSampling(i_lens_uv);
    lens_point *= m_lens_radius;
    // Compute point on plane of focus
    Point3D_d focus_point = o_ray(m_focal_distance);
    // Update ray for effect of lens
    o_ray.m_origin=Point3D_d(lens_point[0], lens_point[1], 0.0);
    o_ray.m_direction = Vector3D_d(focus_point - o_ray.m_origin);
    }

  o_ray.m_direction.Normalize();

  _TransformRay(o_ray, o_ray);
  return 1.0;
  }