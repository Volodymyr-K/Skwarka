#ifndef PERSPECTIVE_CAMERA_H
#define PERSPECTIVE_CAMERA_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/Core/Film.h>

/**
* Perspective camera implementation.
* The class implements the Camera interface for perspective projection.
* The camera also supports "depth of field" for blurring the out of focus objects.
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
    PerspectiveCamera(const Transform &i_camera2world, intrusive_ptr<Film> ip_film, const double &i_lens_radius, const double &i_focal_distance, const double &i_x_view_angle);

    double GetLensRadius() const;

    double GetFocalDistance() const;

    double GetXViewAngle() const;

    /**
    * Generates ray based on the image point and lens UV coordinates.
    * @param i_image_point An image point.
    * @param i_lens_uv Lens UV coordinates in [0;1]^2.
    * @param[out] o_ray Resulting ray in the world space. The direction component of the ray is normalized.
    * @return The weight of the ray. It corresponds to the value the ray brings to the resulting image.
    */
    double GenerateRay(const Point2D_d &i_image_point, const Point2D_d &i_lens_uv, Ray &o_ray) const;

  private:
    double m_lens_radius;
    double m_focal_distance;
    double m_x_view_angle;

    double m_x_tan, m_y_tan;

    size_t m_film_x_resolution, m_film_y_resolution;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Saves the data which is needed to construct PerspectiveCamera to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const PerspectiveCamera *ip_camera, const unsigned int i_version)
  {
  Transform camera2world = ip_camera->GetCamera2WorldTransform();
  intrusive_ptr<Film> p_film = ip_camera->GetFilm();
  double lens_radius = ip_camera->GetLensRadius();
  double focal_distance = ip_camera->GetFocalDistance();
  double x_view_angle = ip_camera->GetXViewAngle();

  i_ar << camera2world;
  i_ar << p_film;
  i_ar << lens_radius;
  i_ar << focal_distance;
  i_ar << x_view_angle;
  }

/**
* Constructs PerspectiveCamera with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, PerspectiveCamera *ip_camera, const unsigned int i_version)
  {
  Transform camera2world;
  intrusive_ptr<Film> p_film;
  double lens_radius, focal_distance, x_view_angle;

  i_ar >> camera2world;
  i_ar >> p_film;
  i_ar >> lens_radius;
  i_ar >> focal_distance;
  i_ar >> x_view_angle;
  ::new(ip_camera)PerspectiveCamera(camera2world, p_film, lens_radius, focal_distance, x_view_angle);
  }

/**
* Serializes PerspectiveCamera to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, PerspectiveCamera &i_camera, const unsigned int version)
  {
  i_ar & boost::serialization::base_object<Camera>(i_camera);
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT(PerspectiveCamera)

#endif // PERSPECTIVE_CAMERA_H