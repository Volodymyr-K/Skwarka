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

#include "OrenNayar.h"
#include <Math/MathRoutines.h>

OrenNayar::OrenNayar(SpectrumCoef_d i_reflectance, double i_sigma):
BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), m_reflectance(i_reflectance)
  {
  ASSERT(InRange(i_reflectance,0.0,1.0));
  ASSERT(i_sigma>=0.0 && i_sigma<=1.0);

  double sigma_sqr = i_sigma*i_sigma;
  m_A = 1.0 - (0.5 * sigma_sqr / (sigma_sqr + 0.33));
  m_B = 0.45 * sigma_sqr / (sigma_sqr + 0.09);
  }

// The method does not check if the vectors are in the same hemisphere.
SpectrumCoef_d OrenNayar::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  double cos_theta_incident = i_incident[2], cos_theta_exitant = i_exitant[2];

  double dcos = i_incident[0] * i_exitant[0] + i_incident[1] * i_exitant[1];
  double max_cos = std::max(0.0, dcos);

  if (fabs(cos_theta_incident) > fabs(cos_theta_exitant))
    return m_reflectance * (INV_PI * (m_A + m_B * max_cos / fabs(cos_theta_incident)));
  else
    return m_reflectance * (INV_PI * (m_A + m_B * max_cos / fabs(cos_theta_exitant)));
  }