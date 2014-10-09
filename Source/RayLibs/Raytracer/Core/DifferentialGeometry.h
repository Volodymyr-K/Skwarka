#ifndef DIFFERENTIAL_GEOMETRY_H
#define DIFFERENTIAL_GEOMETRY_H

#include <Common/Common.h>
#include <Math/Geometry.h>

/**
* The structure holds the data describing the local object's geometry at a surface point.
* The data includes the 3D point, normal direction and UV coordinates and derivatives.
* The structure also holds the surface points and shading normals corresponding to the next X and Y screen-space coordinates.
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
  * Shading normal of a surface. This is a normal that is used for shading and can (slightly) differ from the geometric normal.
  */
  Vector3D_d m_shading_normal;

  /**
  * UV coordinates at the point.
  */
  Point2D_d m_uv;

  /**
  * Surface point corresponding to the next X screen-space coordinate.
  */
  Point3D_d m_point_dx;

  /**
  * Surface point corresponding to the next Y screen-space coordinate.
  */
  Point3D_d m_point_dy;

  /**
  * Shading normal at a surface point corresponding to the next X screen-space coordinate.
  */
  Vector3D_d m_normal_dx;

  /**
  * Shading normal at a surface point corresponding to the next Y screen-space coordinate.
  */
  Vector3D_d m_normal_dy;

  /**
  * First derivative of UV coordinates at X screen-space coordinate.
  */
  Vector2D_d m_duv_dx;
  
  /**
  * First derivative of UV coordinates at Y screen-space coordinate.
  */
  Vector2D_d m_duv_dy;

  /**
  * Surface tangential vector. Used to define "base" direction for anisotropic material properties.
  */
  Vector3D_d m_tangent;
  };

#endif // DIFFERENTIAL_GEOMETRY_H