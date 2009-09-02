#ifndef PERSPECTIVE_CAMERA_H
#define PERSPECTIVE_CAMERA_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/Core/Film.h>

/**
* Perspective camera implementation.
* The class implements the Camera interface for perspective projection. Also, the camera supports "depth of field" for blurring the out of focus objects.
*/
class PerspectiveCamera: public Camera
  {
  public:
    /**
    * Constructs an instance of PerspectiveCamera.
    * @param i_camera2world The transformation object that defines the transformation between the camera space and world space.
    * @param ip_film A smart pointer to the film the camera holds.
    * @param i_lens_radius Radius of the camera lens. Bigger radius means bigger blurriness of out of focus object. Zero radius means no blurriness.
    * @param i_focal_distance Distance from the camera at which objects appear in focus.
    * @param i_x_view_angle Angle value in radians of the camera's field of view along the x direction in the image plane.
    */
    PerspectiveCamera(const Transform &i_camera2world, shared_ptr<Film> ip_film, const double &i_lens_radius, const double &i_focal_distance, const double &i_x_view_angle);

    /**
    * Generates ray based on the image point and lens UV coordinates.
    * @param i_image_point An image point.
    * @param i_lens_uv Lens UV coordinates in [0;1]^2.
    * @param[out] o_ray Resulting ray in the world space.
    * @return The weight of the ray. It corresponds to the value the ray brings to the resulting image.
    */
    double GenerateRay(const Point2D_d &i_image_point, const Point2D_d &i_lens_uv, Ray &o_ray) const;

  private:
    double m_lens_radius;
    double m_focal_distance;
    double m_x_view_angle;

    double m_x_tan, m_y_tan;
  };

#endif // PERSPECTIVE_CAMERA_H