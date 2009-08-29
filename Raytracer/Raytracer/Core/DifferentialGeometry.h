#ifndef DIFFERENTIAL_GEOMETRY_H
#define DIFFERENTIAL_GEOMETRY_H

#include <Common\Common.h>
#include <Math\Geometry.h>

class DifferentialGeometry
  {
  public:
    Point3D_d m_point;
    Vector3D_d m_geometric_normal, m_shading_normal;
    Point2D_d m_uv;

    Vector3D_d m_dn_du, m_dn_dv;
    Vector3D_d m_dp_dx, m_dp_dy;

    Vector2D_d m_duv_dx, m_duv_dy;
  };

#endif // DIFFERENTIAL_GEOMETRY_H