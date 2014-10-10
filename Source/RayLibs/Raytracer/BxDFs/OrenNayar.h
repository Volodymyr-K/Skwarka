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

#ifndef OREN_NAYAR_H
#define OREN_NAYAR_H

#include <Raytracer/Core/BxDF.h>

/**
* Diffuse reflective BxDF implementation that follows the Oren-Nayar model for rough surfaces.
* The model takes into account complex physical phenomena such as masking, shadowing and interreflections
* between points on the surface facets. It can be viewed as a generalization of Lambert's law.
* @sa Lambertian
*/
class OrenNayar: public BxDF
  {
  public:
    /**
    * Creates OrenNayar instance with the specified reflectance and sigma value (roughness of the surface).
    * If sigma value is zero the model produces the same results as the Lambertian one.
    * @param i_reflectance The total hemisphere reflectance. Each spectrum component should be in [0;1] range.
    * @param i_sigma Roughness of the surface. Should be in [0;1] range.
    */
    OrenNayar(SpectrumCoef_d i_reflectance, double i_sigma);

    /**
    * Returns BxDF value for the specified incident and exitant directions.
    */
    virtual SpectrumCoef_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

  private:
    SpectrumCoef_d m_reflectance;
    double m_A, m_B;
  };

#endif // OREN_NAYAR_H