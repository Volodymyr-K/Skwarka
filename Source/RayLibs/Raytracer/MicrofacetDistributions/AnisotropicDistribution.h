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

#ifndef ANISOTROPIC_DISTRIBUTION_H
#define ANISOTROPIC_DISTRIBUTION_H

#include <Common/Common.h>
#include <Math/Constants.h>
#include <Math/MathRoutines.h>
#include <Math/Geometry.h>
#include <cmath>
#include <algorithm>

/**
* An anisotropic microfacet normal distribution implementation (Ashikhmin and Shirley's model)
* In this model the distribution of microfacet normals is approximated by exponential falloff (which is different for U and V directions).
* @sa Microfacet, BlinnDistribution
*/
class AnisotropicDistribution
  {
  public:

    /**
    * Creates AnisotropicDistribution instance with the specified exponential factors.
    * @param i_u_exponent The exponential factor that characterizes the roughness of the surface in U direction. Should not be negative.
    * @param i_v_exponent The exponential factor that characterizes the roughness of the surface in V direction. Should not be negative.
    */
    AnisotropicDistribution(double i_u_exponent, double i_v_exponent);

    /**
    * Evaluates the distribution value for the specified i_microfacet_normal direction.
    * @param i_microfacet_normal Microfacet normal. Should be normalized.
    */
    double Evaluate(const Vector3D_d &i_microfacet_normal) const;

    /**
    * Samples the exitant direction with the anisotropic distribution.
    * @param i_incident Incident direction. Should be normalized.
    * @param[out] o_exitant Exitant direction. May be in any hemisphere, regardless of the incident direction. The returned value should be normalized.
    * @param i_sample 2D sample. Should be in [0;1]x[0;1] range.
    * @param[out] o_pdf PDF value for the sampled exitant direction. Greater or equal than zero.
    */
    void Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const;

    /**
    * Returns PDF value for the specified incident and exitant direction.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_exitant Exitant direction. Should be normalized.
    * @return PDF value. Greater or equal than zero.
    */
    double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

  private:
    void _SampleFirstQuadrant(const Point2D_d &i_sample, double &o_phi, double &o_cos_theta) const;

  private:
    double m_u_exponent, m_v_exponent;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline AnisotropicDistribution::AnisotropicDistribution(double i_u_exponent, double i_v_exponent)
  {
  ASSERT(i_u_exponent>=0.0 && i_v_exponent>=0.0);
  m_u_exponent = MathRoutines::Clamp(i_u_exponent, 0.0, 1000.0);
  m_v_exponent = MathRoutines::Clamp(i_v_exponent, 0.0, 1000.0);
  }

inline double AnisotropicDistribution::Evaluate(const Vector3D_d &i_microfacet_normal) const
  {
  ASSERT(i_microfacet_normal.IsNormalized());

  double cos_theta = fabs(i_microfacet_normal[2]);

  double d = 1.0 - cos_theta*cos_theta;
  if (fabs(d)<DBL_EPS) return 0.0;

  // Exponent is found by considering m_u_exponent and m_v_exponent as the lengths of the axes of an ellipse and finding the appropriate radius
  // for the actual microfacet orientation.
  double e = (m_u_exponent*i_microfacet_normal[0]*i_microfacet_normal[0] + m_v_exponent*i_microfacet_normal[1]*i_microfacet_normal[1]) / d;
  return sqrt((m_u_exponent+2.0)*(m_v_exponent+2.0)) * INV_2PI * pow(cos_theta, e);
  }

inline void AnisotropicDistribution::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
  {
  ASSERT(i_incident.IsNormalized());

  // Sample from first quadrant and remap to hemisphere to sample half-angle vector.
  double phi, cos_theta;
  if (i_sample[0] < 0.25)
    {
    Point2D_d sample(4.0 * i_sample[0], i_sample[1]);
    _SampleFirstQuadrant(sample, phi, cos_theta);
    }
  else if (i_sample[0] < 0.5)
    {
    Point2D_d sample(4.0 * (0.5 - i_sample[0]), i_sample[1]);
    _SampleFirstQuadrant(sample, phi, cos_theta);
    phi = M_PI - phi;
    }
  else if (i_sample[0] < 0.75)
    {
    Point2D_d sample(4.0 * (i_sample[0] - 0.5), i_sample[1]);
    _SampleFirstQuadrant(sample, phi, cos_theta);
    phi += M_PI;
    }
  else
    {
    Point2D_d sample(4.0 * (1.0 - i_sample[0]), i_sample[1]);
    _SampleFirstQuadrant(sample, phi, cos_theta);
    phi = 2.0 * M_PI - phi;
    }

  double sin_theta = sqrt(std::max(0.0, 1.0 - cos_theta*cos_theta));
  Vector3D_d half_angle(sin_theta*cos(phi), sin_theta*sin(phi), cos_theta);

  // Flip the Z coordinate if needed to match the correct hemisphere.
  if (i_incident[2]*half_angle[2]<0.0)
    half_angle[2] *= -1.0;

  // Compute exitant direction by reflecting about the half angle vector.
  o_exitant = half_angle * (2.0*(i_incident*half_angle)) - i_incident;

  // Compute PDF for o_exitant from anisotropic distribution.
  double ds = 1.0 - cos_theta*cos_theta;
  if (ds>0.0)
    {
    double e = (m_u_exponent * half_angle[0] * half_angle[0] + m_v_exponent * half_angle[1] * half_angle[1]) / ds;
    o_pdf = sqrt((m_u_exponent+1.0) * (m_v_exponent+1.0)) * pow(cos_theta, e) / (2.0 * M_PI * 4.0 * fabs(i_incident*half_angle));
    }
  else
    o_pdf = 0.0;
  }

inline void AnisotropicDistribution::_SampleFirstQuadrant(const Point2D_d &i_sample, double &o_phi, double &o_cos_theta) const
  {
  if (m_u_exponent == m_v_exponent)
    o_phi = M_PI * i_sample[0] * 0.5;
  else
    o_phi = atan(sqrt((m_u_exponent+1.0) / (m_v_exponent+1.0)) * tan(M_PI * i_sample[0] * 0.5));

  double cos_phi = cos(o_phi), sin_phi = sin(o_phi);
  o_cos_theta = pow(i_sample[1], 1.0/(m_u_exponent * cos_phi*cos_phi + m_v_exponent * sin_phi*sin_phi + 1.0));
  }

inline double AnisotropicDistribution::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  Vector3D_d half_angle(i_incident + i_exitant);
  half_angle.Normalize();

  // Compute PDF for i_exitant from anisotropic distribution.
  double cos_theta = fabs(half_angle[2]);
  double ds = 1.0 - cos_theta * cos_theta;
  if (ds>0.0)
    {
    double e = (m_u_exponent * half_angle[0] * half_angle[0] + m_v_exponent * half_angle[1] * half_angle[1]) / ds;
    return sqrt((m_u_exponent+1.0) * (m_v_exponent+1.0)) * pow(cos_theta, e) / (2.0 * M_PI * 4.0 * fabs(i_incident*half_angle));
    }
  else
    return 0.0;
  }

#endif // ANISOTROPIC_DISTRIBUTION_H