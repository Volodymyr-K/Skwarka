#include "LTEIntegrator.h"
#include "CoreUtils.h"

LTEIntegrator::LTEIntegrator(intrusive_ptr<const Scene> ip_scene):
mp_scene(ip_scene)
  {
  ASSERT(ip_scene);
  }

LTEIntegrator::~LTEIntegrator()
  {
  }

Spectrum_d LTEIntegrator::Radiance(const RayDifferential &i_ray, const Sample *ip_sample, ThreadSpecifics i_ts) const
  {
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());

  Intersection isect;
  double intersection_t;
  bool hit=mp_scene->Intersect(i_ray, isect, &intersection_t);

  RayDifferential intersection_ray(i_ray);
  Spectrum_d radiance;
  if (hit)
    {
    radiance = this->_SurfaceRadiance(i_ray, isect, ip_sample, i_ts);
    intersection_ray.m_base_ray.m_max_t=intersection_t;
    }
  else
    if (IsInf(i_ray.m_base_ray.m_max_t)) // Check if the ray is unbounded.
      {
      // Add contribution of infinite light sources for an unbounded ray that does not intersect any primitive.
      const LightSources &lights = mp_scene->GetLightSources();
      for (size_t i = 0; i < lights.m_infinite_light_sources.size(); ++i)
        radiance += lights.m_infinite_light_sources[i]->Radiance(i_ray);
      }

  SpectrumCoef_d transmittance(1.0);
  Spectrum_d volume_radiance = _MediaRadianceAndTranmsittance(intersection_ray, ip_sample, transmittance, i_ts);

  return radiance * transmittance + volume_radiance;
  }

void LTEIntegrator::RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  ASSERT(ip_sampler);

  this->_RequestSamples(ip_sampler);
  }

Spectrum_d LTEIntegrator::_SpecularReflect(const RayDifferential &i_ray, const Intersection &i_intersection, const BSDF *ip_bsdf, const Sample *ip_sample, ThreadSpecifics i_ts) const
  {
  ASSERT(ip_bsdf);
  const DifferentialGeometry &dg = i_intersection.m_dg;

  double pdf;
  BxDFType sampled_type;
  Vector3D_d exitant;
  Vector3D_d incident = i_ray.m_base_ray.m_direction*(-1.0);
  ASSERT(incident.IsNormalized());

  SpectrumCoef_d brdf = ip_bsdf->Sample(incident, exitant, pdf, sampled_type, BxDFType(BSDF_REFLECTION | BSDF_SPECULAR));
  ASSERT(sampled_type==BxDFType(BSDF_REFLECTION | BSDF_SPECULAR) || sampled_type==BSDF_NONE);

  if (brdf.IsBlack()==false && fabs(exitant*dg.m_shading_normal)>DBL_EPS)
    {
    RayDifferential rd( Ray(dg.m_point, exitant, CoreUtils::GetNextMinT(i_intersection, exitant)) );
    rd.m_specular_depth = i_ray.m_specular_depth + 1;

    CoreUtils::SetReflectedDifferentials(i_ray, dg, rd);

    // No need to multiply by the cosine value because the specular BRDF already accounts for that.
    return this->Radiance(rd, ip_sample, i_ts) * brdf;
    }
  else
    return Spectrum_d();
  }

Spectrum_d LTEIntegrator::_SpecularTransmit(const RayDifferential &i_ray, const Intersection &i_intersection, const BSDF *ip_bsdf, const Sample *ip_sample, ThreadSpecifics i_ts) const
  {
  ASSERT(ip_bsdf);
  const DifferentialGeometry &dg = i_intersection.m_dg;

  double pdf;
  BxDFType sampled_type;
  Vector3D_d exitant;
  Vector3D_d incident = i_ray.m_base_ray.m_direction*(-1.0);
  ASSERT(incident.IsNormalized());

  // Check if the incident ray is perpendicular to the normal, in that case we return zero spectrum to avoid numerical errors in SpecularTransmission::Sample() method.
  if (fabs(incident*dg.m_shading_normal)<DBL_EPS)
    return Spectrum_d();

  SpectrumCoef_d btdf = ip_bsdf->Sample(incident, exitant, pdf, sampled_type, BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR));
  ASSERT(sampled_type==BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR) || sampled_type==BSDF_NONE);

  if (btdf.IsBlack()==false && fabs(exitant*dg.m_shading_normal)>DBL_EPS)
    {
    RayDifferential rd( Ray(dg.m_point, exitant, CoreUtils::GetNextMinT(i_intersection, exitant)) );
    rd.m_specular_depth = i_ray.m_specular_depth + 1;

    double refractive_index = ip_bsdf->GetRefractiveIndex();
    CoreUtils::SetTransmittedDifferentials(i_ray, dg, refractive_index, rd);

    double eta = (incident*dg.m_shading_normal > 0.0) ? 1.0 / refractive_index : refractive_index;

    // No need to multiply by the cosine value because the specular BTDF already accounts for that.
    return this->Radiance(rd, ip_sample, i_ts) * btdf * (eta*eta);
    }
  else
    return Spectrum_d();
  }

void LTEIntegrator::_RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  // No samples are requested by default.
  }