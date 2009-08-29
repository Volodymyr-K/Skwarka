#ifndef PERSPECTIVE_CAMERA_H
#define PERSPECTIVE_CAMERA_H

#include <Common\Common.h>
#include <Math\Geometry.h>
#include <Raytracer\Core\Camera.h>
#include <Raytracer\Core\Film.h>

class PerspectiveCamera: public Camera
  {
  public:
    PerspectiveCamera(const Transform &i_camera2world, shared_ptr<Film> ip_film, const double &i_lens_radius, const double &i_focal_distance, const double &i_x_view_angle);

    double GenerateRay(const Point2D_d &i_image_point, const Point2D_d &i_lens_uv, Ray &o_ray) const;

  private:
    double m_lens_radius;
    double m_focal_distance;
    double m_x_view_angle;

    double m_x_tan, m_y_tan;
  };

#endif // PERSPECTIVE_CAMERA_H