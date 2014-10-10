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

#include "Lambertian.h"
#include <Math/Constants.h>

Lambertian::Lambertian(SpectrumCoef_d i_reflectance):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), m_reflectance(i_reflectance), m_reflectance_inv_pi(i_reflectance*INV_PI)
  {
  ASSERT(InRange(i_reflectance,0.0,1.0));
  }

// The method does not check if the vectors are in the same hemisphere and returns constant value everywhere.
SpectrumCoef_d Lambertian::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  ASSERT(i_incident.IsNormalized());
  ASSERT(i_exitant.IsNormalized());
  return m_reflectance_inv_pi;
  }

SpectrumCoef_d Lambertian::TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const
  {
  ASSERT(i_incident.IsNormalized());
  return m_reflectance;
  }

SpectrumCoef_d Lambertian::TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples1, SamplesSequence2D i_samples2) const
  {
  return m_reflectance;
  }