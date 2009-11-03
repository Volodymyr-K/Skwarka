#ifndef RENDERER_MOCK_H
#define RENDERER_MOCK_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Renderer.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Core/Sampler.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/Core/Scene.h>

/*
Renderer mock implementation. Does not render anything. Returns black radiance. Returns full transmittance (as if no media was in the scene).
*/
class RendererMock: public Renderer
  {
  public:
    RendererMock(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<Sampler> ip_sampler);

    virtual void Render(intrusive_ptr<const Camera> ip_camera) const;

    virtual Spectrum_d Radiance(const RayDifferential &i_ray, const Sample *ip_sample, MemoryPool &i_pool) const;

    virtual Spectrum_d Transmittance(const Ray &i_ray, const Sample *ip_sample) const;

  private:
    intrusive_ptr<const Scene> mp_scene;

    intrusive_ptr<const Sampler> mp_sampler;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline RendererMock::RendererMock(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<Sampler> ip_sampler): Renderer(ip_scene),
mp_scene(ip_scene), mp_sampler(ip_sampler)
  {
  ASSERT(ip_scene);
  ASSERT(ip_sampler);
  }

inline void RendererMock::Render(intrusive_ptr<const Camera> ip_camera) const
  {
  ASSERT(ip_camera);
  }

inline Spectrum_d RendererMock::Radiance(const RayDifferential &i_ray, const Sample *ip_sample, MemoryPool &i_pool) const
  {
  return Spectrum_d(0.0);
  }

inline Spectrum_d RendererMock::Transmittance(const Ray &i_ray, const Sample *ip_sample) const
  {
  return Spectrum_d(1.0);
  }

#endif // RENDERER_MOCK_H