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

#include "ScaledBxDF.h"

ScaledBxDF::ScaledBxDF(const BxDF *ip_bxdf, SpectrumCoef_d i_scale):
BxDF(ip_bxdf->GetType()), mp_bxdf(ip_bxdf), m_scale(i_scale)
  {
  ASSERT(ip_bxdf);
  }

SpectrumCoef_d ScaledBxDF::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  return m_scale * mp_bxdf->Evaluate(i_incident, i_exitant);
  }

SpectrumCoef_d ScaledBxDF::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
  {
  return m_scale * mp_bxdf->Sample(i_incident, o_exitant, i_sample, o_pdf);
  }

double ScaledBxDF::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  return mp_bxdf->PDF(i_incident, i_exitant);
  }

SpectrumCoef_d ScaledBxDF::TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const
  {
  return m_scale * mp_bxdf->TotalScattering(i_incident, i_samples);
  }

SpectrumCoef_d ScaledBxDF::TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples1, SamplesSequence2D i_samples2) const
  {
  return m_scale * mp_bxdf->TotalScattering(i_hemisphere, i_samples1, i_samples2);
  }