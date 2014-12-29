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

#ifndef PLANAR_MAPPING_2D_H
#define PLANAR_MAPPING_2D_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Mapping.h>

/**
* Maps DifferentialGeometry object to a 2D point by projecting it onto a 2D plane.
*/
class PlanarMapping2D : public Mapping2D
  {
  public:
    /**
    * Creates PlanarMapping2D instance.
    * @param i_u First axis of the 2D plane.
    * @param i_v Second axis of the 2D plane. Must be non-collinear to i_u.
    * @param i_u_offset Offset that is to be added to the u coordinate.
    * @param i_v_offset Offset that is to be added to the v coordinate.
    */
    PlanarMapping2D(const Vector3D_d &i_u, const Vector3D_d &i_v, double i_u_offset, double i_v_offset);

    /**
    * Maps DifferentialGeometry to a 2D point by projecting it onto a 2D plane.
    * @param i_dg DifferentialGeometry object describing the surface point.
    * @param i_triangle_index Mesh triangle index.
    * @param[out] o_point Mapped 2D point.
    * @param[out] o_dp_dx Mapped X screen-space differential.
    * @param[out] o_dp_dy Mapped Y screen-space differential.
    */
    virtual void Map(const DifferentialGeometry &i_dg, size_t i_triangle_index, Point2D_d &o_point, Vector2D_d &o_dp_dx, Vector2D_d &o_dp_dy) const;

  private:
    Vector3D_d m_u, m_v;
    double m_u_offset, m_v_offset;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline PlanarMapping2D::PlanarMapping2D(const Vector3D_d &i_u, const Vector3D_d &i_v, double i_u_offset, double i_v_offset) :
m_u(i_u), m_v(i_v), m_u_offset(i_u_offset), m_v_offset(i_v_offset)
  {
  ASSERT(fabs(i_u*i_v) > DBL_EPS);
  }

inline void PlanarMapping2D::Map(const DifferentialGeometry &i_dg, size_t i_triangle_index, Point2D_d &o_point, Vector2D_d &o_dp_dx, Vector2D_d &o_dp_dy) const
  {
  Vector3D_d point(i_dg.m_point);
  o_point = Point2D_d(point*m_u + m_u_offset, point*m_v + m_v_offset);

  Vector3D_d dp_dx(i_dg.m_point_dx-i_dg.m_point), dp_dy(i_dg.m_point_dy-i_dg.m_point);
  o_dp_dx = Vector2D_d(dp_dx * m_u, dp_dx * m_v);
  o_dp_dy = Vector2D_d(dp_dy * m_u, dp_dy * m_v);
  }

#endif // PLANAR_MAPPING_2D_H