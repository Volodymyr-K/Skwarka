#include "BSDF.h"
#include "Sample.h"
#include <Math/ThreadSafeRandom.h>
#include <Math/SamplingRoutines.h>
#include <vector>

Spectrum_d BSDF::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant, BxDFType i_flags) const
  {
  Vector3D_d incident_local = WorldToLocal(i_incident), exitant_local = WorldToLocal(i_exitant);
  ASSERT(incident_local.IsNormalized());
  ASSERT(exitant_local.IsNormalized());

  // To avoid "light leaks" and "dark spots" due to the difference between geometric and shading normal
  // we evaluate only BxDFs of the types corresponding to the hemisphere defined by the geometric normal at the surface point.
  if ((i_incident*m_geometric_normal) * (i_exitant*m_geometric_normal) > 0)
    i_flags = BxDFType(i_flags & ~BSDF_TRANSMISSION); // Ignore BTDFs.
  else
    i_flags = BxDFType(i_flags & ~BSDF_REFLECTION); // Ignore BRDFs.

  Spectrum_d ret;
  for(size_t i=0;i<m_BxDFs_num;++i)
    if (m_BxDFs[i]->MatchesFlags(i_flags))
      {
      Spectrum_d tmp=m_BxDFs[i]->Evaluate(incident_local, exitant_local);
      ret+=tmp;

      // Assert that all specular BxDFs always return zero value.
      ASSERT(IsSpecular(m_BxDFs[i]->GetType())==false || tmp.IsBlack());
      }

  return ret;
  }

double BSDF::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant, BxDFType i_flags) const
  {
  if (m_BxDFs_num == 0)
    return 0.0;

  Vector3D_d incident_local = WorldToLocal(i_incident), exitant_local = WorldToLocal(i_exitant);
  ASSERT(incident_local.IsNormalized());
  ASSERT(exitant_local.IsNormalized());

  double pdf = 0.0;
  int num_matched = 0;
  for (size_t i=0;i<m_BxDFs_num;++i)
    if (m_BxDFs[i]->MatchesFlags(i_flags))
      {
      ++num_matched;

      double tmp=m_BxDFs[i]->PDF(incident_local, exitant_local);
      pdf += tmp;
      
      // Assert that all specular BxDFs always return zero PDF.
      ASSERT(IsSpecular(m_BxDFs[i]->GetType())==false || tmp==0.0);
      }

  return num_matched > 0 ? pdf/num_matched : 0.0;
  }

Spectrum_d BSDF::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant,
                               const Point2D_d &i_sample, double i_component_sample, double &o_pdf, BxDFType &o_sampled_type, BxDFType i_flags) const
  {
  ASSERT(i_sample[0]>=0.0 && i_sample[0]<1.0);
  ASSERT(i_sample[1]>=0.0 && i_sample[1]<1.0);
  ASSERT(i_component_sample>=0.0 && i_component_sample<1.0);

  size_t num_matched=GetComponentsNum(i_flags);

  if (num_matched == 0)
    {
    o_sampled_type=BSDF_NONE;
    o_pdf=0.0;
    return Spectrum_d(0.0);
    }

  // First, select a component to be sampled.
  BxDF *bxdf = NULL;
  size_t samled_component=(size_t)(i_component_sample*num_matched);
  for(size_t i=0;i<m_BxDFs_num;++i)
    if (m_BxDFs[i]->MatchesFlags(i_flags) && samled_component==0)
      {
      bxdf=m_BxDFs[i];
      break;
      }
    else
      --samled_component;

  ASSERT(bxdf != NULL);
  o_sampled_type = bxdf->GetType();

  Vector3D_d incident_local = WorldToLocal(i_incident);
  ASSERT(incident_local.IsNormalized());

  // Sample the selected component to get the exitant direction.
  o_pdf=0.0;
  Vector3D_d exitant_local;
  Spectrum_d ret_value = bxdf->Sample(incident_local, exitant_local, i_sample, o_pdf);
  
  ASSERT(o_pdf>0.0);

  o_exitant=LocalToWorld(exitant_local);
  ASSERT(o_exitant.IsNormalized());

  // If sampled component is not specular add PDF values for other components.
  if (num_matched>1 && IsSpecular(o_sampled_type)==false)
    for(size_t i=0;i<m_BxDFs_num;++i)
      if (m_BxDFs[i]!=bxdf && m_BxDFs[i]->MatchesFlags(i_flags))
        o_pdf+=m_BxDFs[i]->PDF(incident_local, exitant_local);

  if (num_matched>1)
    o_pdf/=num_matched;

  // If sampled component is not specular compute BxDF values for all matching components.
  if (IsSpecular(o_sampled_type)==false)
    {
    ret_value=Spectrum_d(0.0);

    // To avoid "light leaks" and "dark spots" due to the difference between geometric and shading normal
    // we evaluate only BxDFs of the types corresponding to the hemisphere defined by the geometric normal at the surface point.
  if ((i_incident*m_geometric_normal) * (o_exitant*m_geometric_normal) > 0)
      i_flags = BxDFType(i_flags & ~BSDF_TRANSMISSION); // Ignore BTDFs.
    else
      i_flags = BxDFType(i_flags & ~BSDF_REFLECTION); // Ignore BRDFs.

    for(size_t i=0;i<m_BxDFs_num;++i)
      if (m_BxDFs[i]->MatchesFlags(i_flags))
        ret_value+=m_BxDFs[i]->Evaluate(incident_local, exitant_local);
    }

  return ret_value;
  }

Spectrum_d BSDF::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant,
                        double &o_pdf, BxDFType &o_sampled_type, BxDFType i_flags) const
  {
  Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));
  return Sample(i_incident, o_exitant, sample, RandomDouble(1.0), o_pdf, o_sampled_type, i_flags);
  }

Spectrum_d BSDF::TotalScattering(const Vector3D_d &i_incident, size_t i_samples_num, BxDFType i_flags) const
  {
  ASSERT(i_samples_num>0);
  Vector3D_d incident_local = WorldToLocal(i_incident);
  ASSERT(incident_local.IsNormalized());

  std::vector<Point2D_d> samples(i_samples_num);
  SamplesSequence2D sequence(samples.begin(), samples.end());

  Spectrum_d ret;
  for(size_t i=0;i<m_BxDFs_num;++i)
    if (m_BxDFs[i]->MatchesFlags(i_flags))
      {
      SamplingRoutines::LatinHypercubeSampling2D(sequence.m_begin, i_samples_num, true);
      ret+=m_BxDFs[i]->TotalScattering(i_incident, sequence);
      }

  // Clamp spectrum values because a surface can not physically scatter more light than it received.
  ret[0]=MathRoutines::Clamp(ret[0],0.0,1.0);
  ret[1]=MathRoutines::Clamp(ret[1],0.0,1.0);
  ret[2]=MathRoutines::Clamp(ret[2],0.0,1.0);
  return ret;
  }

Spectrum_d BSDF::TotalScattering(size_t i_samples_num, BxDFType i_flags) const
  {
  ASSERT(i_samples_num>0);

  // Twice the number of samples needed for this version of TotalScattering() method because two 2D samples are needed for one sample of the integral.
  // For the details refer to BxDF::TotalScattering(SamplesSequence2D) method.
  std::vector<Point2D_d> samples(2*i_samples_num);
  SamplesSequence2D sequence(samples.begin(), samples.end());

  Spectrum_d ret;
  for(size_t i=0;i<m_BxDFs_num;++i)
    if (m_BxDFs[i]->MatchesFlags(i_flags))
      {
      SamplingRoutines::LatinHypercubeSampling2D(sequence.m_begin, 2*i_samples_num, true);
      ret+=m_BxDFs[i]->TotalScattering(sequence);
      }

  // Clamp spectrum values because a surface can not physically scatter more light than it received.
  ret[0]=MathRoutines::Clamp(ret[0],0.0,1.0);
  ret[1]=MathRoutines::Clamp(ret[1],0.0,1.0);
  ret[2]=MathRoutines::Clamp(ret[2],0.0,1.0);
  return ret;
  }