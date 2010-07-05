#ifndef SPHERICAL_MAPPING_2D_H
#define SPHERICAL_MAPPING_2D_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Transform.h>
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

    Point3D_d GetSphereCenter() const;

    Vector3D_d GetSphereZAxis() const;

    Vector3D_d GetSphereXAxis() const;

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
    /**
    * Helper private method that maps specified 3D point to (theta,phi) coordinates.
    */
    void _MapPoint(const Point3D_d &i_point, Point2D_d &o_spherical_uv) const;

  private:
    Transform m_world_to_sphere;

    // These values are not actually used for transformation but we still store them
    // so that application can get the values if needed.
    Point3D_d m_sphere_center;
    Vector3D_d m_sphere_z_axis, m_sphere_x_axis;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline SphericalMapping2D::SphericalMapping2D(const Point3D_d &i_sphere_center, Vector3D_d i_sphere_z_axis, Vector3D_d i_sphere_x_axis)
  {
  i_sphere_z_axis.Normalize();
  i_sphere_x_axis.Normalize();
  Vector3D_d sphere_y_axis = (i_sphere_z_axis^i_sphere_x_axis).Normalized();
  i_sphere_x_axis = (sphere_y_axis^i_sphere_z_axis).Normalized();

  m_sphere_center=i_sphere_center;
  m_sphere_z_axis=i_sphere_z_axis;
  m_sphere_x_axis=i_sphere_x_axis;

  ASSERT(i_sphere_z_axis.IsNormalized() && sphere_y_axis.IsNormalized() && i_sphere_x_axis.IsNormalized());

  Transform translation = MakeTranslation(Vector3D_d(i_sphere_center)*(-1.0));
  Transform z_rotation = MakeMatchDirections(i_sphere_z_axis, Vector3D_d(0,0,1));
  z_rotation(i_sphere_x_axis,i_sphere_x_axis);
  Transform x_rotation = MakeMatchDirections(i_sphere_x_axis, Vector3D_d(1,0,0));

  m_world_to_sphere = x_rotation * z_rotation * translation;
  }

inline Point3D_d SphericalMapping2D::GetSphereCenter() const
  {
  return m_sphere_center;
  }

inline Vector3D_d SphericalMapping2D::GetSphereZAxis() const
  {
  return m_sphere_z_axis;
  }

inline Vector3D_d SphericalMapping2D::GetSphereXAxis() const
  {
  return m_sphere_x_axis;
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

/**
* Saves the data which is needed to construct SphericalMapping2D to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const SphericalMapping2D *ip_mapping, const unsigned int i_version)
  {
  Point3D_d sphere_center = ip_mapping->GetSphereCenter();
  Vector3D_d sphere_z_axis = ip_mapping->GetSphereZAxis();
  Vector3D_d sphere_x_axis = ip_mapping->GetSphereXAxis();

  i_ar << sphere_center;
  i_ar << sphere_z_axis;
  i_ar << sphere_x_axis;
  }

/**
* Constructs SphericalMapping2D with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, SphericalMapping2D *ip_mapping, const unsigned int i_version)
  {
  Point3D_d sphere_center;
  Vector3D_d sphere_z_axis, sphere_x_axis;

  i_ar >> sphere_center;
  i_ar >> sphere_z_axis;
  i_ar >> sphere_x_axis;

  ::new(ip_mapping)SphericalMapping2D(sphere_center, sphere_z_axis, sphere_x_axis);
  }

/**
* Serializes SphericalMapping2D to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, SphericalMapping2D &i_mapping, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Mapping2D>(i_mapping);
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT(SphericalMapping2D)

#endif // SPHERICAL_MAPPING_2D_H