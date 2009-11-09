#ifndef UV_MAPPING_2D_H
#define UV_MAPPING_2D_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Mapping.h>

/**
* Maps DifferentialGeometry object to a 2D point according to the UV coordinates of the mesh.
*/
class UVMapping2D: public Mapping2D
  {
  public:
    /**
    * Creates UVMapping2D instance.
    * Optionally, UV scale and translation factors can be specified.
    */
    UVMapping2D(double i_u_scale = 1.0, double i_v_scale = 1.0, const Vector2D_d &i_uv_translation = Vector2D_d(0,0));

    /**
    * Maps DifferentialGeometry to a 2D point according to the UV coordinates of the mesh.
    * The resulting UV coordinates can be scaled and translated according to the scale and translation factors specified in the class constructor.
    * @param i_dg DifferentialGeometry object describing the surface point.
    * @param[out] o_point Mapped 2D point.
    * @param[out] o_dp_dx Mapped X screen-space differential.
    * @param[out] o_dp_dy Mapped Y screen-space differential.
    */
    virtual void Map(const DifferentialGeometry &i_dg, Point2D_d &o_point, Vector2D_d &o_dp_dx, Vector2D_d &o_dp_dy) const;

  private:
    double m_u_scale, m_v_scale;
    Vector2D_d m_uv_translation;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline UVMapping2D::UVMapping2D(double i_u_scale, double i_v_scale, const Vector2D_d &i_uv_translation):
m_u_scale(i_u_scale), m_v_scale(i_v_scale), m_uv_translation(i_uv_translation)
  {
  }

inline void UVMapping2D::Map(const DifferentialGeometry &i_dg, Point2D_d &o_point, Vector2D_d &o_dp_dx, Vector2D_d &o_dp_dy) const
  {
  o_point = Point2D_d(i_dg.m_uv[0]*m_u_scale + m_uv_translation[0], i_dg.m_uv[1]*m_v_scale + m_uv_translation[1]);

  o_dp_dx = Vector2D_d(i_dg.m_duv_dx[0]*m_u_scale + m_uv_translation[0], i_dg.m_duv_dx[1]*m_v_scale + m_uv_translation[1]);
  o_dp_dy = Vector2D_d(i_dg.m_duv_dy[0]*m_u_scale + m_uv_translation[0], i_dg.m_duv_dy[1]*m_v_scale + m_uv_translation[1]);
  }

#endif // UV_MAPPING_2D_H