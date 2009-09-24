#ifndef FRESNEL_H
#define FRESNEL_H

#include <Common/Common.h>
#include <Math/Constants.h>
#include <Math/Util.h>
#include <utility>
#include <cmath>

/**
* Computes the fraction of incoming light that is reflected by a dielectric surface.
*/
class FresnelDielectric
  {
  public:
    /**
    * Creates FresnelDielectric instance.
    * @param i_refractive_index_inner Refractive index of the object. Should be positive.
    * @param i_refractive_index_outer Refractive index of the medium. Should be positive.
    */
    FresnelDielectric(double i_refractive_index_inner, double i_refractive_index_outer);

    /**
    * Returns the fraction of the incoming light that is reflected by a dielectric surface.
    * The fraction of the transmitted light is equal to one minus the fraction of the reflected light.
    * @param i_cos_theta Cosine of the angle between the surface normal and the incident direction.
    * If the light is coming from the outer medium into the object the value should be positive.
    * If the light is coming from within the object into the medium the value should be negative.
    * @return Reflected light fraction. Will be in [0;1] range.
    */
    double operator()(double i_cos_theta) const;

  private:
    double m_refractive_index_inner, m_refractive_index_outer;
  };

/**
* Computes the fraction of incoming light that is reflected by a conductor surface.
*/
class FresnelConductor
  {
  public:
    /**
    * Creates FresnelConductor instance.
    * @param i_refractive_index Refractive index of the object. Should be positive.
    * @param i_absorption Object absorption coefficient (e.g. 2.82 for gold). Should be positive.
    */
    FresnelConductor(double i_refractive_index, double i_absorption);

    /**
    * Returns the fraction of the incoming light that is reflected by a conductor surface.
    * Conductors do not transmit light.
    * @param i_cos_theta Cosine of the angle between the surface normal and the incident direction. Should be positive.
    * @return Reflected light fraction. Will be in [0;1] range.
    */
    double operator()(double i_cos_theta) const;

  private:
    double m_refractive_index, m_absorption;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline FresnelDielectric::FresnelDielectric(double i_refractive_index_inner, double i_refractive_index_outer):
m_refractive_index_inner(i_refractive_index_inner), m_refractive_index_outer(i_refractive_index_outer)
  {
  ASSERT(i_refractive_index_inner>0.0 && i_refractive_index_outer>0.0);
  }

inline double FresnelDielectric::operator()(double i_cos_theta) const
  {
  ASSERT(i_cos_theta>=-1.0-DBL_EPS && i_cos_theta<=1.0+DBL_EPS);
  i_cos_theta = MathRoutines::Clamp(i_cos_theta, -1.0, 1.0);

  double refractive_index_inner(m_refractive_index_inner), refractive_index_outer(m_refractive_index_outer);
  if (i_cos_theta < 0.0)
    std::swap(refractive_index_inner, refractive_index_outer);

  // Compute refracted sin using Snell's law.
  double sin_theta_refracted = refractive_index_outer/refractive_index_inner * sqrt(std::max(0.0, 1.0 - i_cos_theta*i_cos_theta));
  if (sin_theta_refracted >= 1.0)
    // Handle total internal reflection.
    return 1.0;
  else
    {
    double cos_theta_refracted = sqrt(std::max(0.0, 1.0 - sin_theta_refracted*sin_theta_refracted));
    i_cos_theta=fabs(i_cos_theta);

    double R_parl =
      ((refractive_index_inner * i_cos_theta) - (refractive_index_outer * cos_theta_refracted)) /
      ((refractive_index_inner * i_cos_theta) + (refractive_index_outer * cos_theta_refracted));
    double R_perp =
      ((refractive_index_outer * i_cos_theta) - (refractive_index_inner * cos_theta_refracted)) /
      ((refractive_index_outer * i_cos_theta) + (refractive_index_inner * cos_theta_refracted));

    return (R_parl*R_parl + R_perp*R_perp)*0.5;
    }
  }

inline FresnelConductor::FresnelConductor(double i_refractive_index, double i_absorption):
m_refractive_index(i_refractive_index), m_absorption(i_absorption)
  {
  ASSERT(i_refractive_index>0.0 && i_absorption>=0.0);
  }

inline double FresnelConductor::operator()(double i_cos_theta) const
  {
  ASSERT(i_cos_theta>=-DBL_EPS && i_cos_theta<=1.0+DBL_EPS);
  i_cos_theta=fabs(i_cos_theta);

  double tmp1 = m_refractive_index*m_refractive_index + m_absorption*m_absorption;
  double tmp2 = tmp1 * i_cos_theta*i_cos_theta;

  double R_parl_sqr =
    (tmp2 - (2.0 * m_refractive_index * i_cos_theta) + 1.0) /
    (tmp2 + (2.0 * m_refractive_index * i_cos_theta) + 1.0);

  double R_perp_sqr =
    (tmp1 - (2.0 * m_refractive_index * i_cos_theta) + i_cos_theta*i_cos_theta) /
    (tmp1 + (2.0 * m_refractive_index * i_cos_theta) + i_cos_theta*i_cos_theta);

  return (R_parl_sqr + R_perp_sqr)*0.5;
  }

#endif // FRESNEL_H