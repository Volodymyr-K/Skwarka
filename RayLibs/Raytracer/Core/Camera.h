#ifndef CAMERA_H
#define CAMERA_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "Film.h"

/**
* An abstract class for camera placed in the scene.
* This is the abstract class, derived classes must implement GenerateRay() method.
* The camera holds an instance of Film class.
* @sa Film
*/
class Camera: public ReferenceCounted
  {
  public:
    /**
    * Generates ray based on the image point and lens UV coordinates.
    * Implementations that do not have a concept of a lens can ignore the second parameter.
    * @param i_image_point An image point.
    * @param i_lens_uv Lens UV coordinates in [0;1]x[0;1].
    * @param[out] o_ray Resulting ray in the world space. The direction component of the ray is normalized.
    * @return The weight of the ray. It corresponds to the value the ray brings to the resulting image.
    */
    virtual double GenerateRay(const Point2D_d &i_image_point, const Point2D_d &i_lens_uv, Ray &o_ray) const = 0;

    /**
    * Returns the transformation object that defines the transformation between the camera space and world space.
    */
    Transform GetCamera2WorldTransform() const;

    /**
    * Returns the film.
    */
    intrusive_ptr<Film> GetFilm() const;

    virtual ~Camera() {}

  protected:
    /**
    * Constructs an instance of a camera.
    * @param i_camera2world The transformation object that defines the transformation between the camera space and world space.
    * @param ip_film A smart pointer to the film the camera holds.
    */
    Camera(const Transform &i_camera2world, intrusive_ptr<Film> ip_film);

    /**
    * Helper method for derived classes that transform the ray in the camera space to the world space.
    */
    void _TransformRay(const Ray &i_ray, Ray &o_ray) const;

  private:
    // Not implemented, not a value type.
    Camera();
    Camera(const Camera&);
    Camera &operator=(const Camera&);

  private:
    Transform m_camera2world;
    intrusive_ptr<Film> mp_film;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Serializes Camera to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, Camera &i_camera, const unsigned int i_version)
  {
  /*
  Nothing to do here, everything must be serialized by the derived classes.

  We can't serialize the member fields here because there's no default constructor for the class
  and save_construct_data/load_construct_data functions can't be used either
  because it is impossible to create an instance of the abstract class.
  */

  // Just call the serialization for the base ReferenceCounted class.
  i_ar & boost::serialization::base_object<ReferenceCounted>(i_camera);
  }

#endif // CAMERA_H