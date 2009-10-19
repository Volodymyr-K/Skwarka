#ifndef DIRECT_LIGHTING_SURFACE_INTEGRATOR_H
#define DIRECT_LIGHTING_SURFACE_INTEGRATOR_H

#include <Common/Common.h>
#include <Raytracer/Core/SurfaceIntegrator.h>
#include <Raytracer/Core/Renderer.h>

class DirectLightingSurfaceIntegrator: public SurfaceIntegrator
  {
  public:
    DirectLightingSurfaceIntegrator(intrusive_ptr<Renderer> ip_renderer);

    virtual Spectrum_d Radiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const;

  private:
    intrusive_ptr<Scene> mp_scene;
    intrusive_ptr<Renderer> mp_renderer;
  };

#endif // DIRECT_LIGHTING_SURFACE_INTEGRATOR_H