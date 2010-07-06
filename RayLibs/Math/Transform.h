#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <Common/Common.h>
#include "Matrix.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "Ray.h"
#include "BBox3D.h"

/**
* Describes transformation from one 3D coordinate space to another.
* The transformation is defined by a 4x4 matrix.
* @sa Matrix4x4
*/
class Transform
  {
  public:
    /**
    * Creates Transform instance that describes an identity transformation.
    */
    Transform();

    Transform(double i_values[4][4]);

    Transform(const Matrix4x4_d &i_matrix);

    /**
    * Returns inverted transformation.
    */
    Transform Inverted() const;

    /**
    * Applies transformation to the given point.
    */
    template<typename T>
    Point3D<T> operator()(const Point3D<T> &i_point) const;

    /**
    * Applies transformation to the given point and saves the result in the output parameter.
    */
    template<typename T>
    void operator()(const Point3D<T> &i_point, Point3D<T> &o_transformed_point) const;

    /**
    * Applies transformation to the given vector.
    */
    template<typename T>
    Vector3D<T> operator()(const Vector3D<T> &i_vector) const;

    /**
    * Applies transformation to the given vector and saves the result in the output parameter.
    */
    template<typename T>
    void operator()(const Vector3D<T> &i_vector, Vector3D<T> &o_transformed_vector) const;

    /**
    * Applies transformation to the given ray.
    */
    Ray operator()(const Ray &i_ray) const;

    /**
    * Applies transformation to the given ray and saves the result in the output parameter.
    */
    void operator()(const Ray &i_ray, Ray &o_transformed_ray) const;

    /**
    * Composes the transformation with the input transformation.
    */
    Transform operator*(const Transform &i_transform) const;

  private:
    /**
    * Private constructor that takes the matrix and its inverse.
    * It is called by the Inverted() method.
    * The constructor is private because the class needs to keep the invariant that the inverted matrix is indeed an inverse of the transformation matrix.
    */
    Transform(const Matrix4x4_d &i_matrix, const Matrix4x4_d &i_inverted_matrix);

  private:
    // Needed for the boost serialization framework.
    friend class boost::serialization::access;

    /**
    * Serializes Transform to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<typename Archive>
    void serialize(Archive & ar, const unsigned int i_version);

  private:
    Matrix4x4_d m_matrix, m_inverted_matrix;
  };

/**
* Creates Transform object that describes translation by the specified vector.
*/
Transform MakeTranslation(const Vector3D_d &i_translation);

/**
* Creates Transform object that describes scaling by the specified values.
*/
Transform MakeScale(const double &i_x_scale, const double &i_y_scale, const double &i_z_scale);

/**
* Creates Transform object that describes rotation around axis X by the specified angle.
* @param i_angle Angle value in radians.
*/
Transform MakeRotationX(const double &i_angle);

/**
* Creates Transform object that describes rotation around axis Y by the specified angle.
* @param i_angle Angle value in radians.
*/
Transform MakeRotationY(const double &i_angle);

/**
* Creates Transform object that describes rotation around axis Z by the specified angle.
* @param i_angle Angle value in radians.
*/
Transform MakeRotationZ(const double &i_angle);

/**
* Creates Transform object that describes rotation around the specified axis by the specified angle.
* @param i_angle Angle value in radians.
* @param i_axis Axis of rotation.
*/
Transform MakeRotation(const double &i_angle, Vector3D_d i_axis);

/**
* Creates Transform object that describes rotation that matches i_source direction to i_target direction.
* Specified vectors should be normalized and should not be collinear.
* @param i_source Direction to be rotated to match i_target. Should be normalized.
* @param i_target Direction the i_source should be rotated to. Should be normalized.
*/
Transform MakeMatchDirections(const Vector3D_d &i_source, const Vector3D_d &i_target);

/**
* Creates Transform object that describes transformation from "look-at" space to the world space.
* The "look-at" space is a coordinate system defined by the position of a camera, a viewing direction
* and an "up" vector that orients the camera along the viewing direction implied by the first two parameters.
* @param i_origin Position of the camera in the world space.
* @param i_direction Viewing direction of the camera in the world space. The function normalizes the direction, so the length does not matter.
* @param i_up "Up" vector in the world space.
*/
Transform MakeLookAt(const Point3D_d &i_origin, Vector3D_d i_direction, const Vector3D_d &i_up);

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Transform::Transform()
  {
  m_matrix = m_inverted_matrix = Matrix4x4_d(true);
  }

inline Transform::Transform(double i_values[4][4]): m_matrix(i_values)
  {
  m_matrix.Inverted(m_inverted_matrix);
  }

inline Transform::Transform(const Matrix4x4_d &i_matrix): m_matrix(i_matrix)
  {
  m_matrix.Inverted(m_inverted_matrix);
  }

inline Transform::Transform(const Matrix4x4_d &i_matrix, const Matrix4x4_d &i_inverted_matrix): m_matrix(i_matrix), m_inverted_matrix(i_inverted_matrix)
  {
  }

inline Transform Transform::Inverted() const
  {
  return Transform(m_inverted_matrix, m_matrix);
  }

template<typename T>
Point3D<T> Transform::operator()(const Point3D<T> &i_point) const
  {
  T x = i_point[0], y = i_point[1], z = i_point[2];
  T xp = m_matrix.m_values[0][0]*x + m_matrix.m_values[0][1]*y + m_matrix.m_values[0][2]*z + m_matrix.m_values[0][3];
  T yp = m_matrix.m_values[1][0]*x + m_matrix.m_values[1][1]*y + m_matrix.m_values[1][2]*z + m_matrix.m_values[1][3];
  T zp = m_matrix.m_values[2][0]*x + m_matrix.m_values[2][1]*y + m_matrix.m_values[2][2]*z + m_matrix.m_values[2][3];
  T wp = m_matrix.m_values[3][0]*x + m_matrix.m_values[3][1]*y + m_matrix.m_values[3][2]*z + m_matrix.m_values[3][3];

  ASSERT(wp != (T)0.0);
  if (wp == (T)1.0)
    return Point3D<T>(xp, yp, zp);
  else
    return Point3D<T>(xp, yp, zp)/wp;
  }

template<typename T>
void Transform::operator()(const Point3D<T> &i_point, Point3D<T> &o_transformed_point) const
  {
  T x = i_point[0], y = i_point[1], z = i_point[2];
  o_transformed_point[0] = m_matrix.m_values[0][0]*x + m_matrix.m_values[0][1]*y + m_matrix.m_values[0][2]*z + m_matrix.m_values[0][3];
  o_transformed_point[1] = m_matrix.m_values[1][0]*x + m_matrix.m_values[1][1]*y + m_matrix.m_values[1][2]*z + m_matrix.m_values[1][3];
  o_transformed_point[2] = m_matrix.m_values[2][0]*x + m_matrix.m_values[2][1]*y + m_matrix.m_values[2][2]*z + m_matrix.m_values[2][3];
  T w = m_matrix.m_values[3][0]*x + m_matrix.m_values[3][1]*y + m_matrix.m_values[3][2]*z + m_matrix.m_values[3][3];

  ASSERT(w != (T)0.0);
  if (w != (T)1.0)
    o_transformed_point /= w;
  }

template<typename T>
Vector3D<T> Transform::operator()(const Vector3D<T> &i_vector) const
  {
  T x = i_vector[0], y = i_vector[1], z = i_vector[2];
  return Vector3D<T>(
    m_matrix.m_values[0][0]*x + m_matrix.m_values[0][1]*y + m_matrix.m_values[0][2]*z,
    m_matrix.m_values[1][0]*x + m_matrix.m_values[1][1]*y + m_matrix.m_values[1][2]*z,
    m_matrix.m_values[2][0]*x + m_matrix.m_values[2][1]*y + m_matrix.m_values[2][2]*z);
  }

template<typename T>
void Transform::operator()(const Vector3D<T> &i_vector, Vector3D<T> &o_transformed_vector) const
  {
  T x = i_vector[0], y = i_vector[1], z = i_vector[2];
  o_transformed_vector[0] = m_matrix.m_values[0][0]*x + m_matrix.m_values[0][1]*y + m_matrix.m_values[0][2]*z;
  o_transformed_vector[1] = m_matrix.m_values[1][0]*x + m_matrix.m_values[1][1]*y + m_matrix.m_values[1][2]*z;
  o_transformed_vector[2] = m_matrix.m_values[2][0]*x + m_matrix.m_values[2][1]*y + m_matrix.m_values[2][2]*z;
  }

inline Ray Transform::operator()(const Ray &i_ray) const
  {
  Ray ret;
  (*this)(i_ray.m_origin, ret.m_origin);
  (*this)(i_ray.m_direction, ret.m_direction);
  ret.m_min_t = i_ray.m_min_t;
  ret.m_max_t = i_ray.m_max_t;
  return ret;
  }

inline void Transform::operator()(const Ray &i_ray, Ray &o_transformed_ray) const
  {
  (*this)(i_ray.m_origin, o_transformed_ray.m_origin);
  (*this)(i_ray.m_direction, o_transformed_ray.m_direction);
  o_transformed_ray.m_min_t = i_ray.m_min_t;
  o_transformed_ray.m_max_t = i_ray.m_max_t;
  }

inline Transform Transform::operator*(const Transform &i_transform) const
  {
  Matrix4x4_d m1 = m_matrix*i_transform.m_matrix;
  Matrix4x4_d m2 = i_transform.m_inverted_matrix*m_inverted_matrix;
  return Transform(m1, m2);
  }

template<typename Archive>
void Transform::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & m_matrix;
  i_ar & m_inverted_matrix;
  }

// Don't store class info for Transform.
BOOST_CLASS_IMPLEMENTATION(Transform, boost::serialization::object_serializable)

#endif // TRANSFORM_H