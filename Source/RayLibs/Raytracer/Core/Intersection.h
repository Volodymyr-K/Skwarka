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

#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <Common/Common.h>
#include "DifferentialGeometry.h"
#include "Primitive.h"

/**
* The structure holds the data describing the complete local surface properties at an intersection point.
* The data includes the pointer to the primitive, differential geometry object and the index of the intersected mesh triangle.
*/
struct Intersection
  {
  /**
  * Pointer to the intersected primitive.
  */
  const Primitive *mp_primitive;

  /**
  * Describes local surface geometry at the intersection point.
  */
  DifferentialGeometry m_dg;

  /**
  * Index of the intersected mesh triangle.
  */
  size_t m_triangle_index;

  /**
  * Specific parameter of ray-triangle intersection test.
  * This value is used to determine epsilon value for the outgoing rays to avoid self-intersection with the same triangle.
  */
  Vector3D_d m_cross;

  /**
  * Specific parameter of ray-triangle intersection test.
  * This value is used to determine epsilon value for the outgoing rays to avoid self-intersection with the same triangle.
  */
  double m_dot;
  };

#endif // INTERSECTION_H