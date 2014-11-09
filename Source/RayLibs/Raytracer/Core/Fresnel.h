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

#ifndef FRESNEL_H
#define FRESNEL_H

#include <Common/Common.h>
#include "Spectrum.h"
#include <Math/Constants.h>
#include <Math/MathRoutines.h>
#include <cmath>
#include <algorithm>

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
    * @return Reflected light fraction. Each spectrum component will be in [0;1] range.
    */
    SpectrumCoef_d operator()(double i_cos_theta) const;

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
    FresnelConductor(const SpectrumCoef_d &i_refractive_index, const SpectrumCoef_d &i_absorption);

    /**
    * Returns the fraction of the incoming light that is reflected by a conductor surface.
    * Conductors do not transmit light.
    * @param i_cos_theta Cosine of the angle between the surface normal and the incident direction. Should be positive.
    * @return Reflected light fraction. Each spectrum component will be in [0;1] range.
    */
    SpectrumCoef_d operator()(double i_cos_theta) const;

  private:
    SpectrumCoef_d m_refractive_index, m_absorption_sqr;
  };

/**
* Approximates the refractive index and absorption coefficient for a conductor material with the specified reflectance.
* The specified reflectance value is the reflectance of the material for a normal incident direction (i.e. when the theta angle is zero).
* The method uses value 2.0 as the first guess for the absorption coefficient. This works well for most of the conductor materials.
* @param i_reflection Reflectance of the material for a normal incident direction (i.e. when the theta angle is zero). Each spectrum component should be in [0;1] range.
* @param[out] o_refractive_index Refractive index of the object. Each spectrum component will be positive.
* @param[out] o_absorption Object absorption coefficient. Each spectrum component will be positive.
*/
void ApproximateFresnelParameters(const SpectrumCoef_d &i_reflection, SpectrumCoef_d &o_refractive_index, SpectrumCoef_d &o_absorption);

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline FresnelDielectric::FresnelDielectric(double i_refractive_index_inner, double i_refractive_index_outer):
m_refractive_index_inner(i_refractive_index_inner), m_refractive_index_outer(i_refractive_index_outer)
  {
  ASSERT(i_refractive_index_inner>0.0 && i_refractive_index_outer>0.0);
  }

inline SpectrumCoef_d FresnelDielectric::operator()(double i_cos_theta) const
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
    return SpectrumCoef_d(1.0);
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

    return SpectrumCoef_d( (R_parl*R_parl + R_perp*R_perp)*0.5 );
    }
  }

inline FresnelConductor::FresnelConductor(const SpectrumCoef_d &i_refractive_index, const SpectrumCoef_d &i_absorption):
m_refractive_index(i_refractive_index), m_absorption_sqr(i_absorption*i_absorption)
  {
  ASSERT(InRange(i_refractive_index,0.0,DBL_INF));
  ASSERT(InRange(i_absorption,0.0,DBL_INF));
  }

inline SpectrumCoef_d FresnelConductor::operator()(double i_cos_theta) const
  {
  ASSERT(i_cos_theta>=-DBL_EPS && i_cos_theta<=1.0+DBL_EPS);
  i_cos_theta=fabs(i_cos_theta);

  SpectrumCoef_d tmp1 = m_refractive_index*m_refractive_index + m_absorption_sqr;
  SpectrumCoef_d tmp2 = tmp1 * (i_cos_theta*i_cos_theta);

  SpectrumCoef_d R_parl_sqr =
    (tmp2 - (2.0 * i_cos_theta) * m_refractive_index  + SpectrumCoef_d(1.0)) /
    (tmp2 + (2.0 * i_cos_theta) * m_refractive_index  + SpectrumCoef_d(1.0));

  SpectrumCoef_d R_perp_sqr =
    (tmp1 - (2.0 * m_refractive_index * i_cos_theta) + SpectrumCoef_d(i_cos_theta*i_cos_theta)) /
    (tmp1 + (2.0 * m_refractive_index * i_cos_theta) + SpectrumCoef_d(i_cos_theta*i_cos_theta));

  return (R_parl_sqr + R_perp_sqr) * 0.5;
  }

inline void ApproximateFresnelParameters(const SpectrumCoef_d &i_reflection, SpectrumCoef_d &o_refractive_index, SpectrumCoef_d &o_absorption)
  {
  ASSERT(InRange(i_reflection,0.0,1.0));

  for(unsigned char i=0;i<3;++i)
    {
    // We use value 2.0 as the first guess for the absorption coefficient value.
    // It works pretty well for most of the conducting materials.
    double k=2.0;

    if (i_reflection[i]>=1.0)
      {
      o_refractive_index[i] = 0.0;
      o_absorption[i] = k;
      }
    else
      if (i_reflection[i]<=0.0)
        {
        o_refractive_index[i] = 1.0;
        o_absorption[i] = 0.0;
        }
      else
        {
        double discriminant = 4.0*(1+i_reflection[i])*(1+i_reflection[i])-4.0*(1-i_reflection[i])*(1-i_reflection[i])*(1.0+k*k);

        if (discriminant >= 0.0)
          {
          o_refractive_index[i] = (2.0*(1+i_reflection[i])-sqrt(discriminant)) / (2.0*(1-i_reflection[i]));
          o_absorption[i] = k;
          }
        else
          {
          // If the discriminant is less than zero we find the value of absorption coefficient which makes the discriminant zero.
          o_refractive_index[i] = (1+i_reflection[i]) / (1-i_reflection[i]);
          o_absorption[i] = 2.0*sqrt(i_reflection[i]) / (1-i_reflection[i]);
          }
        }
    } // for

  ASSERT(InRange(o_refractive_index,0.0,DBL_INF));
  ASSERT(InRange(o_absorption,0.0,DBL_INF));
  }

#endif // FRESNEL_H