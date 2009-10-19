#include "DirectLightingSurfaceIntegrator.h"
#include <Math/ThreadSafeRandom.h>

DirectLightingSurfaceIntegrator::DirectLightingSurfaceIntegrator(intrusive_ptr<Renderer> ip_renderer): SurfaceIntegrator(),
mp_renderer(ip_renderer), mp_scene(ip_renderer->GetScene())
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

  const LightSources &lights = mp_scene->GetLightSources();

  // Compute lighting from delta light sources.
  for (size_t i = 0; i < lights.m_delta_light_sources.size(); ++i)
    {     
    Ray lighting_ray;
    Spectrum_d Li = lights.m_delta_light_sources[i]->Lighting(i_intersection.m_dg.m_point, lighting_ray);
    lighting_ray.m_min_t += 1e-5;

    if (Li.IsBlack())
      continue;

    Spectrum_d f = p_bsdf->Evaluate(lighting_ray.m_direction.Normalized(), i_ray.m_base_ray.m_direction*(-1.0));
    if (!f.IsBlack() && mp_scene->IntersectTest(lighting_ray)==false)
      {
      Spectrum_d transmittance = mp_renderer->Transmittance(lighting_ray, ip_sample);
      radiance += (f * Li * transmittance) * fabs(lighting_ray.m_direction.Normalized()* i_intersection.m_dg.m_shading_normal);
      }
    }

  return radiance;
  }