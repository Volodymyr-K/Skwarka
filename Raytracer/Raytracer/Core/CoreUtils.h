#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#include <Math/Geometry.h>
#include "DifferentialGeometry.h"

/**
* This namespace contains helper routines for the raytracer core.
*/
namespace CoreUtils
  {

  /**
  * Sets ray differentials for the specularly reflected ray.
  * The method does not change the base ray of the o_exitant_ray.
  * @param i_incident_ray Incident ray. The direction component should be normalized.
  * @param i_dg Describes intersection point.
  * @param[out] o_exitant_ray The ray whose differential components will be set.
  */
  void SetReflectedDifferentials(const RayDifferential &i_incident_ray, const DifferentialGeometry &i_dg, RayDifferential &o_exitant_ray);

  /**
  * Sets ray differentials for the specularly transmitted ray.
  * The method does not change the base ray of the o_exitant_ray.
  * In case of total internal reflection the exitant ray won't have the differentials (the m_has_differentials field will be false).
  * @param i_incident_ray Incident ray. The direction component should be normalized.
  * @param i_dg Describes intersection point.
  * @param i_refractive_index Refractive index of the object (the object is always considered to be "below" the surface normal).
  * @param[out] o_exitant_ray The ray whose differential components will be set.
  */
  void SetTransmittedDifferentials(const RayDifferential &i_incident_ray, const DifferentialGeometry &i_dg, double i_refractive_index, RayDifferential &o_exitant_ray);

  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace CoreUtils
  {

  inline void SetReflectedDifferentials(const RayDifferential &i_incident_ray, const DifferentialGeometry &i_dg, RayDifferential &o_exitant_ray)
    {
    ASSERT(i_incident_ray.m_base_ray.m_direction.IsNormalized());
    o_exitant_ray.m_has_differentials = false;

    if (i_incident_ray.m_has_differentials==false)
      return;

    ASSERT(i_incident_ray.m_direction_dx.IsNormalized());
    ASSERT(i_incident_ray.m_direction_dy.IsNormalized());

    o_exitant_ray.m_origin_dx = i_dg.m_point_dx;
    o_exitant_ray.m_direction_dx = i_incident_ray.m_direction_dx - (2.0*(i_incident_ray.m_direction_dx*i_dg.m_normal_dx)) * i_dg.m_normal_dx;
    ASSERT(o_exitant_ray.m_direction_dx.IsNormalized());

    o_exitant_ray.m_origin_dy = i_dg.m_point_dy;
    o_exitant_ray.m_direction_dy = i_incident_ray.m_direction_dy - (2.0*(i_incident_ray.m_direction_dy*i_dg.m_normal_dy)) * i_dg.m_normal_dy;
    ASSERT(o_exitant_ray.m_direction_dy.IsNormalized());

    o_exitant_ray.m_has_differentials = true;
    }

  inline void SetTransmittedDifferentials(const RayDifferential &i_incident_ray, const DifferentialGeometry &i_dg, double i_refractive_index, RayDifferential &o_exitant_ray)
    {
    ASSERT(i_incident_ray.m_base_ray.m_direction.IsNormalized());
    ASSERT(i_refractive_index > 0.0);
    o_exitant_ray.m_has_differentials = false;

    if (i_incident_ray.m_has_differentials==false)
      return;

    ASSERT(i_incident_ray.m_direction_dx.IsNormalized());
    ASSERT(i_incident_ray.m_direction_dy.IsNormalized());

    double Z_sign = 1.0;
    double eta = i_refractive_index;
    if (i_incident_ray.m_base_ray.m_direction * i_dg.m_shading_normal < 0.0)
      {
      eta = 1.0 / i_refractive_index;
      Z_sign = -1.0;
      }

    double cos_theta_incident_dx = -(i_incident_ray.m_direction_dx * i_dg.m_normal_dx);
    double sin_theta_incident_sqr_dx = 1.0-cos_theta_incident_dx*cos_theta_incident_dx;
    double sin_theta_exitant_sqr_dx = eta*eta*sin_theta_incident_sqr_dx;

    double cos_theta_incident_dy = -(i_incident_ray.m_direction_dy * i_dg.m_normal_dy);
    double sin_theta_incident_sqr_dy = 1.0-cos_theta_incident_dy*cos_theta_incident_dy;
    double sin_theta_exitant_sqr_dy = eta*eta*sin_theta_incident_sqr_dy;

    // If not total internal reflection.
    if (sin_theta_exitant_sqr_dx < 1.0 && sin_theta_exitant_sqr_dy < 1.0)
      {
      double cos_theta_exitant_dx = Z_sign*sqrt(1.0-sin_theta_exitant_sqr_dx);
      double cos_theta_exitant_dy = Z_sign*sqrt(1.0-sin_theta_exitant_sqr_dy);

      o_exitant_ray.m_origin_dx = i_dg.m_point_dx;
      o_exitant_ray.m_direction_dx = eta*i_incident_ray.m_direction_dx+(eta*cos_theta_incident_dx+cos_theta_exitant_dx)*i_dg.m_normal_dx;
      ASSERT(o_exitant_ray.m_direction_dx.IsNormalized());

      o_exitant_ray.m_origin_dy = i_dg.m_point_dy;
      o_exitant_ray.m_direction_dy = eta*i_incident_ray.m_direction_dy+(eta*cos_theta_incident_dy+cos_theta_exitant_dy)*i_dg.m_normal_dy;
      ASSERT(o_exitant_ray.m_direction_dy.IsNormalized());

      o_exitant_ray.m_has_differentials = true;
      }
    }

  }

#endif // CORE_UTILS_H