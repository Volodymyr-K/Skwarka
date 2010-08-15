#include "DirectLightingLTEIntegrator.h"
#include <Raytracer/Core/SpectrumRoutines.h>
#include <Math/ThreadSafeRandom.h>
#include <Math/SamplingRoutines.h>

DirectLightingLTEIntegrator::DirectLightingLTEIntegrator(intrusive_ptr<const Scene> ip_scene, DirectLightingLTEIntegratorParams i_params):
LTEIntegrator(ip_scene), mp_scene(ip_scene), m_params(i_params)
  {
  ASSERT(ip_scene);

  // We double the media step size for secondary rays to reduce computation time (since the accuracy is usually less important for such rays).
  mp_direct_lighting_integrator.reset(new DirectLightingIntegrator(ip_scene, i_params.m_direct_light_samples_num,
    i_params.m_direct_light_samples_num, 2.0*i_params.m_media_step_size));

  if (m_params.m_max_specular_depth > 50)
    m_params.m_max_specular_depth = 50;
  }

Spectrum_d DirectLightingLTEIntegrator::_SurfaceRadiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, ThreadSpecifics i_ts) const
  {
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());
  ASSERT(i_ts.mp_pool && i_ts.mp_random_generator);
  MemoryPool *p_pool = i_ts.mp_pool;

  Spectrum_d radiance;
  const BSDF *p_bsdf = i_intersection.mp_primitive->GetBSDF(i_intersection.m_dg, i_intersection.m_triangle_index, *p_pool);

  // Add emitting lighting from the surface (if the surface has light source properties).
  const AreaLightSource *p_light_source = i_intersection.mp_primitive->GetAreaLightSource_RawPtr();
  if (p_light_source)
    radiance = p_light_source->Radiance(i_intersection.m_dg, i_intersection.m_triangle_index, i_ray.m_base_ray.m_direction*(-1.0));

  // Compute direct lighting.
  bool has_non_specular = p_bsdf->GetComponentsNum( BxDFType(BSDF_ALL & ~BSDF_SPECULAR) ) > 0;
  if (has_non_specular)
    radiance += mp_direct_lighting_integrator->ComputeDirectLighting(i_intersection, i_ray.m_base_ray.m_direction*(-1.0), p_bsdf, ip_sample, i_ts);

  // Trace rays for specular reflection and refraction.
  if (i_ray.m_specular_depth <= m_params.m_max_specular_depth)
    {
    radiance += _SpecularReflect(i_ray, i_intersection, p_bsdf, ip_sample, i_ts);
    radiance += _SpecularTransmit(i_ray, i_intersection, p_bsdf, ip_sample, i_ts);
    }

  return radiance;
  }

Spectrum_d DirectLightingLTEIntegrator::_MediaRadianceAndTranmsittance(const RayDifferential &i_ray, const Sample *ip_sample, SpectrumCoef_d &o_transmittance, ThreadSpecifics i_ts) const
  {
  ASSERT(i_ts.mp_pool && i_ts.mp_random_generator);
  MemoryPool *p_pool = i_ts.mp_pool;
  RandomGenerator<double> *p_rng = i_ts.mp_random_generator;

  Ray ray(i_ray.m_base_ray);
  const VolumeRegion *p_volume = mp_scene->GetVolumeRegion_RawPtr();

  double t0, t1;
  if (p_volume==NULL || p_volume->Intersect(ray, &t0, &t1)==false || t0==t1)
    {
    o_transmittance = SpectrumCoef_d(1.0);
    return Spectrum_d();
    }

  // offset1 is the offset in the step used for primary ray's marching
  // offset2 is the offset used for computing optical thickness between adjacent samples in the primary ray
  double offset1, offset2, base_step;
  if (ip_sample)
    {
    offset1 = *ip_sample->GetSamplesSequence1D(m_media_offset1_id).m_begin;
    offset2 = *ip_sample->GetSamplesSequence1D(m_media_offset2_id).m_begin;
    base_step = m_params.m_media_step_size;
    }
  else
    {
    offset1 = (*p_rng)(1.0);
    offset2 = (*p_rng)(1.0);

    // Increase step size for secondary rays to reduce computation time (accuracy is less important here).
    base_step = 2.0*m_params.m_media_step_size;
    }

  // Do single scattering volume integration.
  Spectrum_d radiance;
  const LightSources &lights = mp_scene->GetLightSources();
  size_t delta_lights = lights.m_delta_light_sources.size();
  size_t area_lights = lights.m_area_light_sources.size();
  size_t infinite_lights = lights.m_infinite_light_sources.size();
  size_t num_lights = delta_lights+area_lights+infinite_lights;

  SpectrumCoef_d transmittance(1.0);
  Point3D_d point = ray(t0), prev_point;
  Vector3D_d direction = ray.m_direction * (-1.0);

  /*
  The step used for ray marching is not constant.
  The step is decreased inversely to the transmittance. Such sampling strategy is much more efficient than using a constant step size since the samples with low transmittance are less important
  than the ones with high transmittance. Think of it as sampling with the PDF based on the transmittance.
  */
  
  double step = base_step;
  for (size_t i=0;t0<t1-DBL_EPS;++i)
    {
    step = std::min(step, t1-t0);

    // We use low discrepancy samples. The point here is that we don't know the exact number of samples needed.
    // RadicalInverse produce well stratified samples for any number of samples.
    double sample1D = SamplingRoutines::RadicalInverse((unsigned int)i+1, 2);
    Point2D_d sample2D(SamplingRoutines::RadicalInverse((unsigned int)i+1, 3), SamplingRoutines::RadicalInverse((unsigned int)i+1, 5));

    prev_point = point;
    point = ray(t0+offset1*step);
    t0 += step;

    Ray delta_ray(prev_point, ray.m_direction, 0.0, Vector3D_d(point-prev_point).Length());

    // Note that we still use constant step size for the optical thickness calculation.
    SpectrumCoef_d opt_thickness = p_volume->OpticalThickness(delta_ray, base_step, offset2);

    transmittance[0] *= exp(-opt_thickness[0]);
    transmittance[1] *= exp(-opt_thickness[1]);
    transmittance[2] *= exp(-opt_thickness[2]);

    // Compute single-scattering source term.
    radiance += transmittance * p_volume->Emission(point);
    SpectrumCoef_d scattering = p_volume->Scattering(point);
    if (scattering.IsBlack()==false && num_lights > 0)
      {
      // We sample all lights with uniform PDF.
      // Probably need to use a better strategy but that's not immediately clear how to get a good PDF since it changes as we move along the ray.
      size_t light_index = std::min((size_t)(sample1D * num_lights), num_lights-1);

      double light_pdf = 1.0;
      Ray lighting_ray;
      Spectrum_d light_radiance;

      if (light_index < delta_lights)
        light_radiance = lights.m_delta_light_sources[light_index]->Lighting(point, lighting_ray);
      else if (light_index < delta_lights+infinite_lights)
        {
        light_radiance = lights.m_infinite_light_sources[light_index-delta_lights]->SampleLighting(sample2D, lighting_ray.m_direction, light_pdf);
        lighting_ray.m_origin=point;
        lighting_ray.m_min_t=0.0;
        lighting_ray.m_max_t=DBL_INF;
        }
      else
        light_radiance = lights.m_area_light_sources[light_index-delta_lights-infinite_lights]->SampleLighting(point, (*p_rng)(1.0), sample2D, lighting_ray, light_pdf);

      if (light_radiance.IsBlack()==false && light_pdf > 0.0 && mp_scene->IntersectTest(lighting_ray)==false)
        {
        Spectrum_d tmp = light_radiance * _MediaTransmittance(lighting_ray, i_ts);
        radiance += transmittance * scattering * tmp * (p_volume->Phase(point, lighting_ray.m_direction*(-1.0), direction) * step * double(num_lights) / light_pdf);
        }
      }

    // Increase the step size. The step size is inversely proportional to the transmittance.
    double luminance = SpectrumRoutines::Luminance(transmittance);
    if (luminance < DBL_EPS) break;
    step = base_step / luminance;
    }

  o_transmittance = transmittance;
  return radiance;
  }

SpectrumCoef_d DirectLightingLTEIntegrator::_MediaTransmittance(const Ray &i_ray, ThreadSpecifics i_ts) const
  {
  ASSERT(i_ts.mp_pool && i_ts.mp_random_generator);

  const VolumeRegion *p_volume = mp_scene->GetVolumeRegion_RawPtr();
  if (p_volume==NULL)
    return SpectrumCoef_d(1.0);

  // Increase step size for secondary rays to reduce computation time.
  SpectrumCoef_d opt_thickness = p_volume->OpticalThickness(i_ray, 2.0*m_params.m_media_step_size, (*i_ts.mp_random_generator)(1.0));
  return SpectrumCoef_d(exp(-opt_thickness[0]), exp(-opt_thickness[1]), exp(-opt_thickness[2]));
  }

void DirectLightingLTEIntegrator::_RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  ASSERT(ip_sampler);
  mp_direct_lighting_integrator->RequestSamples(ip_sampler);

  m_media_offset1_id = ip_sampler->AddSamplesSequence1D(1);
  m_media_offset2_id = ip_sampler->AddSamplesSequence1D(1);
  }