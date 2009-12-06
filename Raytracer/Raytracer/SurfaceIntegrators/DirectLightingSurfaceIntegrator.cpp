#include "DirectLightingSurfaceIntegrator.h"
#include <Math/ThreadSafeRandom.h>

DirectLightingSurfaceIntegrator::DirectLightingSurfaceIntegrator(intrusive_ptr<const Renderer> ip_renderer,
                                                                 intrusive_ptr<DirectLightingIntegrator> ip_direct_lighting_integrator,
                                                                 size_t i_min_specular_depth, double i_stop_probability):
SurfaceIntegrator(ip_renderer), mp_direct_lighting_integrator(ip_direct_lighting_integrator),
m_min_specular_depth(i_min_specular_depth), m_stop_probability(i_stop_probability)
  {
  ASSERT(ip_direct_lighting_integrator);
  ASSERT(i_stop_probability>0.0 && i_stop_probability<=1.0);

  m_stop_probability = std::min(m_stop_probability, 1.0);
  m_stop_probability = std::max(m_stop_probability, 0.01);
  }

Spectrum_d DirectLightingSurfaceIntegrator::Radiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const
  {
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());
  Spectrum_d radiance;

  const BSDF *p_bsdf = i_intersection.mp_primitive->GetBSDF(i_intersection.m_dg, i_intersection.m_triangle_index, i_pool);

  const AreaLightSource *p_light_source = i_intersection.mp_primitive->GetAreaLightSource_RawPtr();
  if (p_light_source)
    radiance = p_light_source->Radiance(i_intersection.m_dg, i_intersection.m_triangle_index, i_ray.m_base_ray.m_direction*(-1.0));

  radiance += mp_direct_lighting_integrator->ComputeDirectLighting(i_intersection, i_ray.m_base_ray.m_direction*(-1.0), p_bsdf, ip_sample, i_pool);

  // Use Russian Roulette to determine if we need to trace specular rays.
  // Russian Roulette is only used if the ray's specular depth is greater or equal than m_min_specular_depth.
  bool trace_specular_rays = false;
  double inv_trace_probability = 1.0;
  if (i_ray.m_specular_depth < m_min_specular_depth)
    trace_specular_rays = true;
  else
    if (RandomDouble(1.0) > m_stop_probability)
      {
      trace_specular_rays = true;
      inv_trace_probability = 1.0 / (1.0-m_stop_probability); // Can be optimized by precomputing this.
      }

  if (trace_specular_rays)
    {
    // Trace rays for specular reflection and refraction.
    radiance.AddWeighted(_SpecularReflect(i_ray, i_intersection, p_bsdf, ip_sample, i_pool), inv_trace_probability);
    radiance.AddWeighted(_SpecularTransmit(i_ray, i_intersection, p_bsdf, ip_sample, i_pool), inv_trace_probability);
    }

  return radiance;
  }

void DirectLightingSurfaceIntegrator::RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  ASSERT(ip_sampler);
  mp_direct_lighting_integrator->RequestSamples(ip_sampler);
  }