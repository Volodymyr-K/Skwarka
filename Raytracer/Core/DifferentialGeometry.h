#ifndef DIFFERENTIAL_GEOMETRY_H
#define DIFFERENTIAL_GEOMETRY_H

#include <Common\Common.h>
#include <Math\Geometry.h>

class DifferentialGeometry
  {
  public:
    Point3Dd m_point;
    Vector3Dd m_geometric_normal, m_shading_normal;
    double m_u, m_v;

    Vector3Dd m_dn_du, m_dn_dv;
    Vector3Dd m_dp_dx, m_dp_dy;

    double m_du_dx, m_dv_dx;
    double m_du_dy, m_dv_dy;
  };

#endif // DIFFERENTIAL_GEOMETRY_H