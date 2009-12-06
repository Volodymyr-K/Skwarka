#include "SurfaceIntegrator.h"
#include "CoreUtils.h"

SurfaceIntegrator::SurfaceIntegrator(intrusive_ptr<const Renderer> ip_renderer): mp_renderer(ip_renderer)
  {
  ASSERT(ip_renderer);
  }

SurfaceIntegrator::~SurfaceIntegrator()
  {
  }

void SurfaceIntegrator::RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  }

Spectrum_d SurfaceIntegrator::_SpecularReflect(const RayDifferential &i_ray, const Intersection &i_intersection, const BSDF *ip_bsdf, const Sample *ip_sample, MemoryPool &i_pool) const
  {
  ASSERT(ip_bsdf);
  const DifferentialGeometry &dg = i_intersection.m_dg;

  double pdf;
  BxDFType sampled_type;
  Vector3D_d exitant;
  Vector3D_d incident = i_ray.m_base_ray.m_direction*(-1.0);
  ASSERT(incident.IsNormalized());

  Spectrum_d brdf = ip_bsdf->Sample(incident, exitant, pdf, sampled_type, BxDFType(BSDF_REFLECTION | BSDF_SPECULAR));
  ASSERT(sampled_type==BxDFType(BSDF_REFLECTION | BSDF_SPECULAR) || sampled_type==BSDF_NONE);

  if (brdf.IsBlack()==false && fabs(exitant*dg.m_shading_normal)>DBL_EPS)
    {
    RayDifferential rd( Ray(dg.m_point, exitant, 1e-5) ); // TBD: fix this
    rd.m_specular_depth = i_ray.m_specular_depth + 1;

    CoreUtils::SetReflectedDifferentials(i_ray, dg, rd);

    return mp_renderer->Radiance(rd, ip_sample, i_pool) * brdf * fabs(incident*dg.m_shading_normal);
    }
  else
    return Spectrum_d();
  }

Spectrum_d SurfaceIntegrator::_SpecularTransmit(const RayDifferential &i_ray, const Intersection &i_intersection, const BSDF *ip_bsdf, const Sample *ip_sample, MemoryPool &i_pool) const
  {
  ASSERT(ip_bsdf);
  const DifferentialGeometry &dg = i_intersection.m_dg;

  double pdf;
  BxDFType sampled_type;
  Vector3D_d exitant;
  Vector3D_d incident = i_ray.m_base_ray.m_direction*(-1.0);
  ASSERT(incident.IsNormalized());

  Spectrum_d btdf = ip_bsdf->Sample(incident, exitant, pdf, sampled_type, BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR));
  ASSERT(sampled_type==BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR) || sampled_type==BSDF_NONE);

  if (btdf.IsBlack()==false && fabs(exitant*dg.m_shading_normal)>DBL_EPS)
    {
    RayDifferential rd( Ray(dg.m_point, exitant, 1e-5) ); // TBD: fix this
    rd.m_specular_depth = i_ray.m_specular_depth + 1;

    CoreUtils::SetTransmittedDifferentials(i_ray, dg, ip_bsdf->GetRefractiveIndex(), rd);

    return mp_renderer->Radiance(rd, ip_sample, i_pool) * btdf * fabs(incident*dg.m_shading_normal);
    }
  else
    return Spectrum_d();
  }