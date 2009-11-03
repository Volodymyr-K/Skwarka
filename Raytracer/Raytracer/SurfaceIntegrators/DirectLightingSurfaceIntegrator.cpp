#include "DirectLightingSurfaceIntegrator.h"
#include <Math/ThreadSafeRandom.h>

DirectLightingSurfaceIntegrator::DirectLightingSurfaceIntegrator(intrusive_ptr<const Renderer> ip_renderer, intrusive_ptr<DirectLightingIntegrator> ip_direct_lighting_integrator):
SurfaceIntegrator(), mp_renderer(ip_renderer), mp_scene(ip_renderer->GetScene()), mp_direct_lighting_integrator(ip_direct_lighting_integrator)
  {
  ASSERT(ip_renderer);
  ASSERT(ip_renderer->GetScene());
  }

Spectrum_d DirectLightingSurfaceIntegrator::Radiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const
  {
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());
  Spectrum_d radiance;

  BSDF *p_bsdf = i_intersection.mp_primitive->GetBSDF(i_intersection.m_dg, i_intersection.m_triangle_index, i_pool);

  if (i_intersection.mp_primitive->GetAreaLightSource())
    radiance = i_intersection.mp_primitive->GetAreaLightSource()->Radiance(i_intersection.m_dg, i_intersection.m_triangle_index, i_ray.m_base_ray.m_direction*(-1.0));

  radiance += mp_direct_lighting_integrator->ComputeDirectLighting(i_intersection, i_ray.m_base_ray.m_direction*(-1.0), p_bsdf, ip_sample, i_pool);

  return radiance;
  }

void DirectLightingSurfaceIntegrator::RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  ASSERT(ip_sampler);
  mp_direct_lighting_integrator->RequestSamples(ip_sampler);
  }