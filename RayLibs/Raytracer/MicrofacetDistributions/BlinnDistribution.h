#ifndef BLINN_DISTRIBUTION_H
#define BLINN_DISTRIBUTION_H

#include <Common/Common.h>
#include <Math/Constants.h>
#include <Math/MathRoutines.h>
#include <Math/Geometry.h>
#include <cmath>

/**
* The Blinn microfacet normal distribution implementation.
* In this model the distribution of microfacet normals is approximated by an exponential falloff.
* @sa Microfacet, AnisotropicDistribution
*/
class BlinnDistribution
  {
  public:
    /**
    * Creates BlinnDistribution instance with the specified exponential factor.
    * @param i_exponent The exponential factor that characterizes the roughness of the surface. Should not be negative.
    */
    BlinnDistribution(double i_exponent);

    /**
    * Evaluates the distribution value for the specified i_microfacet_normal direction.
    * @param i_microfacet_normal Microfacet normal. Should be normalized.
    */
    double Evaluate(const Vector3D_d &i_microfacet_normal) const;

    /**
    * Samples the exitant direction with the Blinn distribution.
    * @param i_incident Incident direction. Should be normalized.
    * @param[out] o_exitant Exitant direction. May be in any hemisphere, regardless of the incident direction. The returned value should be normalized.
    * @param i_sample 2D sample. Should be in [0;1]^2 range.
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
    double m_exponent;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline BlinnDistribution::BlinnDistribution(double i_exponent)
  {
  ASSERT(i_exponent>=0.0);
  m_exponent = MathRoutines::Clamp(i_exponent, 0.0, 1000.0);
  }

inline double BlinnDistribution::Evaluate(const Vector3D_d &i_microfacet_normal) const
  {
  ASSERT(i_microfacet_normal.IsNormalized());

  double cos_theta = fabs(i_microfacet_normal[2]);
  return (m_exponent+2.0) * INV_2PI * pow(cos_theta, m_exponent);
  }

inline void BlinnDistribution::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
  {
  ASSERT(i_incident.IsNormalized());

  double cos_theta = pow(i_sample[0], 1.0 / (m_exponent+1.0));
  double sin_theta = sqrt(std::max(0.0, 1.0 - cos_theta*cos_theta));
  double phi = i_sample[1] * 2.0 * M_PI;
  Vector3D_d half_angle(sin_theta*cos(phi), sin_theta*sin(phi), cos_theta);

  // Flip the Z coordinate if needed to match the correct hemisphere.
  if (i_incident[2]*half_angle[2]<0.0)
    half_angle[2] *= -1.0;

	// Compute exitant direction by reflecting about the half angle vector.
  o_exitant = half_angle * (2.0*(i_incident*half_angle)) - i_incident;
  o_pdf = ((m_exponent + 1.0) * pow(cos_theta, m_exponent)) / (2.0 * M_PI * 4.0 * fabs(i_incident*half_angle));
  }

inline double BlinnDistribution::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  Vector3D_d half_angle(i_incident + i_exitant);
  half_angle.Normalize();

  double cos_theta = fabs(half_angle[2]);
  return ((m_exponent + 1.0) * pow(cos_theta, m_exponent)) / (2.0 * M_PI * 4.0 * fabs(i_incident*half_angle));
  }

#endif // BLINN_DISTRIBUTION_H