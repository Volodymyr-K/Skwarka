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

#include "Camera.h"

Camera::Camera(const Transform &i_camera2world, intrusive_ptr<Film> ip_film):
m_camera2world(i_camera2world), mp_film(ip_film)
  {
  ASSERT(ip_film!=NULL);
  }

Transform Camera::GetCamera2WorldTransform() const
  {
  return m_camera2world;
  }

intrusive_ptr<Film> Camera::GetFilm() const
  {
  return mp_film;
  }

void Camera::_TransformRay(const Ray &i_ray, Ray &o_ray) const
  {
  m_camera2world(i_ray,o_ray);
  }