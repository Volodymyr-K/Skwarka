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

#ifndef MICROFACET_H
#define MICROFACET_H

#include <Raytracer/Core/BxDF.h>
#include <Raytracer/Core/Fresnel.h>

/**
* Glossy reflective BxDF implementation that follows the Torrance-Sparrow model for rough surfaces.
* The model takes into account complex physical phenomena such as masking, shadowing and interreflections between points on the surface facets.
* The two template parameters are:
*   - Fresnel implementation that is used to get the facet reflectance. Should define "double operator(double) const" method.
*   - Microfacet distribution implementation that defines the distribution of the facets normals.
*/
template<typename Fresnel, typename MicrofacetDistribution>
class Microfacet: public BxDF
  {
  public:
    /**
    * Creates Microfacet instance with the specified reflectance, Fresnel and MicrofacetDistribution instances.
    * @param i_reflectance The reflectance(color) of the surface. Each spectrum component should be in [0;1] range.
    * @param i_fresnel Fresnel implementation.
    * @param i_distribution MicrofacetDistribution implementation.
    */
    Microfacet(SpectrumCoef_d i_reflectance, const Fresnel &i_fresnel, const MicrofacetDistribution &i_distribution);

    /**
    * Returns BxDF value for the specified incident and exitant directions.
    */
    virtual SpectrumCoef_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Samples BxDF value for the specified incident direction.
    * The sampled exitant direction may be in any hemisphere, regardless of the incident direction.
    * The MicrofacetDistribution is used to sample the exitant direction.
    * @param i_incident Incident direction. Should be normalized.
    * @param[out] o_exitant Exitant direction. The returned value should be normalized.
    * @param i_sample 2D sample. Should be in [0;1]x[0;1] range.
    * @param[out] o_pdf PDF value for the sampled exitant direction. The returned value should be greater or equal than zero.
    * @return Sampled BxDF value.
    */
    virtual SpectrumCoef_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const;

    /**
    * Returns PDF value for the specified incident and exitant direction.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_exitant Exitant direction. Should be normalized.
    * @return PDF value. Greater or equal than zero.
    */
    virtual double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

  private:
    /**
    * Computes the geometric attenuation term of the Torrance-Sparrow model.
    */
    double _GeometricAttenuation(const Vector3D_d &i_incident, const Vector3D_d &i_exitant, const Vector3D_d &i_half_angle) const;

  private:
    SpectrumCoef_d m_reflectance;
    Fresnel m_fresnel;
    MicrofacetDistribution m_distribution;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Fresnel, typename MicrofacetDistribution>
Microfacet<Fresnel,MicrofacetDistribution>::Microfacet(SpectrumCoef_d i_reflectance, const Fresnel &i_fresnel,
                                                       const MicrofacetDistribution &i_distribution):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)), m_reflectance(i_reflectance), m_fresnel(i_fresnel), m_distribution(i_distribution)
  {
  ASSERT(InRange(i_reflectance, 0.0, 1.0));
  }

template<typename Fresnel, typename MicrofacetDistribution>
SpectrumCoef_d Microfacet<Fresnel,MicrofacetDistribution>::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  double cos_theta_incident = fabs(i_incident[2]);
  double cos_theta_exitant = fabs(i_exitant[2]);
  if (cos_theta_incident<DBL_EPS || cos_theta_exitant<DBL_EPS)
    return SpectrumCoef_d();

  Vector3D_d half_angle(i_incident + i_exitant);
  if (fabs(half_angle[0])<DBL_EPS && fabs(half_angle[1])<DBL_EPS && fabs(half_angle[2])<DBL_EPS)
    return SpectrumCoef_d();
  half_angle.Normalize();

  double cosine_half_angle = i_exitant*half_angle;
  SpectrumCoef_d fresnel = m_fresnel(cosine_half_angle);

  return (m_reflectance * fresnel) *
    (m_distribution.Evaluate(half_angle) * _GeometricAttenuation(i_incident, i_exitant, half_angle) / (4.0 * cos_theta_incident * cos_theta_exitant));
  }

template<typename Fresnel, typename MicrofacetDistribution>
SpectrumCoef_d Microfacet<Fresnel,MicrofacetDistribution>::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant,
                                                                  const Point2D_d &i_sample, double &o_pdf) const
  {
  ASSERT(i_sample[0]>=0.0 && i_sample[0]<=1.0);
  ASSERT(i_sample[1]>=0.0 && i_sample[1]<=1.0);
  ASSERT(i_incident.IsNormalized());

  m_distribution.Sample(i_incident, o_exitant, i_sample, o_pdf);

  // Microfacet distribution implementation can sample exitant vectors in a wrong hemisphere, so we just ignore those results.
  if (i_incident[2]*o_exitant[2]<0.0)
    {
    o_pdf = 0.0;
    return SpectrumCoef_d(0.0);
    }

  return this->Evaluate(i_incident, o_exitant);
  }

template<typename Fresnel, typename MicrofacetDistribution>
double Microfacet<Fresnel,MicrofacetDistribution>::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());

  // Microfacet distribution implementation can sample exitant vectors in a wrong hemisphere, so we just ignore those results.
  if (i_incident[2]*i_exitant[2]<0.0)
    return 0.0;
  else
    return m_distribution.PDF(i_incident,i_exitant);
  }

template<typename Fresnel, typename MicrofacetDistribution>
double Microfacet<Fresnel,MicrofacetDistribution>::_GeometricAttenuation(const Vector3D_d &i_incident,const Vector3D_d &i_exitant,
                                             const Vector3D_d &i_half_angle) const
  {
  double cos_theta_half_angle = fabs(i_half_angle[2]);
  double cos_theta_incident = fabs(i_incident[2]);
  double cos_theta_exitant = fabs(i_exitant[2]);
  double tmp = 2.0 * cos_theta_half_angle/fabs(i_incident*i_half_angle);
  return std::min(1.0, std::min(cos_theta_incident * tmp, cos_theta_exitant * tmp) );
  }

#endif // MICROFACET_H