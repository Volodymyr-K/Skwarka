#include "Primitive.h"

void Primitive::_Bump(const DifferentialGeometry &i_dg, size_t i_triangle_index, DifferentialGeometry &o_bumped_dg) const
  {
  ASSERT(mp_bump_map);

  if (i_dg.m_dp_dx==Vector3D_d() || i_dg.m_dp_dy==Vector3D_d())
    {
    o_bumped_dg = i_dg;
    return;
    }

  DifferentialGeometry dg_eval = i_dg;

  dg_eval.m_point = i_dg.m_point+i_dg.m_dp_dx;
  dg_eval.m_uv = i_dg.m_uv+i_dg.m_duv_dx;
  Point3D_d displaced_x = dg_eval.m_point + dg_eval.m_geometric_normal*mp_bump_map->Evaluate(dg_eval, i_triangle_index);

  dg_eval.m_point = i_dg.m_point+i_dg.m_dp_dy;
  dg_eval.m_uv = i_dg.m_uv+i_dg.m_duv_dy;
  Point3D_d displaced_y = dg_eval.m_point + dg_eval.m_geometric_normal*mp_bump_map->Evaluate(dg_eval, i_triangle_index);

  Point3D_d displace_original = i_dg.m_point + i_dg.m_geometric_normal*mp_bump_map->Evaluate(i_dg, i_triangle_index);

  o_bumped_dg = i_dg;
  o_bumped_dg.m_shading_normal = Vector3D_d(displaced_x-displace_original)^Vector3D_d(displaced_y-displace_original);
  o_bumped_dg.m_shading_normal.Normalize();

  // Orient shading normal to match the geometric normal.
  if (i_dg.m_geometric_normal*o_bumped_dg.m_shading_normal < 0.0)
    o_bumped_dg.m_shading_normal *= -1.0;

  o_bumped_dg.m_shading_normal += i_dg.m_shading_normal-i_dg.m_geometric_normal;
  o_bumped_dg.m_shading_normal.Normalize();
  }
