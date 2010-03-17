#include "DirectLightingLTEIntegrator.h"
#include <Math/ThreadSafeRandom.h>

DirectLightingLTEIntegrator::DirectLightingLTEIntegrator(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<VolumeIntegrator> ip_volume_integrator, DirectLightingLTEIntegratorParams i_params):
LTEIntegrator(ip_scene, ip_volume_integrator), m_params(i_params)
  {
  ASSERT(ip_scene);

  mp_direct_lighting_integrator.reset(new DirectLightingIntegrator(ip_scene, ip_volume_integrator, i_params.m_direct_light_samples_num, i_params.m_direct_light_samples_num));

  if (m_params.m_max_specular_depth > 50)
    m_params.m_max_specular_depth = 50;
  }

Spectrum_d DirectLightingLTEIntegrator::_SurfaceRadiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const
  {
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());
  Spectrum_d radiance;

  const BSDF *p_bsdf = i_intersection.mp_primitive->GetBSDF(i_intersection.m_dg, i_intersection.m_triangle_index, i_pool);

  // Add emitting lighting from the surface (if the surface has light source properties).
  const AreaLightSource *p_light_source = i_intersection.mp_primitive->GetAreaLightSource_RawPtr();
  if (p_light_source)
    radiance = p_light_source->Radiance(i_intersection.m_dg, i_intersection.m_triangle_index, i_ray.m_base_ray.m_direction*(-1.0));

  // Compute direct lighting.
  bool has_non_specular = p_bsdf->GetComponentsNum( BxDFType(BSDF_ALL & ~BSDF_SPECULAR) ) > 0;
  if (has_non_specular)
    radiance += mp_direct_lighting_integrator->ComputeDirectLighting(i_intersection, i_ray.m_base_ray.m_direction*(-1.0), p_bsdf, ip_sample, i_pool);

  // Trace rays for specular reflection and refraction.
  if (i_ray.m_specular_depth <= m_params.m_max_specular_depth)
    {
    radiance += _SpecularReflect(i_ray, i_intersection, p_bsdf, ip_sample, i_pool);
    radiance += _SpecularTransmit(i_ray, i_intersection, p_bsdf, ip_sample, i_pool);
    }

  return radiance;
  }

void DirectLightingLTEIntegrator::_RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  ASSERT(ip_sampler);
  mp_direct_lighting_integrator->RequestSamples(ip_sampler);
  }