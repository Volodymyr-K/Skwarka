#ifndef TRANSFORM_MAPPING_3D_H
#define TRANSFORM_MAPPING_3D_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Transform.h>
#include <Raytracer/Core/Mapping.h>

/**
* Maps DifferentialGeometry object to a 3D point by applying a transformation to the 3D point and the differentials.
*/
class TransformMapping3D: public Mapping3D
  {
  public:
    /**
    * Creates TransformMapping3D instance with the specified transformation.
    */
    TransformMapping3D(const Transform &i_transform = Transform());

    /**
    * Maps DifferentialGeometry to a 3D point.
    * @param i_dg DifferentialGeometry object describing the surface point.
    * @param i_triangle_index Mesh triangle index.
    * @param[out] o_point Mapped 3D point.
    * @param[out] o_dp_dx Mapped X screen-space differential.
    * @param[out] o_dp_dy Mapped Y screen-space differential.
    */
    virtual void Map(const DifferentialGeometry &i_dg, size_t i_triangle_index, Point3D_d &o_point, Vector3D_d &o_dp_dx, Vector3D_d &o_dp_dy) const;

  private:
    Transform m_transform;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline TransformMapping3D::TransformMapping3D(const Transform &i_transform): m_transform(i_transform)
  {
  }

inline void TransformMapping3D::Map(const DifferentialGeometry &i_dg, size_t i_triangle_index, Point3D_d &o_point, Vector3D_d &o_dp_dx, Vector3D_d &o_dp_dy) const
  {
  o_point = m_transform(i_dg.m_point);
  o_dp_dx = Vector3D_d(m_transform(i_dg.m_point_dx)-o_point);
  o_dp_dy = Vector3D_d(m_transform(i_dg.m_point_dy)-o_point);
  }

#endif // TRANSFORM_MAPPING_3D_H