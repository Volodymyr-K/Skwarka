#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Matrix.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "Ray.h"
#include "BBox3D.h"

class Transform
  {
  public:
    Transform();

    Transform(double i_values[4][4]);

    Transform(const Matrix4x4_d &i_matrix);

    Transform Inverted() const;

    template<typename T>
    Point3D<T> operator()(const Point3D<T> &i_point) const;

    template<typename T>
    void operator()(const Point3D<T> &i_point, Point3D<T> &o_transformed_point) const;

    template<typename T>
    Vector3D<T> operator()(const Vector3D<T> &i_vector) const;

    template<typename T>
    void operator()(const Vector3D<T> &i_vector, Vector3D<T> &o_transformed_vector) const;

    Ray operator()(const Ray &i_ray) const;
    void operator()(const Ray &i_ray, Ray &o_transformed_ray) const;

    Transform operator*(const Transform &i_transform) const;

  private:
    Transform(const Matrix4x4_d &i_matrix, const Matrix4x4_d &i_inverted_matrix);

  private:
    Matrix4x4_d m_matrix, m_inverted_matrix;
  };

Transform MakeTranslation(const Vector3D_d &i_translation);
Transform MakeScale(const double &i_x_scale, const double &i_y_scale, const double &i_z_scale);
Transform MakeRotationX(const double &i_angle);
Transform MakeRotationY(const double &i_angle);
Transform MakeRotationZ(const double &i_angle);
Transform MakeRotation(const double &i_angle, Vector3D_d i_axis);
Transform MakeLookAt(const Point3D_d &i_origin, Vector3D_d i_direction, const Vector3D_d &i_up);

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Transform::Transform()
  {
  m_matrix = m_inverted_matrix = Matrix4x4_d();
  }

inline Transform::Transform(double i_values[4][4]): m_matrix(i_values)
  {
  m_inverted_matrix = m_matrix.Inverted();
  }

inline Transform::Transform(const Matrix4x4_d &i_matrix): m_matrix(i_matrix)
  {
  m_inverted_matrix = m_matrix.Inverted();
  }

inline Transform::Transform(const Matrix4x4_d &i_matrix, const Matrix4x4_d &i_inverted_matrix): m_matrix(i_matrix), m_inverted_matrix(m_matrix)
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
  ret.m_mint = i_ray.m_mint;
  ret.m_maxt = i_ray.m_maxt;
  return ret;
  }

inline void Transform::operator()(const Ray &i_ray, Ray &o_transformed_ray) const
  {
  (*this)(i_ray.m_origin, o_transformed_ray.m_origin);
  (*this)(i_ray.m_direction, o_transformed_ray.m_direction);
  o_transformed_ray.m_mint = i_ray.m_mint;
  o_transformed_ray.m_maxt = i_ray.m_maxt;
  }

inline Transform Transform::operator*(const Transform &i_transform) const
  {
  Matrix4x4_d m1 = m_matrix*i_transform.m_matrix;
  Matrix4x4_d m2 = i_transform.m_inverted_matrix*m_inverted_matrix;
  return Transform(m1, m2);
  }

#endif // TRANSFORM_H