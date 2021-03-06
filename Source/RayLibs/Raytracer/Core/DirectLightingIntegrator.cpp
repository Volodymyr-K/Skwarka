/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "DirectLightingIntegrator.h"
#include <Raytracer/LightsSamplingStrategies/IrradianceLightsSamplingStrategy.h>
#include <Math/SamplingRoutines.h>
#include "CoreUtils.h"

DirectLightingIntegrator::DirectLightingIntegrator(intrusive_ptr<const Scene> ip_scene, size_t i_lights_samples_num, size_t i_bsdf_samples_num, double i_media_step_size,
                                                   intrusive_ptr<const LightsSamplingStrategy> ip_lights_sampling_strategy):
mp_scene(ip_scene), m_lights_samples_num(i_lights_samples_num), m_bsdf_samples_num(i_bsdf_samples_num), m_media_step_size(i_media_step_size), m_samples_requested(false)
  {
  ASSERT(ip_scene);
  ASSERT(i_media_step_size > 0.0);

  if (ip_lights_sampling_strategy)
    mp_lights_sampling_strategy = ip_lights_sampling_strategy;
  else
    mp_lights_sampling_strategy.reset( new IrradianceLightsSamplingStrategy(mp_scene->GetLightSources()) );
  
  // Sort and cache area lights. This is needed for a quick search of the area light's index by a pointer.
  const LightSources &light_sources = ip_scene->GetLightSources();
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
                                                           const BSDF *ip_bsdf, const Sample *ip_sample, ThreadSpecifics i_ts) const
  {
  ASSERT(ip_bsdf);
  ASSERT(i_view_direction.IsNormalized());
  ASSERT(i_ts.mp_pool && i_ts.mp_random_generator);
  MemoryPool *p_pool = i_ts.mp_pool;
  RandomGenerator<double> *p_rng = i_ts.mp_random_generator;
  Vector3D_d shading_normal = ip_bsdf->GetShadingNormal();

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
  size_t infinity_light_sources_num = light_sources.m_infinite_light_sources.size();
  size_t area_light_sources_num = light_sources.m_area_light_sources.size();

  // Compute direct lighting from delta lights.
  for(size_t i=0;i<delta_light_sources_num;++i)
    {
    Ray lighting_ray;
    Spectrum_d light = light_sources.m_delta_light_sources[i]->Lighting(i_intersection.m_dg.m_point, lighting_ray);
    if (light.IsBlack()==false)
      {
      SpectrumCoef_d reflectance = ip_bsdf->Evaluate(lighting_ray.m_direction, i_view_direction);

      lighting_ray.m_min_t = CoreUtils::GetNextMinT(i_intersection, lighting_ray.m_direction);
      if (reflectance.IsBlack()==false && mp_scene->IntersectTest(lighting_ray) == false)
        {
        SpectrumCoef_d transmittance = _MediaTransmittance(lighting_ray, i_ts);
        radiance.AddWeighted(reflectance*light*transmittance, fabs(lighting_ray.m_direction*shading_normal));
        }
      }
    }

  double *lights_CDF = static_cast<double*>(p_pool->Alloc( (infinity_light_sources_num + area_light_sources_num)*sizeof(double) ));

  if (reflection_components_num==0)
    {
    // The BSDF has only transmission components, so need to sample only lights in the opposite hemisphere.
    Vector3D_d normal = (i_view_direction*shading_normal <= 0.0) ? shading_normal : shading_normal*(-1.0);
    mp_lights_sampling_strategy->GetLightsCDF(i_intersection.m_dg.m_point, normal, lights_CDF);
    }
  else if (transmission_components_num==0)
    {
    // The BSDF has only reflection components, so need to sample only lights in the same hemisphere.
    Vector3D_d normal = (i_view_direction*shading_normal >= 0.0) ? shading_normal : shading_normal*(-1.0);
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

    radiance += _SampleLights(i_intersection, i_view_direction, ip_bsdf, samples, lights_CDF, i_ts);
    radiance += _SampleBSDF(i_intersection, i_view_direction, ip_bsdf, samples, lights_CDF, i_ts);
    }
  else
    {
    size_t total_samples = m_lights_samples_num+m_bsdf_samples_num;
    double *samples_1D = (double *)p_pool->Alloc( total_samples * sizeof(double) );
    Point2D_d *samples_2D = (Point2D_d *)p_pool->Alloc( total_samples * sizeof(Point2D_d) );

    DirectLightingSamples samples;
    samples.m_light_1D_samples=SamplesSequence1D(samples_1D, samples_1D+m_lights_samples_num);
    samples.m_bsdf_1D_samples=SamplesSequence1D(samples_1D+m_lights_samples_num, samples_1D+total_samples );

    samples.m_light_2D_samples=SamplesSequence2D(samples_2D, samples_2D+m_lights_samples_num);
    samples.m_bsdf_2D_samples=SamplesSequence2D(samples_2D+m_lights_samples_num, samples_2D+total_samples );

    SamplingRoutines::StratifiedSampling1D(samples.m_light_1D_samples.m_begin, m_lights_samples_num, true, p_rng);
    SamplingRoutines::StratifiedSampling1D(samples.m_bsdf_1D_samples.m_begin,  m_bsdf_samples_num,   true, p_rng);
    SamplingRoutines::LatinHypercubeSampling2D(samples.m_light_2D_samples.m_begin, m_lights_samples_num, true, p_rng);
    SamplingRoutines::LatinHypercubeSampling2D(samples.m_bsdf_2D_samples.m_begin,  m_bsdf_samples_num,   true, p_rng);

    radiance += _SampleLights(i_intersection, i_view_direction, ip_bsdf, samples, lights_CDF, i_ts);
    radiance += _SampleBSDF(i_intersection, i_view_direction, ip_bsdf, samples, lights_CDF, i_ts);
    }

  return radiance;
  }

Spectrum_d DirectLightingIntegrator::_SampleLights(const Intersection &i_intersection, const Vector3D_d &i_view_direction,
                                                   const BSDF *ip_bsdf, const DirectLightingSamples &i_samples, double *ip_lights_CDF, ThreadSpecifics i_ts) const
  {
  Spectrum_d radiance;

  const LightSources &light_sources = mp_scene->GetLightSources();
  size_t infinity_light_sources_num = light_sources.m_infinite_light_sources.size();
  size_t area_light_sources_num = light_sources.m_area_light_sources.size();
  size_t light_sources_num = infinity_light_sources_num + area_light_sources_num;
  if (m_lights_samples_num==0 || light_sources_num==0)
    return Spectrum_d();

  Vector3D_d shading_normal = ip_bsdf->GetShadingNormal();
  double inv_infinity_lights_probability = infinity_light_sources_num>0 ? 1.0/ip_lights_CDF[infinity_light_sources_num-1] : 0.0;

  // Determine the (hemi)sphere to be sampled.
  Vector3D_d normal;
  bool sample_entire_sphere = false;
  if (ip_bsdf->GetComponentsNum( BxDFType(BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_REFLECTION) )==0)
    // The BSDF has only transmission components, so need to sample only lights in the opposite hemisphere.
    normal = (i_view_direction*shading_normal <= 0.0) ? shading_normal : shading_normal*(-1.0);
  else if (ip_bsdf->GetComponentsNum( BxDFType(BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_TRANSMISSION) )==0)
    // The BSDF has only reflection components, so need to sample only lights in the same hemisphere.
    normal = (i_view_direction*shading_normal >= 0.0) ? shading_normal : shading_normal*(-1.0);
  else
    // The BSDF has reflection and transmission components, so need to sample lights in both hemispheres.
    sample_entire_sphere = true;

  Ray lighting_ray;
  SamplesSequence1D::Iterator component_iterator = i_samples.m_light_1D_samples.m_begin;
  SamplesSequence2D::Iterator position_iterator = i_samples.m_light_2D_samples.m_begin;
  for(size_t i=0;i<m_lights_samples_num;++i)
    {
    double component_sample = *component_iterator;
    Point2D_d position_sample = *position_iterator;

    // Binary search for the sampled light source.
    double light_component_pdf;
    size_t sampled_index = MathRoutines::BinarySearchCDF(ip_lights_CDF, ip_lights_CDF+light_sources_num, component_sample, &light_component_pdf) - ip_lights_CDF;
    ASSERT(sampled_index<light_sources_num);

    if (sampled_index<infinity_light_sources_num)
      {
      // If infinity light is sampled.
      double light_pdf=0.0;
      Vector3D_d sampled_direction;
      Spectrum_d light = sample_entire_sphere ? light_sources.m_infinite_light_sources[sampled_index]->SampleLighting(position_sample, sampled_direction, light_pdf) :
        light_sources.m_infinite_light_sources[sampled_index]->SampleLighting(normal, position_sample, sampled_direction, light_pdf);
      lighting_ray = Ray(i_intersection.m_dg.m_point, sampled_direction);

      if (light_pdf>0.0 && light.IsBlack()==false)
        {
        light *= ip_bsdf->Evaluate(i_view_direction, lighting_ray.m_direction);
        lighting_ray.m_min_t = CoreUtils::GetNextMinT(i_intersection, lighting_ray.m_direction);

        if (light.IsBlack() == false && mp_scene->IntersectTest(lighting_ray) == false)
          {
          double bsdf_pdf = ip_bsdf->PDF(i_view_direction, lighting_ray.m_direction);

          // Compute weighting coefficient for the multiple importance sampling.
          double weight = SamplingRoutines::PowerHeuristic(m_lights_samples_num, light_pdf*light_component_pdf, m_bsdf_samples_num, bsdf_pdf*light_component_pdf*inv_infinity_lights_probability);
          weight *= fabs(lighting_ray.m_direction*shading_normal) / (light_pdf*light_component_pdf);

          radiance.AddWeighted(light*_MediaTransmittance(lighting_ray, i_ts), weight);
          }
        }
      }
    else
      {
      // If area light is sampled.
      double light_pdf=0.0;
      double triangle_sample = (sampled_index==0 ? component_sample : component_sample-ip_lights_CDF[sampled_index-1]) / light_component_pdf;
      Spectrum_d light = light_sources.m_area_light_sources[sampled_index-infinity_light_sources_num]->SampleLighting(i_intersection.m_dg.m_point, triangle_sample,
        position_sample, lighting_ray, light_pdf);

      if (light_pdf>0.0 && light.IsBlack()==false)
        {
        light *= ip_bsdf->Evaluate(i_view_direction, lighting_ray.m_direction);
        lighting_ray.m_min_t = CoreUtils::GetNextMinT(i_intersection, lighting_ray.m_direction);
        lighting_ray.m_max_t -= (1e-4); // To avoid intersection with the area light.

        if (light.IsBlack() == false && mp_scene->IntersectTest(lighting_ray) == false)
          {
          double bsdf_pdf = ip_bsdf->PDF(i_view_direction, lighting_ray.m_direction);

          /*
          Compute weighting coefficient for the multiple importance sampling.
          The subtle point here is that for the BSDF probability we don't multiply it by the probability of sampling exactly this light source and exactly this triangle.
          This is because when sampling the BSDF there is exactly one area light and exactly one triangle contributing to the direct lighting along this direction.
          If the light source (or triangle) currently being sampled is not the nearest one it will contribute zero radiance to the direct lighting along this direction.
          */
          double weight = SamplingRoutines::PowerHeuristic(m_lights_samples_num, light_pdf*light_component_pdf, m_bsdf_samples_num, bsdf_pdf);
          weight *= fabs(lighting_ray.m_direction*shading_normal) / (light_pdf*light_component_pdf);

          radiance.AddWeighted(light*_MediaTransmittance(lighting_ray, i_ts), weight);
          }
        }
      }

    ++component_iterator;
    ++position_iterator;
    }

  return radiance / (double)m_lights_samples_num;
  }

Spectrum_d DirectLightingIntegrator::_SampleBSDF(const Intersection &i_intersection, const Vector3D_d &i_view_direction,
                                                   const BSDF *ip_bsdf, const DirectLightingSamples &i_samples, double *ip_lights_CDF, ThreadSpecifics i_ts) const
  {
  ASSERT(i_ts.mp_pool && i_ts.mp_random_generator);
  RandomGenerator<double> *p_rng = i_ts.mp_random_generator;

  Spectrum_d radiance;

  const LightSources &light_sources = mp_scene->GetLightSources();
  size_t infinity_light_sources_num = light_sources.m_infinite_light_sources.size();
  size_t area_light_sources_num = light_sources.m_area_light_sources.size();
  size_t light_sources_num = infinity_light_sources_num + area_light_sources_num;
  if (m_bsdf_samples_num==0 || light_sources_num==0)
    return Spectrum_d();

  Vector3D_d shading_normal = ip_bsdf->GetShadingNormal();
  double infinity_lights_probability = infinity_light_sources_num>0 ? ip_lights_CDF[infinity_light_sources_num-1] : 0.0;
  double inv_infinity_lights_probability = infinity_light_sources_num>0 ? 1.0/ip_lights_CDF[infinity_light_sources_num-1] : 0.0;

  // Determine the (hemi)sphere to be sampled.
  Vector3D_d normal;
  bool sample_entire_sphere = false;
  if (ip_bsdf->GetComponentsNum( BxDFType(BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_REFLECTION) )==0)
    // The BSDF has only transmission components, so need to sample only lights in the opposite hemisphere.
    normal = (i_view_direction*shading_normal <= 0.0) ? shading_normal : shading_normal*(-1.0);
  else if (ip_bsdf->GetComponentsNum( BxDFType(BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_TRANSMISSION) )==0)
    // The BSDF has only reflection components, so need to sample only lights in the same hemisphere.
    normal = (i_view_direction*shading_normal >= 0.0) ? shading_normal : shading_normal*(-1.0);
  else
    // The BSDF has reflection and transmission components, so need to sample lights in both hemispheres.
    sample_entire_sphere = true;

  SamplesSequence1D::Iterator component_iterator = i_samples.m_bsdf_1D_samples.m_begin;
  SamplesSequence2D::Iterator bxdf_iterator = i_samples.m_bsdf_2D_samples.m_begin;
  for(size_t i=0;i<m_bsdf_samples_num;++i)
    {
    double component_sample = *component_iterator;
    Point2D_d bxdf_sample = *bxdf_iterator;

    double bsdf_pdf=0.0;
    BxDFType sampled_type;
    Vector3D_d lighting_direction;
    SpectrumCoef_d reflectance = ip_bsdf->Sample(i_view_direction, lighting_direction, bxdf_sample, component_sample, bsdf_pdf, sampled_type, BxDFType(BSDF_ALL & ~BSDF_SPECULAR));
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
          Spectrum_d light = p_area_light->Radiance(isect.m_dg, isect.m_triangle_index, lighting_ray.m_direction*(-1.0));
          if (light.IsBlack()==false)
            {
            double light_pdf = p_area_light->LightingPDF(lighting_ray, isect.m_triangle_index);
            size_t light_index = _GetAreaLightIndex(p_area_light) + infinity_light_sources_num;
            double light_component_pdf = light_index==0 ? ip_lights_CDF[0] : ip_lights_CDF[light_index]-ip_lights_CDF[light_index-1];

            // Compute weighting coefficient for the multiple importance sampling.
            double weight = SamplingRoutines::PowerHeuristic(m_bsdf_samples_num, bsdf_pdf, m_lights_samples_num, light_pdf*light_component_pdf);
            weight *= fabs(lighting_ray.m_direction*shading_normal) / bsdf_pdf;
            radiance.AddWeighted(reflectance*light*_MediaTransmittance(lighting_ray, i_ts), weight);
            }
          }
        }
      else
        {
        if (infinity_light_sources_num > 0)
          {
          // Select a random infinity light based on the CDF.
          double light_component_pdf;
          size_t sampled_index = MathRoutines::BinarySearchCDF(ip_lights_CDF, ip_lights_CDF+infinity_light_sources_num, (*p_rng)(infinity_lights_probability), &light_component_pdf) - ip_lights_CDF;
          ASSERT(sampled_index<infinity_light_sources_num);
          bsdf_pdf *= light_component_pdf*inv_infinity_lights_probability;
          ASSERT(bsdf_pdf > 0.0);

          Spectrum_d light = light_sources.m_infinite_light_sources[sampled_index]->Radiance(RayDifferential(lighting_ray));
          if (light.IsBlack()==false)
            {
            double light_pdf = sample_entire_sphere ? light_sources.m_infinite_light_sources[sampled_index]->LightingPDF(lighting_direction) : 
              light_sources.m_infinite_light_sources[sampled_index]->LightingPDF(normal, lighting_direction);

            // Compute weighting coefficient for the multiple importance sampling.
            double weight = SamplingRoutines::PowerHeuristic(m_bsdf_samples_num, bsdf_pdf, m_lights_samples_num, light_component_pdf*light_pdf);
            weight *= fabs(lighting_ray.m_direction*shading_normal) / bsdf_pdf;
            radiance.AddWeighted(reflectance*light*_MediaTransmittance(lighting_ray, i_ts), weight);
            }
          }
        }
      }

    ++component_iterator;
    ++bxdf_iterator;
    }

  return radiance / (double)m_bsdf_samples_num;
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

SpectrumCoef_d DirectLightingIntegrator::_MediaTransmittance(const Ray &i_ray, ThreadSpecifics i_ts) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());
  ASSERT(i_ts.mp_pool && i_ts.mp_random_generator);

  const VolumeRegion *p_volume = mp_scene->GetVolumeRegion_RawPtr();
  if (p_volume==NULL)
    return SpectrumCoef_d(1.0);

  SpectrumCoef_d opt_thickness = p_volume->OpticalThickness(i_ray, m_media_step_size, (*i_ts.mp_random_generator)(1.0));
  return Exp(-1.0*opt_thickness);
  }