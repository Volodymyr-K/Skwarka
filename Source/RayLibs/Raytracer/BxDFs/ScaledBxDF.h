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

#ifndef SCALED_BXDF_H
#define SCALED_BXDF_H

#include <Raytracer/Core/BxDF.h>

/**
* BxDF implementation that scales another pluggable BxDF with a scale factor.
* Implements Decorator design pattern.
*/
class ScaledBxDF: public BxDF
  {
  public:
    /**
    * Creates ScaledBxDF instance with the specified reference BxDF and scale factor.
    * @param ip_bxdf Underlying BxDF instance. Should not be NULL.
    * @param i_scale Scale factor.
    */
    ScaledBxDF(const BxDF *ip_bxdf, SpectrumCoef_d i_scale);

    /**
    * Returns BxDF value for the specified incident and exitant directions.
    */
    virtual SpectrumCoef_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Samples BxDF value for the specified incident direction.
    * The sampled exitant direction may be in any hemisphere, regardless of the incident direction.
    * @param i_incident Incident direction. Should be normalized.
    * @param[out] o_exitant Exitant direction in the correct hemisphere. The returned value should be normalized.
    * @param i_sample 2D sample. Should be in [0;1)^2 range.
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

    /**
    * Returns total scattering (i.e. fraction of scattered light) assuming a unit of light coming from the specified incident direction.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_samples 2D Samples sequence to be used for sampling the hemisphere. Should have at least one sample.
    * @return Total scattering value. Each spectrum component will be in [0;1] range.
    */
    virtual SpectrumCoef_d TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const;

    /**
    * Returns total scattering (i.e. fraction of scattered light) assuming a light coming uniformly from the specified hemisphere.
    * The method takes two 2D samples sequences that must have the same number of elements.
    * @param i_hemisphere Defines the hemisphere of the incoming light.
    * Value true corresponds to the hemisphere above XY plane (i.e. with positive Z coordinate) and
    * value false corresponds to the hemisphere below XY plane (i.e. with negative Z coordinate).
    * @param i_samples1 First samples sequence. Should have the same number of elements that i_samples2 has.
    * @param i_samples2 Second samples sequence. Should have the same number of elements that i_samples1 has.
    * @return Total scattering value. Each spectrum component will be in [0;1] range.
    */
    virtual SpectrumCoef_d TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples1, SamplesSequence2D i_samples2) const;

  private:
    const BxDF *mp_bxdf;

    SpectrumCoef_d m_scale;
  };

#endif // SCALED_BXDF_H