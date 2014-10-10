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

#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include <Raytracer/Core/BxDF.h>

/**
* Diffuse reflective BxDF implementation that conforms to the Lambertian law.
* The BxDF value is constant over the entire hemisphere.
* @sa OrenNayar
*/
class Lambertian: public BxDF
  {
  public:
    /**
    * Creates Lambertian instance with the specified reflectance.
    * @param i_reflectance The total hemisphere reflectance. Each spectrum component should be in [0;1] range.
    */
    Lambertian(SpectrumCoef_d i_reflectance);

    /**
    * Returns BxDF value for the specified incident and exitant directions.
    * The returned value is constant over the entire hemisphere.
    */
    virtual SpectrumCoef_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Returns total scattering (i.e. fraction of scattered light) assuming a unit of light coming from the specified incident direction.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_samples 2D Samples sequence to be used for sampling the hemisphere. Should have at least one sample.
    * @return Total scattering value.
    */
    virtual SpectrumCoef_d TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const;

    /**
    * Returns total scattering (i.e. fraction of scattered light) assuming a light coming uniformly from the specified hemisphere.
    * Each sample of the integral requires two 2D samples so the input samples sequence should have twice the number of needed samples.
    * @param i_hemisphere Defines the hemisphere of the incoming light.
    * Value true corresponds to the hemisphere above XY plane (i.e. with positive Z coordinate) and
    * value false corresponds to the hemisphere below XY plane (i.e. with negative Z coordinate).
    * @param i_samples1 First samples sequence. Should have the same number of elements that i_samples2 has.
    * @param i_samples2 Second samples sequence. Should have the same number of elements that i_samples1 has.
    * @return Total scattering value.
    */
    virtual SpectrumCoef_d TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples1, SamplesSequence2D i_samples2) const;

  private:
    SpectrumCoef_d m_reflectance, m_reflectance_inv_pi;
  };

#endif // LAMBERTIAN_H