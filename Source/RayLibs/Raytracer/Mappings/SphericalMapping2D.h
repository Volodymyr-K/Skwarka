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

#ifndef SPHERICAL_MAPPING_2D_H
#define SPHERICAL_MAPPING_2D_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Transform.h>
#include <Math/MathRoutines.h>
#include <Raytracer/Core/Mapping.h>

/**
* This mapping effectively wraps a sphere around an object and returns projected (theta, phi) coordinates as a resulting 2D point.
*/
class SphericalMapping2D: public Mapping2D
  {
  public:
    /**
    * Creates SphericalMapping2D instance with the specified sphere position and orientation.
    * @param i_sphere_center Center of the sphere.
    * @param i_sphere_z_axis Z axis of the sphere. Theta coordinates are computed by projecting a direction onto this axis.
    * @param i_sphere_x_axis X axis of the sphere. Defines the "zero point" for phi coordinates. Should not be collinear to the z axis.
    */
    SphericalMapping2D(const Point3D_d &i_sphere_center, Vector3D_d i_sphere_z_axis, Vector3D_d i_sphere_x_axis);

    /**
    * Creates SphericalMapping2D instance with the specified world-to-sphere transformation.
    * @param i_world_to_sphere World-to-sphere transformation.
    */
    SphericalMapping2D(const Transform &i_world_to_sphere);

    /**
    * Maps 3D point defined by the specified DifferentialGeometry object by projecting it onto the sphere and returning (theta,phi) coordinates.
    * The method does not account for the singularity near the poles and so the differentials may be inaccurate if they are near the sphere poles.
    * @param i_dg DifferentialGeometry object describing the surface point.
    * @param i_triangle_index Mesh triangle index.
    * @param[out] o_point Mapped 2D point.
    * @param[out] o_dp_dx Mapped X screen-space differential.
    * @param[out] o_dp_dy Mapped Y screen-space differential.
    */
    virtual void Map(const DifferentialGeometry &i_dg, size_t i_triangle_index, Point2D_d &o_point, Vector2D_d &o_dp_dx, Vector2D_d &o_dp_dy) const;

  private:
    SphericalMapping2D() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    /**
    * Helper private method that maps specified 3D point to (theta,phi) coordinates.
    */
    void _MapPoint(const Point3D_d &i_point, Point2D_d &o_spherical_uv) const;

  private:
    Transform m_world_to_sphere;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline SphericalMapping2D::SphericalMapping2D(const Point3D_d &i_sphere_center, Vector3D_d i_sphere_z_axis, Vector3D_d i_sphere_x_axis)
  {
  i_sphere_z_axis.Normalize();
  i_sphere_x_axis.Normalize();
  Vector3D_d sphere_y_axis = (i_sphere_z_axis^i_sphere_x_axis).Normalized();
  i_sphere_x_axis = (sphere_y_axis^i_sphere_z_axis).Normalized();

  ASSERT(i_sphere_z_axis.IsNormalized() && sphere_y_axis.IsNormalized() && i_sphere_x_axis.IsNormalized());

  Transform translation = MakeTranslation(Vector3D_d(i_sphere_center)*(-1.0));
  Transform z_rotation = MakeMatchDirections(i_sphere_z_axis, Vector3D_d(0,0,1));
  z_rotation(i_sphere_x_axis,i_sphere_x_axis);
  Transform x_rotation = MakeMatchDirections(i_sphere_x_axis, Vector3D_d(1,0,0));

  m_world_to_sphere = x_rotation * z_rotation * translation;
  }

inline SphericalMapping2D::SphericalMapping2D(const Transform &i_world_to_sphere): m_world_to_sphere(i_world_to_sphere)
  {
  }

inline void SphericalMapping2D::Map(const DifferentialGeometry &i_dg, size_t i_triangle_index, Point2D_d &o_point, Vector2D_d &o_dp_dx, Vector2D_d &o_dp_dy) const
  {
  _MapPoint(i_dg.m_point, o_point);

  // Compute texture coordinate differentials for sphere uv mapping.
  Point2D_d point_dx, point_dy;

  _MapPoint(i_dg.m_point_dx, point_dx);
  o_dp_dx = Vector2D_d(point_dx-o_point);
  if (o_dp_dx[1] > 0.5) o_dp_dx[1] = o_dp_dx[1]-1.0;
  else
    if (o_dp_dx[1] < -0.5) o_dp_dx[1] = o_dp_dx[1] + 1.0;

  _MapPoint(i_dg.m_point_dy, point_dy);
  o_dp_dy = Vector2D_d(point_dy-o_point);
  if (o_dp_dy[1] > 0.5) o_dp_dy[1] = o_dp_dy[1] - 1.0;
  else
    if (o_dp_dy[1] < -0.5) o_dp_dy[1] = o_dp_dy[1] + 1.0;
  }

inline void SphericalMapping2D::_MapPoint(const Point3D_d &i_point, Point2D_d &o_spherical_uv) const
  {
  Vector3D_d direction = Vector3D_d(m_world_to_sphere(i_point));
  direction.Normalize();

  double theta = MathRoutines::SphericalTheta(direction);
  double phi = MathRoutines::SphericalPhi(direction);

  o_spherical_uv[0] = theta*INV_PI;
  o_spherical_uv[1] = phi*INV_2PI;
  }

template<class Archive>
void SphericalMapping2D::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Mapping2D>(*this);
  i_ar & m_world_to_sphere;
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_KEY(SphericalMapping2D)

#endif // SPHERICAL_MAPPING_2D_H