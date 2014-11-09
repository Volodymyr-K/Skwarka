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

#ifndef MAPPING_H
#define MAPPING_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "DifferentialGeometry.h"

/**
* An abstract base class defining the contract for mapping surface point described by a DifferentialGeometry object to a 2D point.
* @sa Mapping3D
*/
class Mapping2D : public ReferenceCounted
  {
  public:
    /**
    * Maps DifferentialGeometry to a 2D point.
    * @param i_dg DifferentialGeometry object describing the surface point.
    * @param i_triangle_index Mesh triangle index.
    * @param[out] o_point Mapped 2D point.
    * @param[out] o_dp_dx Mapped X screen-space differential.
    * @param[out] o_dp_dy Mapped Y screen-space differential.
    */
    virtual void Map(const DifferentialGeometry &i_dg, size_t i_triangle_index, Point2D_d &o_point, Vector2D_d &o_dp_dx, Vector2D_d &o_dp_dy) const = 0;

    virtual ~Mapping2D();

  protected:
    Mapping2D();

  private:
    // Not implemented, not a value type.
    Mapping2D(const Mapping2D&);
    Mapping2D &operator=(const Mapping2D&);
  };

/**
* An abstract base class defining the contract for mapping surface point described by a DifferentialGeometry object to a 3D point.
* @sa Mapping2D
*/
class Mapping3D : public ReferenceCounted
  {
  public:
    /**
    * Maps DifferentialGeometry to a 3D point.
    * @param i_dg DifferentialGeometry object describing the surface point.
    * @param i_triangle_index Mesh triangle index.
    * @param[out] o_point Mapped 3D point.
    * @param[out] o_dp_dx Mapped X screen-space differential.
    * @param[out] o_dp_dy Mapped Y screen-space differential.
    */
    virtual void Map(const DifferentialGeometry &i_dg, size_t i_triangle_index, Point3D_d &o_point, Vector3D_d &o_dp_dx, Vector3D_d &o_dp_dy) const = 0;

    virtual ~Mapping3D();

  protected:
    Mapping3D();

  private:
    // Not implemented, not a value type.
    Mapping3D(const Mapping3D&);
    Mapping3D &operator=(const Mapping3D&);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Mapping2D::Mapping2D()
  {
  }

inline Mapping2D::~Mapping2D()
  {
  }

inline Mapping3D::Mapping3D()
  {
  }

inline Mapping3D::~Mapping3D()
  {
  }

#endif // MAPPING_H