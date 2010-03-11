#include "DirectLightingIntegrator.h"
#include <Raytracer/LightsSamplingStrategies/IrradianceLightsSampling.h>
#include <Math/SamplingRoutines.h>
#include "CoreUtils.h"

DirectLightingIntegrator::DirectLightingIntegrator(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<VolumeIntegrator> ip_volume_integrator, size_t i_lights_samples_num,
                                                   size_t i_bsdf_samples_num, intrusive_ptr<const LightsSamplingStrategy> ip_lights_sampling_strategy):
mp_scene(ip_scene), mp_volume_integrator(ip_volume_integrator), m_lights_samples_num(i_lights_samples_num), m_bsdf_samples_num(i_bsdf_samples_num), m_samples_requested(false)
  {
  ASSERT(ip_scene);

  if (i_lights_samples_num>=0)
    m_lights_samples_num = i_lights_samples_num;
  else
    m_lights_samples_num = 0;

  if (i_bsdf_samples_num>=0)
    m_bsdf_samples_num = i_bsdf_samples_num;
  else
    m_bsdf_samples_num = 0;

  if (ip_lights_sampling_strategy)
    mp_lights_sampling_strategy = ip_lights_sampling_strategy;
  else
    mp_lights_sampling_strategy.reset( new IrradianceLightsSampling(mp_scene->GetLightSources()) );
  
  // Sort and cache area lights. This is needed for a quick search of the area light's index by a pointer.
  const LightSources &light_sources = mp_scene->GetLightSources();
  size_t n = light_sources.m_area_light_sources.size();
  m_area_lights_sorted.reserve(n);
  for(size_t i=0;i<n;++i)
    m_area_lights_sorted.push_back(std::make_pair(light_sources.m_area_light_sources[i].get(),i));
  std::sort(m_area_lights_sorted.begin(), m_area_lights_sorted.end());
  }

void DirectLightingIntegrator::RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  ASSERT(ip_sampler);

  m_light_1D_samples_id = ip_sampler->AddSamplesSequence1D(m_lights_samples_num, &m_lights_samples_num);
  m_light_2D_samples_id = ip_sampler->AddSamplesSequence2D(m_lights_samples_num, &m_lights_samples_num);

  m_bsdf_1D_samples_id = ip_sampler->AddSamplesSequence1D(m_bsdf_samples_num, &m_bsdf_samples_num);
  m_bsdf_2D_samples_id = ip_sampler->AddSamplesSequence2D(m_bsdf_samples_num, &m_bsdf_samples_num);

  m_samples_requested = true;
  }

Spectrum_d DirectLightingIntegrator::ComputeDirectLighting(const Intersection &i_intersection, const Vector3D_d &i_view_direction,
                                                           const BSDF *ip_bsdf, const Sample *ip_sample, MemoryPool &i_pool) const
  {
  ASSERT(ip_bsdf);
  ASSERT(i_view_direction.IsNormalized());

  if (m_samples_requested==false && ip_sample!=NULL)
    {
    ip_sample=NULL;
    ASSERT(0 && "Samples have not been requested yet.");
    }

  size_t reflection_components_num = ip_bsdf->GetComponentsNum( BxDFType(BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_REFLECTION) );
  size_t transmission_components_num = ip_bsdf->GetComponentsNum( BxDFType(BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_TRANSMISSION) );
  if (reflection_components_num + transmission_components_num == 0)
    return Spectrum_d(0.0);

  Spectrum_d radiance;
  const LightSources &light_sources = mp_scene->GetLightSources();
  size_t delta_light_sources_num = light_sources.m_delta_light_sources.size();
  size_t infinity_light_sources_num = light_sources.m_infinitiy_light_sources.size();
  size_t area_light_sources_num = light_sources.m_area_light_sources.size();

  // Compute direct lighting from delta lights.
  for(size_t i=0;i<delta_light_sources_num;++i)
    {
    Ray lighting_ray;
    Spectrum_d light = light_sources.m_delta_light_sources[i]->Lighting(i_intersection.m_dg.m_point, lighting_ray);
    if (light.IsBlack()==false)
      {
      Spectrum_d reflectance = ip_bsdf->Evaluate(lighting_ray.m_direction, i_view_direction);

      lighting_ray.m_min_t = CoreUtils::GetNextMinT(i_intersection, lighting_ray.m_direction);
      if (reflectance.IsBlack()==false && mp_scene->IntersectTest(lighting_ray) == false)
        {
        Spectrum_d transmittance = _VolumeTransmittance(lighting_ray);
        radiance.AddWeighted(reflectance*light*transmittance, fabs(lighting_ray.m_direction*i_intersection.m_dg.m_shading_normal));
        }
      }
    }

  double *lights_CDF = static_cast<double*>(i_pool.Alloc( (infinity_light_sources_num + area_light_sources_num)*sizeof(double) ));

  if (reflection_components_num==0)
    {
    // The BSDF has only transmission components, so need to sample only lights in the opposite hemisphere.
    Vector3D_d normal = (i_view_direction*i_intersection.m_dg.m_shading_normal <= 0.0) ? i_intersection.m_dg.m_shading_normal : i_intersection.m_dg.m_shading_normal*(-1.0);
    mp_lights_sampling_strategy->GetLightsCDF(i_intersection.m_dg.m_point, normal, lights_CDF);
    }
  else if (transmission_components_num==0)
    {
    // The BSDF has only reflection components, so need to sample only lights in the same hemisphere.
    Vector3D_d normal = (i_view_direction*i_intersection.m_dg.m_shading_normal >= 0.0) ? i_intersection.m_dg.m_shading_normal : i_intersection.m_dg.m_shading_normal*(-1.0);
    mp_lights_sampling_strategy->GetLightsCDF(i_intersection.m_dg.m_point, normal, lights_CDF);
    }
  else
    // The BSDF has reflection and transmission components, so need to sample lights in both hemispheres.
    mp_lights_sampling_strategy->GetLightsCDF(i_intersection.m_dg.m_point, lights_CDF);

  if (ip_sample)
    {
    DirectLightingSamples samples;
    samples.m_light_1D_samples = ip_sample->GetSamplesSequence1D(m_light_1D_samples_id);
    samples.m_light_2D_samples = ip_sample->GetSamplesSequence2D(m_light_2D_samples_id);
    ASSERT(std::distance(samples.m_light_1D_samples.m_begin, samples.m_light_1D_samples.m_end) == m_lights_samples_num);
    ASSERT(std::distance(samples.m_light_2D_samples.m_begin, samples.m_light_2D_samples.m_end) == m_lights_samples_num);

    samples.m_bsdf_1D_samples = ip_sample->GetSamplesSequence1D(m_bsdf_1D_samples_id);
    samples.m_bsdf_2D_samples = ip_sample->GetSamplesSequence2D(m_bsdf_2D_samples_id);
    ASSERT(std::distance(samples.m_bsdf_1D_samples.m_begin, samples.m_bsdf_1D_samples.m_end) == m_bsdf_samples_num);
    ASSERT(std::distance(samples.m_bsdf_2D_samples.m_begin, samples.m_bsdf_2D_samples.m_end) == m_bsdf_samples_num);

    radiance += _SampleLights(i_intersection, i_view_direction, ip_bsdf, samples, lights_CDF);
    radiance += _SampleBSDF(i_intersection, i_view_direction, ip_bsdf, samples, lights_CDF);
    }
  else
    {
    size_t total_samples = m_lights_samples_num+m_bsdf_samples_num;
    double *samples_1D = (double *)i_pool.Alloc( total_samples * sizeof(double) );
    Point2D_d *samples_2D = (Point2D_d *)i_pool.Alloc( total_samples * sizeof(Point2D_d) );

    DirectLightingSamples samples;
    samples.m_light_1D_samples=SamplesSequence1D(samples_1D, samples_1D+m_lights_samples_num);
    samples.m_bsdf_1D_samples=SamplesSequence1D(samples_1D+m_lights_samples_num, samples_1D+total_samples );

    samples.m_light_2D_samples=SamplesSequence2D(samples_2D, samples_2D+m_lights_samples_num);
    samples.m_bsdf_2D_samples=SamplesSequence2D(samples_2D+m_lights_samples_num, samples_2D+total_samples );

    SamplingRoutines::StratifiedSampling1D(samples.m_light_1D_samples.m_begin, m_lights_samples_num, true);
    SamplingRoutines::StratifiedSampling1D(samples.m_bsdf_1D_samples.m_begin,  m_bsdf_samples_num,   true);
    SamplingRoutines::LatinHypercubeSampling2D(samples.m_light_2D_samples.m_begin, m_lights_samples_num, true);
    SamplingRoutines::LatinHypercubeSampling2D(samples.m_bsdf_2D_samples.m_begin,  m_bsdf_samples_num,   true);

    radiance += _SampleLights(i_intersection, i_view_direction, ip_bsdf, samples, lights_CDF);
    radiance += _SampleBSDF(i_intersection, i_view_direction, ip_bsdf, samples, lights_CDF);
    }

  return radiance;
  }

Spectrum_d DirectLightingIntegrator::_SampleLights(const Intersection &i_intersection, const Vector3D_d &i_view_direction,
                                                   const BSDF *ip_bsdf, const DirectLightingSamples &i_samples, double *ip_lights_CDF) const
  {
  Spectrum_d radiance;

  const LightSources &light_sources = mp_scene->GetLightSources();
  size_t infinity_light_sources_num = light_sources.m_infinitiy_light_sources.size();
  size_t area_light_sources_num = light_sources.m_area_light_sources.size();
  size_t light_sources_num = infinity_light_sources_num + area_light_sources_num;
  if (m_lights_samples_num==0 || light_sources_num==0)
    return Spectrum_d();

  double inv_infinity_lights_probability = infinity_light_sources_num>0 ? 1.0/ip_lights_CDF[infinity_light_sources_num-1] : 0.0;

  Ray lighting_ray;
  SamplesSequence1D::Iterator component_iterator = i_samples.m_light_1D_samples.m_begin;
  SamplesSequence2D::Iterator position_iterator = i_samples.m_light_2D_samples.m_begin;
  for(size_t i=0;i<m_lights_samples_num;++i)
    {
    double component_sample = *component_iterator;
    Point2D_d position_sample = *position_iterator;

    // Binary search for the sampled light source.
    size_t sampled_index = MathRoutines::BinarySearchCDF(ip_lights_CDF, ip_lights_CDF+light_sources_num, component_sample) - ip_lights_CDF;
    ASSERT(sampled_index>=0 && sampled_index<light_sources_num);

    // This is the probability of the sampled light source to be sampled.
    double component_pdf = sampled_index==0 ? ip_lights_CDF[0] : ip_lights_CDF[sampled_index]-ip_lights_CDF[sampled_index-1];

    if (sampled_index<infinity_light_sources_num)
      {
      // If infinity light is sampled.
      double light_pdf=0.0;
      Spectrum_d light = light_sources.m_infinitiy_light_sources[sampled_index]->SampleLighting(i_intersection.m_dg.m_point,
        i_intersection.m_dg.m_shading_normal, position_sample, lighting_ray, light_pdf);

      if (light_pdf>0.0 && light.IsBlack()==false)
        {
        light *= ip_bsdf->Evaluate(i_view_direction, lighting_ray.m_direction);

        double bsdf_pdf = ip_bsdf->PDF(i_view_direction, lighting_ray.m_direction);

        // Compute weighting coefficient for the multiple importance sampling.
        double weight = SamplingRoutines::PowerHeuristic(m_lights_samples_num, light_pdf*component_pdf, m_bsdf_samples_num, bsdf_pdf*component_pdf*inv_infinity_lights_probability);
        weight *= fabs(lighting_ray.m_direction*i_intersection.m_dg.m_shading_normal) / (light_pdf*component_pdf);

        lighting_ray.m_min_t = CoreUtils::GetNextMinT(i_intersection, lighting_ray.m_direction);
        if (weight>0.0 && light.IsBlack()==false && mp_scene->IntersectTest(lighting_ray)==false)
          radiance.AddWeighted(light*_VolumeTransmittance(lighting_ray), weight);
        }
      }
    else
      {
      // If area light is sampled.
      double light_pdf=0.0;
      double triangle_sample = (sampled_index==0 ? component_sample : component_sample-ip_lights_CDF[sampled_index-1]) / component_pdf;
      Spectrum_d light = light_sources.m_area_light_sources[sampled_index-infinity_light_sources_num]->SampleLighting(i_intersection.m_dg.m_point, triangle_sample,
        position_sample, lighting_ray, light_pdf);

      if (light_pdf>0.0 && light.IsBlack()==false)
        {
        lighting_ray.m_max_t -= (1e-4); // To avoid intersection with the area light.
        light *= ip_bsdf->Evaluate(i_view_direction, lighting_ray.m_direction);

        double bsdf_pdf = ip_bsdf->PDF(i_view_direction, lighting_ray.m_direction);

        /*
        Compute weighting coefficient for the multiple importance sampling.
        The subtle point here is that for the BSDF probability we don't multiply it by the probability of sampling exactly this light source and exactly this triangle.
        This is because when sampling the BSDF there is exactly one area light and exactly one triangle contributing to the direct lighting along this direction.
        If the light source (or triangle) currently being sampled is not the nearest one it will contribute zero radiance to the direct lighting along this direction.
        */
        double weight = SamplingRoutines::PowerHeuristic(m_lights_samples_num, light_pdf*component_pdf, m_bsdf_samples_num, bsdf_pdf);
        weight *= fabs(lighting_ray.m_direction*i_intersection.m_dg.m_shading_normal) / (light_pdf*component_pdf);

        lighting_ray.m_min_t = CoreUtils::GetNextMinT(i_intersection, lighting_ray.m_direction);
        if (weight>0.0 && light.IsBlack()==false && mp_scene->IntersectTest(lighting_ray)==false)
          radiance.AddWeighted(light*_VolumeTransmittance(lighting_ray), weight);
        }
      }

    ++component_iterator;
    ++position_iterator;
    }

  return radiance / m_lights_samples_num;
  }

Spectrum_d DirectLightingIntegrator::_SampleBSDF(const Intersection &i_intersection, const Vector3D_d &i_view_direction,
                                                   const BSDF *ip_bsdf, const DirectLightingSamples &i_samples, double *ip_lights_CDF) const
  {
  Spectrum_d radiance;

  const LightSources &light_sources = mp_scene->GetLightSources();
  size_t infinity_light_sources_num = light_sources.m_infinitiy_light_sources.size();
  size_t area_light_sources_num = light_sources.m_area_light_sources.size();
  size_t light_sources_num = infinity_light_sources_num + area_light_sources_num;
  if (m_bsdf_samples_num==0 || light_sources_num==0)
    return Spectrum_d();

  double infinity_lights_probability = infinity_light_sources_num>0 ? ip_lights_CDF[infinity_light_sources_num-1] : 0.0;
  double inv_infinity_lights_probability = infinity_light_sources_num>0 ? 1.0/ip_lights_CDF[infinity_light_sources_num-1] : 0.0;

  SamplesSequence1D::Iterator component_iterator = i_samples.m_bsdf_1D_samples.m_begin;
  SamplesSequence2D::Iterator bxdf_iterator = i_samples.m_bsdf_2D_samples.m_begin;
  for(size_t i=0;i<m_bsdf_samples_num;++i)
    {
    double component_sample = *component_iterator;
    Point2D_d bxdf_sample = *bxdf_iterator;

    double bsdf_pdf=0.0;
    BxDFType sampled_type;
    Vector3D_d lighting_direction;
    Spectrum_d reflectance = ip_bsdf->Sample(i_view_direction, lighting_direction, bxdf_sample, component_sample, bsdf_pdf, sampled_type, BxDFType(BSDF_ALL & ~BSDF_SPECULAR));
    if (bsdf_pdf>0.0 && reflectance.IsBlack()==false)
      {
      Ray lighting_ray(i_intersection.m_dg.m_point, lighting_direction);
      lighting_ray.m_min_t = CoreUtils::GetNextMinT(i_intersection, lighting_direction);

      Intersection isect;
      bool hit = mp_scene->Intersect(RayDifferential(lighting_ray), isect, &lighting_ray.m_max_t);

      if (hit)
        {
        // If area light is sampled.
        const AreaLightSource *p_area_light = isect.mp_primitive->GetAreaLightSource_RawPtr();
        if (p_area_light)
          {
          double light_pdf = p_area_light->LightingPDF(lighting_ray, isect.m_triangle_index);
          Spectrum_d light = p_area_light->Radiance(isect.m_dg, isect.m_triangle_index, lighting_ray.m_direction*(-1.0));
          if (light_pdf > 0.0 && light.IsBlack()==false)
            {
            size_t light_index = _GetAreaLightIndex(p_area_light) + infinity_light_sources_num;
            double light_component_pdf = light_index==0 ? ip_lights_CDF[0] : ip_lights_CDF[light_index]-ip_lights_CDF[light_index-1];

            // Compute weighting coefficient for the multiple importance sampling.
            double weight = SamplingRoutines::PowerHeuristic(m_bsdf_samples_num, bsdf_pdf, m_lights_samples_num, light_pdf*light_component_pdf);
            weight *= fabs(lighting_ray.m_direction*i_intersection.m_dg.m_shading_normal) / bsdf_pdf;
            radiance.AddWeighted(reflectance*light*_VolumeTransmittance(lighting_ray), weight);
            }
          }
        }
      else
        {
        if (infinity_light_sources_num > 0)
          {
          // Select a random infinity light based on the CDF.
          size_t sampled_index = MathRoutines::BinarySearchCDF(ip_lights_CDF, ip_lights_CDF+infinity_light_sources_num, RandomDouble(infinity_lights_probability)) - ip_lights_CDF;
          ASSERT(sampled_index>=0 && sampled_index<infinity_light_sources_num);
          double light_component_pdf = sampled_index==0 ? ip_lights_CDF[0] : ip_lights_CDF[sampled_index]-ip_lights_CDF[sampled_index-1];
          bsdf_pdf *= light_component_pdf*inv_infinity_lights_probability;
          ASSERT(bsdf_pdf > 0.0);

          double light_pdf = light_sources.m_infinitiy_light_sources[sampled_index]->LightingPDF(i_intersection.m_dg.m_point,i_intersection.m_dg.m_shading_normal,lighting_direction);
          Spectrum_d light = light_sources.m_infinitiy_light_sources[sampled_index]->Radiance(RayDifferential(lighting_ray));
          if (light_pdf > 0.0 && light.IsBlack()==false)
            {
            // Compute weighting coefficient for the multiple importance sampling.
            double weight = SamplingRoutines::PowerHeuristic(m_bsdf_samples_num, bsdf_pdf, m_lights_samples_num, light_component_pdf*light_pdf);
            weight *= fabs(lighting_ray.m_direction*i_intersection.m_dg.m_shading_normal) / bsdf_pdf;
            radiance.AddWeighted(reflectance*light*_VolumeTransmittance(lighting_ray), weight);
            }
          }
        }
      }

    ++component_iterator;
    ++bxdf_iterator;
    }

  return radiance / m_bsdf_samples_num;
  }

size_t DirectLightingIntegrator::_GetAreaLightIndex(const AreaLightSource *ip_area_light) const
  {
  if (m_area_lights_sorted.empty())
    {
    ASSERT(0 && "Invalid area light.");
    return 0;
    }

  size_t l=0, r=m_area_lights_sorted.size()-1;
  while(l<r)
    {
    size_t m=(l+r)/2;
    if (m_area_lights_sorted[m].first<ip_area_light) l=m+1; else r=m;
    }

  ASSERT(m_area_lights_sorted[l].first==ip_area_light && "Invalid area light.");
  return m_area_lights_sorted[l].second;
  }

Spectrum_d DirectLightingIntegrator::_VolumeTransmittance(const Ray &i_ray) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());

  if (mp_volume_integrator)
    {
    Spectrum_d transmittance = mp_volume_integrator->Transmittance(i_ray, NULL);
    ASSERT(InRange(transmittance, 0.0, 1.0));
    return transmittance;
    }
  else
    return Spectrum_d(1.0);
  }