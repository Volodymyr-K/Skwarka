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

#include "Primitive.h"

void Primitive::_Bump(const DifferentialGeometry &i_dg, size_t i_triangle_index, DifferentialGeometry &o_bumped_dg) const
  {
  ASSERT(mp_bump_map);

  DifferentialGeometry dg_eval = i_dg;
  Vector3D_d dp_dx(i_dg.m_point_dx-i_dg.m_point);
  Vector3D_d dp_dy(i_dg.m_point_dy-i_dg.m_point);

  // shift in x screen direction
  dg_eval.m_point_dx = i_dg.m_point_dx+dp_dx;
  dg_eval.m_point_dy = i_dg.m_point_dy+dp_dx;
  dg_eval.m_point = i_dg.m_point_dx;
  dg_eval.m_uv = i_dg.m_uv+i_dg.m_duv_dx;
  Point3D_d displaced_x = dg_eval.m_point + i_dg.m_shading_normal*mp_bump_map->Evaluate(dg_eval, i_triangle_index);

  // shift in y screen direction
  dg_eval.m_point_dx = i_dg.m_point_dx+dp_dy;
  dg_eval.m_point_dy = i_dg.m_point_dy+dp_dy;
  dg_eval.m_point = i_dg.m_point_dy;
  dg_eval.m_uv = i_dg.m_uv+i_dg.m_duv_dy;
  Point3D_d displaced_y = dg_eval.m_point + i_dg.m_shading_normal*mp_bump_map->Evaluate(dg_eval, i_triangle_index);

  Point3D_d displace_original = i_dg.m_point + i_dg.m_shading_normal*mp_bump_map->Evaluate(i_dg, i_triangle_index);

  o_bumped_dg = i_dg;
  o_bumped_dg.m_shading_normal = Vector3D_d(displaced_x-displace_original)^Vector3D_d(displaced_y-displace_original);
  o_bumped_dg.m_tangent = (o_bumped_dg.m_shading_normal^i_dg.m_tangent)^o_bumped_dg.m_shading_normal;

  if (o_bumped_dg.m_shading_normal.Normalize()==false || o_bumped_dg.m_tangent.Normalize()==false)
    {
    o_bumped_dg.m_shading_normal = i_dg.m_shading_normal;
    o_bumped_dg.m_tangent = i_dg.m_tangent;
    return;
    }

  // Orient shading normal to match the geometric normal (the tangent vector does not need to be adjusted).
  if (i_dg.m_geometric_normal*o_bumped_dg.m_shading_normal < 0.0)
    o_bumped_dg.m_shading_normal *= -1.0;
  }
