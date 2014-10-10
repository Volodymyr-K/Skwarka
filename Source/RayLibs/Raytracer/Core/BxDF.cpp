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

#include "BxDF.h"
#include <Math/Constants.h>
#include <Math/MathRoutines.h>
#include <Math/SamplingRoutines.h>

SpectrumCoef_d BxDF::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
  {
  ASSERT(i_sample[0]>=0.0 && i_sample[0]<1.0);
  ASSERT(i_sample[1]>=0.0 && i_sample[1]<1.0);
  ASSERT(i_incident.IsNormalized());
  o_exitant=SamplingRoutines::CosineHemisphereSampling(i_sample);

  // Flip Z coordinate of the exitant direction if necessary to match the correct hemisphere.
  if ( (i_incident[2] < 0.0 && IsReflection(m_type)) || (i_incident[2] > 0.0 && IsTransmission(m_type)) )
    o_exitant[2] *= -1.0;

  ASSERT(o_exitant.IsNormalized());

  o_pdf=this->PDF(i_incident, o_exitant);
  return this->Evaluate(i_incident, o_exitant);
  }

double BxDF::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
  {
  bool same_hemisphere = i_incident[2]*i_exitant[2] > 0.0;
  return same_hemisphere == IsReflection(m_type) ? fabs(i_exitant[2]) * INV_PI : 0.0;
  }

SpectrumCoef_d BxDF::TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const
  {
  size_t num_samples = std::distance(i_samples.m_begin, i_samples.m_end);
  ASSERT(num_samples > 0);
  ASSERT(i_incident.IsNormalized());

  SpectrumCoef_d ret;
  for(SamplesSequence2D::Iterator it=i_samples.m_begin;it!=i_samples.m_end;++it)
    {
    Point2D_d sample = *it;

    double pdf_exitant=0.0;
    Vector3D_d exitant;
    SpectrumCoef_d tmp = this->Sample(i_incident, exitant, sample, pdf_exitant);
    ASSERT(exitant.IsNormalized());
    ASSERT(pdf_exitant >= 0.0);

    if (pdf_exitant > 0.0)
      ret.AddWeighted(tmp, fabs(exitant[2]) / pdf_exitant);
    }

  ret/=num_samples;

  // Clamp spectrum values because a surface can not physically scatter more light than it received.
  ret.Clamp(0.0,1.0);
  return ret;
  }

SpectrumCoef_d BxDF::TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples1, SamplesSequence2D i_samples2) const
  {
  size_t num_samples = std::distance(i_samples1.m_begin, i_samples1.m_end);
  ASSERT(std::distance(i_samples2.m_begin, i_samples2.m_end) == num_samples);
  ASSERT(num_samples > 0);

  double Z_sign = i_hemisphere ? 1.0 : -1.0;

  SpectrumCoef_d ret;
  SamplesSequence2D::Iterator it1=i_samples1.m_begin;
  SamplesSequence2D::Iterator it2=i_samples2.m_begin;
  for(size_t i=0;i<num_samples;++i)
    {
    Point2D_d sample_incident = *(it1++);
    Point2D_d sample_exitant = *(it2++);

    Vector3D_d incident = SamplingRoutines::UniformHemisphereSampling(sample_incident);

    // Flip the hemisphere if needed.
    incident[2] *= Z_sign;
    ASSERT(incident.IsNormalized());

    Vector3D_d exitant;
    double pdf_exitant=0.0;
    SpectrumCoef_d tmp = this->Sample(incident, exitant, sample_exitant, pdf_exitant);
    ASSERT(exitant.IsNormalized());
    ASSERT(pdf_exitant >= 0.0);

    if (pdf_exitant > 0.0)
      ret.AddWeighted(tmp, fabs(incident[2]*exitant[2]) / pdf_exitant );
    }

  ret*=2.0/num_samples;

  // Clamp spectrum values because a surface can not physically scatter more light than it received.
  ret.Clamp(0.0,1.0);
  return ret;
  }