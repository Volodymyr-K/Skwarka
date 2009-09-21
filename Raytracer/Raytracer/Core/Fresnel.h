#ifndef FRESNEL_H
#define FRESNEL_H

#include <Common/Common.h>
#include <Math/Constants.h>
#include <Math/Util.h>
#include <utility>
#include <cmath>

/**
* Returns the fraction of the incoming light that is reflected by a dielectric surface.
* The fraction of the transmitted light is equal to one minus the fraction of the reflected light.
* @param i_refractive_index_inner Refractive index of the object. Should be positive.
* @param i_refractive_index_outer Refractive index of the medium. Should be positive.
* @param i_cos_theta Cosine of the angle between the surface normal and the incident direction.
* If the light is coming from the outer medium into the object the value should be positive.
* If the light is coming from within the object into the medium the value should be negative.
*/
double FresnelDielectricReflectance(double i_refractive_index_inner, double i_refractive_index_outer, double i_cos_theta);

/**
* Returns the fraction of the incoming light that is reflected by a conductor surface.
* Conductors do not transmit light.
* @param i_refractive_index Refractive index of the object. Should be positive.
* @param i_absorption Object absorption coefficient (e.g. 2.82 for gold). Should be positive.
* @param i_cos_theta Cosine of the angle between the surface normal and the incident direction. Should be positive.
*/
double FresnelConductorReflectance(double i_refractive_index, double i_absorption, double i_cos_theta);

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline double FresnelDielectricReflectance(double i_refractive_index_inner, double i_refractive_index_outer, double i_cos_theta)
  {
  ASSERT(i_refractive_index_inner>0.0 && i_refractive_index_outer>0.0);
  ASSERT(i_cos_theta>=-1.0-DBL_EPS && i_cos_theta<=1.0+DBL_EPS);

  i_cos_theta = MathRoutines::Clamp(i_cos_theta, -1.0, 1.0);
  if (i_cos_theta < 0.0)
    std::swap(i_refractive_index_inner, i_refractive_index_outer);

  // Compute refracted sin using Snell's law.
  double sin_theta_refracted = i_refractive_index_outer/i_refractive_index_inner * sqrt(std::max(0.0, 1.0 - i_cos_theta*i_cos_theta));
  if (sin_theta_refracted > 1.0)
    // Handle total internal reflection.
    return 1.0;
  else
    {
    double cos_theta_refracted = sqrt(std::max(0.0, 1.0 - sin_theta_refracted*sin_theta_refracted));
    i_cos_theta=fabs(i_cos_theta);

    double R_parl =
      ((i_refractive_index_inner * i_cos_theta) - (i_refractive_index_outer * cos_theta_refracted)) /
      ((i_refractive_index_inner * i_cos_theta) + (i_refractive_index_outer * cos_theta_refracted));
    double R_perp =
      ((i_refractive_index_outer * i_cos_theta) - (i_refractive_index_inner * cos_theta_refracted)) /
      ((i_refractive_index_outer * i_cos_theta) + (i_refractive_index_inner * cos_theta_refracted));

    return (R_parl*R_parl + R_perp*R_perp)*0.5;
    }
  }

inline double FresnelConductorReflectance(double i_refractive_index, double i_absorption, double i_cos_theta)
  {
  ASSERT(i_refractive_index>0.0 && i_absorption>=0.0);
  ASSERT(i_cos_theta>=-DBL_EPS && i_cos_theta<=1.0+DBL_EPS);
  i_cos_theta=fabs(i_cos_theta);

  double tmp = (i_refractive_index*i_refractive_index + i_absorption*i_absorption) * i_cos_theta*i_cos_theta;
  double R_parl_sqr =
    (tmp - (2.0 * i_refractive_index * i_cos_theta) + 1.0) /
    (tmp + (2.0 * i_refractive_index * i_cos_theta) + 1.0);

  double tmp2 = i_refractive_index*i_refractive_index + i_absorption*i_absorption;
  double R_perp_sqr =
    (tmp2 - (2.0 * i_refractive_index * i_cos_theta) + i_cos_theta*i_cos_theta) /
    (tmp2 + (2.0 * i_refractive_index * i_cos_theta) + i_cos_theta*i_cos_theta);

  return (R_parl_sqr + R_perp_sqr)*0.5;
  }

#endif // FRESNEL_H