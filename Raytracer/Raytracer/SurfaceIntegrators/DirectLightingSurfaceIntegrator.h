#ifndef DIRECT_LIGHTING_SURFACE_INTEGRATOR_H
#define DIRECT_LIGHTING_SURFACE_INTEGRATOR_H

#include <Common/Common.h>
#include <Raytracer/Core/SurfaceIntegrator.h>
#include <Raytracer/Core/Renderer.h>
#include <Raytracer/Core/DirectLightingIntegrator.h>

class DirectLightingSurfaceIntegrator: public SurfaceIntegrator
  {
  public:
    DirectLightingSurfaceIntegrator(intrusive_ptr<const Renderer> ip_renderer, intrusive_ptr<DirectLightingIntegrator> ip_direct_lighting_integrator);

    virtual Spectrum_d Radiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const;

    virtual void RequestSamples(intrusive_ptr<Sampler> ip_sampler);

  private:
    intrusive_ptr<const Scene> mp_scene;
    intrusive_ptr<const Renderer> mp_renderer;
    intrusive_ptr<DirectLightingIntegrator> mp_direct_lighting_integrator;
  };

#endif // DIRECT_LIGHTING_SURFACE_INTEGRATOR_H