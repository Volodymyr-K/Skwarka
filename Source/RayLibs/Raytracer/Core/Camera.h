/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

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

    Camera() {} // Empty default constructor for the boost serialization framework.

    /**
    * Helper method for derived classes that transform the ray in the camera space to the world space.
    */
    void _TransformRay(const Ray &i_ray, Ray &o_ray) const;

  private:
    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    // Not implemented, not a value type.
    Camera(const Camera&);
    Camera &operator=(const Camera&);

  private:
    Transform m_camera2world;
    intrusive_ptr<Film> mp_film;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void Camera::serialize(Archive &i_ar,  const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<ReferenceCounted>(*this);
  i_ar & m_camera2world;
  i_ar & mp_film;
  }

#endif // CAMERA_H