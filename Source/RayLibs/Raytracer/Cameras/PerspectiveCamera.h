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

    /**
    * Generates ray based on the image point and lens UV coordinates.
    * @param i_image_point An image point.
    * @param i_lens_uv Lens UV coordinates in [0;1]x[0;1].
    * @param[out] o_ray Resulting ray in the world space. The direction component of the ray is normalized.
    * @return The weight of the ray. It corresponds to the value the ray brings to the resulting image.
    */
    double GenerateRay(const Point2D_d &i_image_point, const Point2D_d &i_lens_uv, Ray &o_ray) const;

    double GetLensRadius() const;

    double GetFocalDistance() const;

    double GetXViewAngle() const;

  private:
    double m_lens_radius;
    double m_focal_distance;
    double m_x_view_angle;

    double m_x_tan, m_y_tan;

    size_t m_film_x_resolution, m_film_y_resolution;
  };

#endif // PERSPECTIVE_CAMERA_H