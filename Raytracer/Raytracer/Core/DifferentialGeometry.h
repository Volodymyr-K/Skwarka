#ifndef DIFFERENTIAL_GEOMETRY_H
#define DIFFERENTIAL_GEOMETRY_H

#include <Common/Common.h>
#include <Math/Geometry.h>

/**
* The structure holds the data describing the local object's geometry at a surface point.
* The data includes the 3D point, normal direction, UV coordinates and various first derivatives.
*/
struct DifferentialGeometry
  {
  /**
  * 3D point on an object's surface.
  */
  Point3D_d m_point;

  /**
  * Geometric normal of a surface. This is the "true" normal of the mesh's triangle.
  */
  Vector3D_d m_geometric_normal;

  /**
  * A shading normal of a surface. This is a normal that is used for shading and can (slightly) differ from the geometric normal.
  */
  Vector3D_d m_shading_normal;

  /**
  * UV coordinates at the point.
  */
  Point2D_d m_uv;

  /**
  * First derivative of shading normal at U coordinate.
  */
  Vector3D_d m_dn_du;

  /**
  * First derivative of shading normal at V coordinate.
  */
  Vector3D_d m_dn_dv;

  /**
  * First derivative of point at U screen-space X coordinate.
  */
  Vector3D_d m_dp_dx;
  
  /**
  * First derivative of point at U screen-space Y coordinate.
  */
  Vector3D_d m_dp_dy;

  /**
  * First derivative of UV coordinates at U screen-space X coordinate.
  */
  Vector2D_d m_duv_dx;
  
  /**
  * First derivative of UV coordinates at U screen-space Y coordinate.
  */
  Vector2D_d m_duv_dy;
  };

#endif // DIFFERENTIAL_GEOMETRY_H