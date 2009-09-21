#ifndef BLINN_DISTRIBUTION_H
#define BLINN_DISTRIBUTION_H

#include <Common/Common.h>
#include <Math/Constants.h>
#include <Math/Util.h>
#include <Math/Geometry.h>
#include <limits>

class BlinnDistribution
  {
  public:
    BlinnDistribution(double i_exponent);

    double Evaluate(const Vector3D_d &i_half_angle_direction) const;

    void Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const;

    double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

  private:
    double m_exponent;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline BlinnDistribution::BlinnDistribution(double i_exponent)
  {
  ASSERT(i_exponent>=0.0);
  m_exponent=MathRoutines::Clamp(i_exponent, 0.0, 1000.0);
  }

inline double BlinnDistribution::Evaluate(const Vector3D_d &i_half_angle_direction) const
  {
  double cos_theta = fabs(i_half_angle_direction[2]);
  return (m_exponent+2.0) * INV_2PI * pow(cos_theta, m_exponent);
  }

inline void BlinnDistribution::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
  {
  double cos_theta = pow(i_sample[0], 1.0 / (m_exponent+1.0));
  double sint_heta = sqrt(std::max(0.0, 1.0 - cos_theta*cos_theta));
  double phi = i_sample[1] * 2.0 * M_PI;
  Vector3D_d half_angle(sint_heta*cos(phi), sint_heta*sin(phi), cos_theta);
  if (i_incident[2]*half_angle[2]>0.0)
    half_angle[2] *= -1.0;

  o_exitant = half_angle * (2.0*(i_incident*half_angle)) - i_incident;
  o_pdf = ((m_exponent + 2.0) * pow(cos_theta, m_exponent)) / (2.0 * M_PI * 4.0 * (i_incident*half_angle));
  }

double BlinnDistribution::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  Vector3D_d half_angle(i_incident + i_exitant);
  half_angle.Normalize();

  double cos_theta = fabs(half_angle[2]);

  return ((m_exponent + 2.0) * pow(cos_theta, m_exponent)) / (2.0 * M_PI * 4.0 * (i_incident*half_angle));
  }

#endif // BLINN_DISTRIBUTION_H