#ifndef LTE_INTEGRATOR_MOCK_H
#define LTE_INTEGRATOR_MOCK_H

#include <Common/Common.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Core/LTEIntegrator.h>
#include <Raytracer/Core/Renderer.h>
#include <Raytracer/Core/Scene.h>

/*
LTEIntegrator mock implementation.
Evaluates direct lighting only with fixed number of random samples.
*/
class LTEIntegratorMock: public LTEIntegrator
  {
  public:
    LTEIntegratorMock(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<VolumeIntegrator> ip_volume_integrator);

  private:
    virtual Spectrum_d _SurfaceRadiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const;

  private:
    intrusive_ptr<const Scene> mp_scene;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline LTEIntegratorMock::LTEIntegratorMock(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<VolumeIntegrator> ip_volume_integrator):
LTEIntegrator(ip_scene, ip_volume_integrator), mp_scene(ip_scene)
  {
  }

inline Spectrum_d LTEIntegratorMock::_SurfaceRadiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const
  {
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());
  Spectrum_d radiance;

  const BSDF *p_bsdf = i_intersection.mp_primitive->GetBSDF(i_intersection.m_dg, i_intersection.m_triangle_index, i_pool);

  const AreaLightSource *p_light_source = i_intersection.mp_primitive->GetAreaLightSource_RawPtr();
  if (p_light_source)
    radiance = p_light_source->Radiance(i_intersection.m_dg, i_intersection.m_triangle_index, i_ray.m_base_ray.m_direction*(-1.0));

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
      Spectrum_d transmittance = _VolumeTransmittance(lighting_ray, ip_sample);
      radiance += (f * Li * transmittance) * fabs(lighting_ray.m_direction.Normalized()* i_intersection.m_dg.m_shading_normal);
      }
    }
  
  // Sample BSDF to estimate lighting from infinity light sources.
  Spectrum_d infinity_light;
  for (size_t i = 0; i < 10; ++i)
    {
    Vector3D_d exitant;
    Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));
    double pdf;
    BxDFType sampled_type;
    Spectrum_d f = p_bsdf->Sample(i_ray.m_base_ray.m_direction*(-1.0), exitant, sample, RandomDouble(1.0), pdf, sampled_type);

    if (pdf>0.0)
      {
      double cs = fabs(exitant * i_intersection.m_dg.m_shading_normal);
      for(size_t j = 0; j<lights.m_infinite_light_sources.size();++j)
        {
        if (mp_scene->IntersectTest(Ray(i_intersection.m_dg.m_point, exitant, 1e-5, DBL_INF))==false)
          {
          Ray lighting_ray(i_intersection.m_dg.m_point, exitant);
          Spectrum_d Li = lights.m_infinite_light_sources[j]->Radiance(RayDifferential(lighting_ray));
          Spectrum_d transmittance = _VolumeTransmittance(lighting_ray, ip_sample);

          infinity_light += (f * Li * transmittance) * cs / pdf;
          }
        }
      }
    }

  radiance += infinity_light / 10;
  

  // Uncomment the following block of code to enable lighting from area light sources.

  /*
  // Sample BSDF to estimate lighting from area light sources.
  Spectrum_d area_light;
  for (size_t i = 0; i < 10; ++i)
    {
    Vector3D_d exitant;
    Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));
    double pdf;
    BxDFType sampled_type;
    Spectrum_d f = p_bsdf->Sample(i_ray.m_base_ray.m_direction*(-1.0), exitant, sample, RandomDouble(1.0), pdf, sampled_type);

    if (pdf>0.0)
      {
      double cs = fabs(exitant * i_intersection.m_dg.m_shading_normal);
      Intersection isect2;
      if (mp_scene->Intersect(RayDifferential(Ray(i_intersection.m_dg.m_point, exitant, 1e-5, DBL_INF)), isect2) && isect2.mp_primitive->GetAreaLightSource())
        {
        area_light += f*isect2.mp_primitive->GetAreaLightSource()->Radiance(isect2.m_dg, isect2.m_triangle_index, exitant*(-1.0)) * cs / pdf;
        }
      }
    }

  radiance += area_light / 10;
  */

  return radiance;
  }

#endif // LTE_INTEGRATOR_MOCK_H