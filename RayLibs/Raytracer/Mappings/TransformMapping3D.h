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

    Transform GetTransform() const;

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

inline Transform TransformMapping3D::GetTransform() const
  {
  return m_transform;
  }

inline void TransformMapping3D::Map(const DifferentialGeometry &i_dg, size_t i_triangle_index, Point3D_d &o_point, Vector3D_d &o_dp_dx, Vector3D_d &o_dp_dy) const
  {
  o_point = m_transform(i_dg.m_point);
  o_dp_dx = Vector3D_d(m_transform(i_dg.m_point_dx)-o_point);
  o_dp_dy = Vector3D_d(m_transform(i_dg.m_point_dy)-o_point);
  }

/**
* Saves the data which is needed to construct TransformMapping3D to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const TransformMapping3D *ip_mapping, const unsigned int i_version)
  {
  Transform transform = ip_mapping->GetTransform();

  i_ar << transform;
  }

/**
* Constructs TransformMapping3D with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, TransformMapping3D *ip_mapping, const unsigned int i_version)
  {
  Transform transform;

  i_ar >> transform;

  ::new(ip_mapping)TransformMapping3D(transform);
  }

/**
* Serializes TransformMapping3D to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, TransformMapping3D &i_mapping, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Mapping3D>(i_mapping);
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT(TransformMapping3D)

#endif // TRANSFORM_MAPPING_3D_H