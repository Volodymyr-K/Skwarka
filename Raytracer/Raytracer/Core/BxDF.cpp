#include "BxDF.h"
#include <Math/Constants.h>
#include <Math/Util.h>
#include <Math/SamplingRoutines.h>

Spectrum_d BxDF::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
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

Spectrum_d BxDF::TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const
  {
  size_t num_samples = std::distance(i_samples.m_begin, i_samples.m_end);
  ASSERT(num_samples > 0);
  ASSERT(i_incident.IsNormalized());

  Spectrum_d ret;
  for(SamplesSequence2D::Iterator it=i_samples.m_begin;it!=i_samples.m_end;++it)
    {
    Point2D_d sample = *it;

    double pdf_exitant=0.0;
    Vector3D_d exitant;
    Spectrum_d tmp = this->Sample(i_incident, exitant, sample, pdf_exitant);
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

Spectrum_d BxDF::TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples) const
  {
  size_t num_sample_pairs = std::distance(i_samples.m_begin, i_samples.m_end) / 2;
  ASSERT(num_sample_pairs > 0);

  double Z_sign = i_hemisphere ? 1.0 : -1.0;

  Spectrum_d ret;
  SamplesSequence2D::Iterator it=i_samples.m_begin;
  for(size_t i=0;i<num_sample_pairs;++i)
    {
    Point2D_d sample_incident = *(it++);
    Point2D_d sample_exitant = *(it++);

    Vector3D_d incident = SamplingRoutines::UniformHemisphereSampling(sample_incident);

    // Flip the hemisphere if needed.
    incident[2] *= Z_sign;
    ASSERT(incident.IsNormalized());

    Vector3D_d exitant;
    double pdf_exitant=0.0;
    Spectrum_d tmp = this->Sample(incident, exitant, sample_exitant, pdf_exitant);
    ASSERT(exitant.IsNormalized());
    ASSERT(pdf_exitant >= 0.0);

    if (pdf_exitant > 0.0)
      ret.AddWeighted(tmp, fabs(incident[2]*exitant[2]) / pdf_exitant );
    }

  ret*=2.0/num_sample_pairs;

  // Clamp spectrum values because a surface can not physically scatter more light than it received.
  ret.Clamp(0.0,1.0);
  return ret;
  }