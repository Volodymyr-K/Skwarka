#include "BSDF.h"
#include "Sample.h"
#include "SpectrumRoutines.h"
#include <Math/ThreadSafeRandom.h>
#include <Math/SamplingRoutines.h>
#include <vector>

SpectrumCoef_d BSDF::Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant, BxDFType i_flags) const
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

  SpectrumCoef_d ret;
  for(size_t i=0;i<m_BxDFs_num;++i)
    if (m_BxDFs[i]->MatchesFlags(i_flags))
      {
      SpectrumCoef_d tmp=m_BxDFs[i]->Evaluate(incident_local, exitant_local);
      ret+=tmp;

      // Assert that all specular BxDFs always return zero value.
      ASSERT(IsSpecular(m_BxDFs[i]->GetType())==false || tmp.IsBlack());
      }

  return ret;
  }

double BSDF::PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant, BxDFType i_flags) const
  {
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

/**
* The method samples only specular components of the BSDF.
* The method first computes BxDF values for all specular components and then selects one of these components based on the luminance values of the BxDFs.
*/
SpectrumCoef_d BSDF::_SampleSpecularOnly(const Vector3D_d &i_incident, Vector3D_d &o_exitant,
                                         const Point2D_d &i_sample, double i_component_sample, double &o_pdf, BxDFType &o_sampled_type, BxDFType i_flags) const
  {
  SpectrumCoef_d bsdfs[MAX_BXDFS_NUM];
  Vector3D_d exitants[MAX_BXDFS_NUM];
  double pdfs[MAX_BXDFS_NUM];
  double luminances[MAX_BXDFS_NUM];
  unsigned char indices[MAX_BXDFS_NUM];

  Vector3D_d incident_local = WorldToLocal(i_incident);
  ASSERT(incident_local.IsNormalized());

  // Sample all BxDFs and compute their luminances.
  size_t num_matched = 0;
  double luminance_sum = 0;
  for(size_t i=0;i<m_BxDFs_num;++i)
    if (m_BxDFs[i]->MatchesFlags(i_flags))
      {
      ASSERT(IsSpecular(m_BxDFs[i]->GetType()));

      bsdfs[num_matched] = m_BxDFs[i]->Sample(incident_local, exitants[num_matched], i_sample, pdfs[num_matched]);
      indices[num_matched] = i;
      luminances[num_matched] = SpectrumRoutines::Luminance(bsdfs[num_matched]);
      luminance_sum += luminances[num_matched];

      ++num_matched;
      }

  ASSERT(num_matched > 1);

  // Select the BxDF component.
  // The components are selected based on their luminances.
  i_component_sample *= luminance_sum;
  for(size_t i=0;i<num_matched;++i)
    if (i_component_sample * 0.999 < luminances[i]) // multiply by 0.999 to avoid numerical errors
      {
      o_sampled_type = m_BxDFs[indices[i]]->GetType();
      o_pdf = pdfs[i] * (luminances[i] / luminance_sum);
      o_exitant = LocalToWorld(exitants[i]);
      ASSERT(o_exitant.IsNormalized());

      return bsdfs[i];
      }
    else
      i_component_sample -= luminances[i];

  ASSERT(0 && "We should not have got here.");
  return SpectrumCoef_d();
  }

SpectrumCoef_d BSDF::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant,
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
    return SpectrumCoef_d(0.0);
    }

  // If only specular components are being sampled and there is more than one matched specular component the BxDF to be sampled is determined based on BxDF's luminance values.
  BxDFType specular_types = BxDFType(i_flags & (BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION));
  if (num_matched>1 && GetComponentsNum(specular_types)==num_matched)
    return _SampleSpecularOnly(i_incident, o_exitant, i_sample, i_component_sample, o_pdf, o_sampled_type, i_flags);

  // First, select a component to be sampled.
  const BxDF *bxdf = NULL;
  size_t sampled_component=std::min((size_t)(i_component_sample*num_matched), num_matched-1);
  for(size_t i=0;i<m_BxDFs_num;++i)
    if (m_BxDFs[i]->MatchesFlags(i_flags) && sampled_component-- == 0)
      {
      bxdf=m_BxDFs[i];
      break;
      }

  ASSERT(bxdf != NULL);
  o_sampled_type = bxdf->GetType();

  Vector3D_d incident_local = WorldToLocal(i_incident);
  ASSERT(incident_local.IsNormalized());

  // Sample the selected component to get the exitant direction.
  o_pdf=0.0;
  Vector3D_d exitant_local;
  SpectrumCoef_d ret_value = bxdf->Sample(incident_local, exitant_local, i_sample, o_pdf);
  
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

  // If sampled component is not specular, compute BxDF values for all matching components.
  if (IsSpecular(o_sampled_type)==false)
    {
    ret_value=SpectrumCoef_d(0.0);

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

SpectrumCoef_d BSDF::Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant,
                            double &o_pdf, BxDFType &o_sampled_type, BxDFType i_flags) const
  {
  Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));
  return Sample(i_incident, o_exitant, sample, RandomDouble(1.0), o_pdf, o_sampled_type, i_flags);
  }

SpectrumCoef_d BSDF::TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples, BxDFType i_flags) const
  {
  ASSERT(std::distance(i_samples.m_begin, i_samples.m_end) > 0);

  Vector3D_d incident_local = WorldToLocal(i_incident);
  ASSERT(incident_local.IsNormalized());

  SpectrumCoef_d ret;
  for(size_t i=0;i<m_BxDFs_num;++i)
    if (m_BxDFs[i]->MatchesFlags(i_flags))
      ret+=m_BxDFs[i]->TotalScattering(i_incident, i_samples);

  // Clamp spectrum values because a surface can not physically scatter more light than it received.
  ret.Clamp(0.0,1.0);
  return ret;
  }

SpectrumCoef_d BSDF::TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples1, SamplesSequence2D i_samples2, BxDFType i_flags) const
  {
  ASSERT(std::distance(i_samples1.m_begin, i_samples1.m_end) > 0);
  ASSERT(std::distance(i_samples1.m_begin, i_samples1.m_end) == std::distance(i_samples2.m_begin, i_samples2.m_end));

  SpectrumCoef_d ret;
  for(size_t i=0;i<m_BxDFs_num;++i)
    if (m_BxDFs[i]->MatchesFlags(i_flags))      
      ret+=m_BxDFs[i]->TotalScattering(i_hemisphere, i_samples1, i_samples2);

  // Clamp spectrum values because a surface can not physically scatter more light than it received.
  ret.Clamp(0.0,1.0);
  return ret;
  }