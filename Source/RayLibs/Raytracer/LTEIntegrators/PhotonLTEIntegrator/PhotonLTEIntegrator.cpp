/*
* Copyright (C) 2014 - 2015 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
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

#include "../PhotonLTEIntegrator.h"
#include "PhotonInternalTypes.h"
#include <Common/MemoryPool.h>
#include <Math/SamplingRoutines.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Core/CoreUtils.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <tbb/pipeline.h>
#include <tbb/parallel_invoke.h>
#include <chrono>

// 0.87 cosine value corresponds to 30 degrees angle.
const double PhotonLTEIntegrator::MAX_NORMAL_DEVIATION_COS = 0.87;

//////////////////////////////////////// PhotonLTEIntegrator /////////////////////////////////////////////
PhotonLTEIntegrator::PhotonLTEIntegrator(intrusive_ptr<const Scene> ip_scene, PhotonLTEIntegratorParams i_params, intrusive_ptr<Log> ip_log) :
LTEIntegrator(ip_scene), mp_scene(ip_scene), m_params(i_params), mp_log(ip_log), m_shooting_in_progress(false), m_shooting_stopped(false)
  {
  ASSERT(ip_scene);

  // We double the media step size for secondary rays to reduce computation time (since the accuracy is less important here).
  mp_direct_lighting_integrator.reset(new DirectLightingIntegrator(ip_scene, i_params.m_direct_light_samples_num,
    i_params.m_direct_light_samples_num, 2.0*i_params.m_media_step_size));

  if (m_params.m_max_specular_depth > 50)
    m_params.m_max_specular_depth = 50;

  if (m_params.m_max_caustic_photons == 0 || m_params.m_max_caustic_photons > MAX_PHOTONS_IN_MAP)
    m_params.m_max_caustic_photons = MAX_PHOTONS_IN_MAP;

  if (m_params.m_max_direct_photons == 0 || m_params.m_max_direct_photons > MAX_PHOTONS_IN_MAP)
    m_params.m_max_direct_photons = MAX_PHOTONS_IN_MAP;

  if (m_params.m_max_indirect_photons == 0 || m_params.m_max_indirect_photons > MAX_PHOTONS_IN_MAP)
    m_params.m_max_indirect_photons = MAX_PHOTONS_IN_MAP;

  m_scene_total_area = 0.0;
  const std::vector<intrusive_ptr<const Primitive>> &primitives = ip_scene->GetPrimitives();
  for(size_t i=0;i<primitives.size();++i)
    m_scene_total_area += primitives[i]->GetTriangleMesh_RawPtr()->GetArea();
  }

void PhotonLTEIntegrator::_RequestSamples(intrusive_ptr<Sampler> ip_sampler)
  {
  ASSERT(ip_sampler);
  mp_direct_lighting_integrator->RequestSamples(ip_sampler);

  if (m_params.m_gather_samples_num>0)
    {
    size_t gather_samples_num = m_params.m_gather_samples_num;

    m_bsdf_1D_samples_id = ip_sampler->AddSamplesSequence1D(gather_samples_num, &gather_samples_num);
    m_bsdf_2D_samples_id = ip_sampler->AddSamplesSequence2D(gather_samples_num, &gather_samples_num);

    m_direction_1D_samples_id = ip_sampler->AddSamplesSequence1D(gather_samples_num, &gather_samples_num);
    m_direction_2D_samples_id = ip_sampler->AddSamplesSequence2D(gather_samples_num, &gather_samples_num);

    ASSERT(gather_samples_num >= m_params.m_gather_samples_num);
    m_params.m_gather_samples_num = gather_samples_num;
    }

  m_media_offset1_id = ip_sampler->AddSamplesSequence1D(1);
  m_media_offset2_id = ip_sampler->AddSamplesSequence1D(1);
  }

void PhotonLTEIntegrator::_GetLightsPowerCDF(const LightSources &i_light_sources, std::vector<double> &o_lights_CDF)
  {
  size_t delta_lights_num = i_light_sources.m_delta_light_sources.size();
  size_t infinite_lights_num = i_light_sources.m_infinite_light_sources.size();
  size_t area_lights_num = i_light_sources.m_area_light_sources.size();
  size_t lights_num = delta_lights_num+infinite_lights_num + area_lights_num;

  o_lights_CDF.resize(lights_num, 0.0);
  if (lights_num == 0)
    return;

  for(size_t i=0;i<delta_lights_num;++i)
    {
    o_lights_CDF[i] = SpectrumRoutines::Luminance(i_light_sources.m_delta_light_sources[i]->Power());
    ASSERT(o_lights_CDF[i] >= 0.0);
    }

  for(size_t i=0;i<infinite_lights_num;++i)
    {
    size_t j=delta_lights_num+i;
    o_lights_CDF[j] = SpectrumRoutines::Luminance(i_light_sources.m_infinite_light_sources[i]->Power());
    ASSERT(o_lights_CDF[j] >= 0.0);
    }

  for(size_t i=0;i<area_lights_num;++i)
    {
    size_t j=delta_lights_num+infinite_lights_num+i;
    o_lights_CDF[j] = SpectrumRoutines::Luminance(i_light_sources.m_area_light_sources[i]->Power());
    ASSERT(o_lights_CDF[j] >= 0.0);
    }

  for (size_t i=1;i<o_lights_CDF.size();++i)
    o_lights_CDF[i] += o_lights_CDF[i-1];

  // Normalize CDF values.
  double total_power = o_lights_CDF[lights_num-1];
  ASSERT(total_power >= 0.0);
  if (total_power > DBL_EPS)
    for(size_t i=0;i<lights_num;++i)
      o_lights_CDF[i] /= total_power;
  else
    {
    // If all lights have zero power luminance we just always sample the last one.
    o_lights_CDF[lights_num-1] = 1.0;
    }
  }

std::pair<Spectrum_f, Spectrum_f>
PhotonLTEIntegrator::_LookupPhotonIrradiance(const Point3D_d &i_point, const Vector3D_d &i_normal, shared_ptr<const KDTree<Photon>> ip_photon_map,
                                             size_t i_photon_paths, double i_max_lookup_dist, NearestPhoton *op_nearest_photons) const
  {
  if (ip_photon_map == NULL)
    return std::make_pair(Spectrum_f(), Spectrum_f());

  PhotonFilter filter(i_point, i_normal, MAX_NORMAL_DEVIATION_COS);
  size_t photons_found = ip_photon_map->GetNearestPoints(i_point, LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE, op_nearest_photons, filter, i_max_lookup_dist);
  if (photons_found == 0)
    return std::make_pair(Spectrum_f(), Spectrum_f());

  double max_dist_sqr = 0.0;
  Spectrum_d external_irradiance, internal_irradiance;
  for(size_t i=0;i<photons_found;++i)
    {
    Point3D_d photon_position = Convert<double>( op_nearest_photons[i].mp_point->m_point );
    Vector3D_d photon_direction = op_nearest_photons[i].mp_point->m_incident_direction.ToVector3D<double>();
    Spectrum_d photon_weight = Convert<double>( op_nearest_photons[i].mp_point->m_weight );

    double tmp_dist_sqr = Vector3D_d(photon_position - i_point).LengthSqr();
    if (tmp_dist_sqr > max_dist_sqr) max_dist_sqr = tmp_dist_sqr;

    if (photon_direction * i_normal > 0.0)
      external_irradiance += photon_weight;
    else
      internal_irradiance += photon_weight;
    }

  if (photons_found<LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE || max_dist_sqr==0.0)
    max_dist_sqr = i_max_lookup_dist * i_max_lookup_dist;
  else
    /*
    Since the max_dist_sqr is exactly equal to the squared distance to the farthest photon we need to multiply the area by the correcting factor.
    The easy way to understand it is the following. Think of what would happen if we decrease the radius a little bit.
    The farthest photon will drop out while the area won't change significantly. Thus the resulting radiance value would change by the value brought by the farthest photon.
    We need to increase the total area by a half of a single photon's area.
    */
    max_dist_sqr *= (photons_found) / (photons_found-0.5);

  double inv = 1.0 / (M_PI * i_photon_paths * max_dist_sqr);
  return std::make_pair(Convert<float>(external_irradiance * inv), Convert<float>(internal_irradiance * inv) );
  }

void PhotonLTEIntegrator::_ConstructIrradiancePhotonMap()
  {
  // Caustic map can be null.
  ASSERT(mp_photon_maps->GetDirectMap());

  // If the indirect map is null, nothing to do here.
  if (mp_photon_maps->GetIndirectMap() == NULL)
    return;

  mp_irradiance_map.reset((KDTree<IrradiancePhoton>*)NULL);

  const std::vector<Photon> &direct_photons = mp_photon_maps->GetDirectMap()->GetAllPoints();
  const std::vector<Photon> &indirect_photons = mp_photon_maps->GetIndirectMap()->GetAllPoints();

  //The method selects 10% of indirect photons as positions for irradiance photons.  
  std::vector<IrradiancePhoton> irradiance_photons;
  irradiance_photons.reserve(indirect_photons.size() / 10);
  for(size_t i=0;i<indirect_photons.size();i+=10)
    irradiance_photons.push_back( IrradiancePhoton(indirect_photons[i].m_point, Spectrum_f(), Spectrum_f(), indirect_photons[i].m_normal) );

  if (irradiance_photons.empty())
    return;

  // Estimate lookup radius so that the corresponding area will contain required number of photons (in average).
  double direct_photon_area = direct_photons.empty() ? 0.0 : m_scene_total_area / direct_photons.size();
  double max_direct_lookup_dist = sqrt(direct_photon_area*LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE*INV_PI);

  double indirect_photon_area = indirect_photons.empty() ? 0.0 : m_scene_total_area / indirect_photons.size();
  double max_indirect_lookup_dist = sqrt(indirect_photon_area*LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE*INV_PI);

  // Not sure what's a better way to estimate caustic lookup radius.
  double max_caustic_lookup_dist = std::max(max_direct_lookup_dist, max_indirect_lookup_dist);

  // Compute irradiance value for each of the irradiance photons.
  // We do that in multiple threads since all photons can be processed independently.
  IrradiancePhotonProcess process(this, irradiance_photons, max_caustic_lookup_dist, max_direct_lookup_dist, max_indirect_lookup_dist);
  tbb::parallel_for(tbb::blocked_range<size_t>(0,irradiance_photons.size()), process);

  mp_irradiance_map.reset( new KDTree<IrradiancePhoton>(irradiance_photons) );

  /*
  Estimate maximum lookup distance for the irradiance photons.
  We do this by finding the set of the most distant indirect photons to the set of irradiance photons.
  The set contains 0.1% of all of the indirect photons. After we have the distances we take the smallest one from this set and use it as the maximum lookup distance.
  That way we can say that for 99.9% of all points in the scene (reachable by indirect light) the nearest irradiance photon can be found within this distance.
  */

  // Process only every 11th indirect photon to speed things up. It's important that this step is relatively prime with the increment step for irradiance photons above.
  size_t step = 11;

  // Compute min distance for every indirect photon in parallel.
  std::vector<double> sqr_distances((indirect_photons.size()+step-1)/step, 0.f);
  tbb::parallel_for((size_t)0, sqr_distances.size(), [&](size_t i)
    {
    Point3D_d photon_position = Convert<double>(indirect_photons[i*step].m_point);
    Vector3D_d photon_normal = indirect_photons[i*step].m_normal.ToVector3D<double>();

    IrradiancePhotonFilter filter(photon_position, photon_normal, MAX_NORMAL_DEVIATION_COS);
    const IrradiancePhoton *p_irradiance_photon = mp_irradiance_map->GetNearestPoint(photon_position, filter);
    if (p_irradiance_photon == NULL)
      sqr_distances[i] = 0.0; //  by setting it to 0 we prevent this value from affecting the final result
    else
      sqr_distances[i] = Vector3D_d(photon_position - Convert<double>(p_irradiance_photon->m_point)).LengthSqr();
    });

  // Now get the 1/1000th most distant photon to get the 0.1% percentile
  size_t index = sqr_distances.size()<=1 ? 0 : std::max(sqr_distances.size()/1000, (size_t)1);
  std::nth_element(sqr_distances.begin(), sqr_distances.begin()+index, sqr_distances.end(), std::greater<double>());
  ASSERT(sqr_distances[index]>=0.0);

  if (sqr_distances[index] == 0.0)
    m_max_irradiance_lookup_dist = DBL_INF;
  else
    m_max_irradiance_lookup_dist = sqrt(sqr_distances[index]);
  }

void PhotonLTEIntegrator::ShootPhotons(size_t i_photons, bool i_low_thread_priority)
  {
  auto start_time = std::chrono::system_clock::now();
  mp_photon_maps.reset(new PhotonMaps());

  const LightSources &lights = mp_scene->GetLightSources();
  if (lights.m_delta_light_sources.size() + lights.m_area_light_sources.size() + lights.m_infinite_light_sources.size() == 0 || i_photons == 0)
    return;

  m_shooting_in_progress = true;
  m_shooting_stopped = false;

  std::vector<double> lights_CDF;
  _GetLightsPowerCDF(lights, lights_CDF);

  PhotonsInputFilter input_filter(this, mp_photon_maps, i_photons, m_params.m_max_caustic_photons, m_params.m_max_direct_photons, m_params.m_max_indirect_photons, MAX_PIPELINE_TOKENS_NUM, 4096);
  PhotonsShootingFilter shooting_filter(this, mp_scene, lights_CDF, i_low_thread_priority);
  PhotonsMergingFilter merging_filter(mp_photon_maps);

  tbb::pipeline pipeline;
  pipeline.add_filter(input_filter);
  pipeline.add_filter(shooting_filter);
  pipeline.add_filter(merging_filter);

  pipeline.run(MAX_PIPELINE_TOKENS_NUM);
  pipeline.clear();
  m_shooting_in_progress = false;

  // Construct the KD trees. We explicitly do this now while we are still in a single thread to avoid concurrency issues later.
  tbb::parallel_invoke([&]{mp_photon_maps->GetCausticMap(); },
                       [&]{mp_photon_maps->GetDirectMap(); },
                       [&]{mp_photon_maps->GetIndirectMap(); });

  _ConstructIrradiancePhotonMap();

  if (mp_log && m_shooting_stopped == false)
    {
    auto end_time = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    mp_log->LogMessage(Log::INFO_LEVEL, "Photon shooting complete in " + std::to_string(duration) + " ms.");
    }
  }

bool PhotonLTEIntegrator::StopShooting()
  {
  if (m_shooting_in_progress == false)
    {
    if (mp_log)
      mp_log->LogMessage(Log::WARNING_LEVEL, "Photon shooting is not active. Nothing to stop.");

    return false;
    }

  if (m_shooting_stopped == true)
    {
    if (mp_log)
      mp_log->LogMessage(Log::WARNING_LEVEL, "Photon shooting has already been stopped.");

    return false;
    }

  if (mp_log)
    mp_log->LogMessage(Log::INFO_LEVEL, "Photon shooting has been stopped.");

  m_shooting_stopped = true;
  return true;
  }

bool PhotonLTEIntegrator::InProgress() const
  {
  return m_shooting_in_progress;
  }

Spectrum_d PhotonLTEIntegrator::_SurfaceRadiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, ThreadSpecifics i_ts) const
  {
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());
  ASSERT(i_ts.mp_pool && i_ts.mp_random_generator);
  MemoryPool *p_pool = i_ts.mp_pool;
  RandomGenerator<double> *p_rng = i_ts.mp_random_generator;

  Spectrum_d radiance;
  const BSDF *p_bsdf = i_intersection.mp_primitive->GetBSDF(i_intersection.m_dg, i_intersection.m_triangle_index, *p_pool);
  Vector3D_d incident = i_ray.m_base_ray.m_direction*(-1.0);
  Vector3D_d geometric_normal = p_bsdf->GetGeometricNormal();

  // Add emitting lighting from the surface (if the surface has light source properties).
  const AreaLightSource *p_light_source = i_intersection.mp_primitive->GetAreaLightSource_RawPtr();
  if (p_light_source)
    radiance = p_light_source->Radiance(i_intersection.m_dg, i_intersection.m_triangle_index, incident);

  bool has_non_specular = p_bsdf->GetComponentsNum( BxDFType(BSDF_ALL & ~BSDF_SPECULAR) ) > 0;
  if (has_non_specular)
    {
    // Compute direct lighting.
    radiance += mp_direct_lighting_integrator->ComputeDirectLighting(i_intersection, incident, p_bsdf, ip_sample, i_ts);

    radiance += _LookupCausticRadiance(p_bsdf, i_intersection.m_dg, incident, i_ts);

    // Compute indirect lighting by shooting final gather rays.
    if (mp_irradiance_map)
      radiance += _FinalGather(i_intersection, incident, p_bsdf, ip_sample, i_ts);
   
    /*
    const size_t samples_num_sqrt = 5;
    Point2D_d bsdf_scattering_samples[samples_num_sqrt*samples_num_sqrt];
    SamplesSequence2D bsdf_scattering_sequence(bsdf_scattering_samples, bsdf_scattering_samples + samples_num_sqrt*samples_num_sqrt);
    SamplingRoutines::StratifiedSampling2D(bsdf_scattering_sequence.m_begin, samples_num_sqrt, samples_num_sqrt, true, p_rng);

    IrradiancePhotonFilter filter(i_intersection.m_dg.m_point, geometric_normal, MAX_NORMAL_DEVIATION_COS);
    const IrradiancePhoton *p_irradiance_photon = mp_irradiance_map->GetNearestPoint(i_intersection.m_dg.m_point, filter, m_max_irradiance_lookup_dist);
    if (p_irradiance_photon)
      {
      Spectrum_d tmp;
      if (incident*geometric_normal > 0.0)
        {
        tmp += p_bsdf->TotalScattering(incident, bsdf_scattering_sequence, BxDFType(BSDF_ALL_REFLECTION))  *Convert<double>(p_irradiance_photon->m_external_irradiance);
        tmp += p_bsdf->TotalScattering(incident, bsdf_scattering_sequence, BxDFType(BSDF_ALL_TRANSMISSION))*Convert<double>(p_irradiance_photon->m_internal_irradiance);
        }
      else
        {
        tmp += p_bsdf->TotalScattering(incident, bsdf_scattering_sequence, BxDFType(BSDF_ALL_REFLECTION))  *Convert<double>(p_irradiance_photon->m_internal_irradiance);
        tmp += p_bsdf->TotalScattering(incident, bsdf_scattering_sequence, BxDFType(BSDF_ALL_TRANSMISSION))*Convert<double>(p_irradiance_photon->m_external_irradiance);
        }

      tmp *= INV_PI * 1;
      radiance += tmp;
      }
    */
    }

  // Trace rays for specular reflection and refraction.
  if (i_ray.m_specular_depth <= m_params.m_max_specular_depth)
    {
    radiance += _SpecularReflect(i_ray, i_intersection, p_bsdf, ip_sample, i_ts);
    radiance += _SpecularTransmit(i_ray, i_intersection, p_bsdf, ip_sample, i_ts);
    }

  return radiance;
  }

Spectrum_d PhotonLTEIntegrator::_FinalGather(const Intersection &i_intersection, const Vector3D_d &i_incident, const BSDF *ip_bsdf, const Sample *ip_sample, ThreadSpecifics i_ts) const
  {
  ASSERT(ip_bsdf);
  ASSERT(mp_irradiance_map);
  ASSERT(i_incident.IsNormalized());
  MemoryPool *p_pool = i_ts.mp_pool;
  RandomGenerator<double> *p_rng = i_ts.mp_random_generator;
  Vector3D_d shading_normal = ip_bsdf->GetShadingNormal();

  const double cos_gather_angle = 0.9848; // 10 degrees
  const double cone_pdf = SamplingRoutines::UniformConePDF(cos_gather_angle);

  BxDFType non_specular = BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_DIFFUSE | BSDF_GLOSSY);
  size_t gather_samples = m_params.m_gather_samples_num;
  if (gather_samples == 0)
    return Spectrum_d();

  // Maybe it is worth to put this block out of the _FinalGather() method to improve the performance.
  const size_t samples_num_sqrt = 5;
  Point2D_d bsdf_scattering_samples[samples_num_sqrt*samples_num_sqrt];
  SamplesSequence2D bsdf_scattering_sequence(bsdf_scattering_samples, bsdf_scattering_samples + samples_num_sqrt*samples_num_sqrt);
  SamplingRoutines::StratifiedSampling2D(bsdf_scattering_sequence.m_begin, samples_num_sqrt, samples_num_sqrt, false);

  SamplesSequence1D bsdf_1D_samples, direction_1D_samples;
  SamplesSequence2D bsdf_2D_samples, direction_2D_samples;
  if (ip_sample)
    {
    bsdf_1D_samples = ip_sample->GetSamplesSequence1D(m_bsdf_1D_samples_id);
    bsdf_2D_samples = ip_sample->GetSamplesSequence2D(m_bsdf_2D_samples_id);

    direction_1D_samples = ip_sample->GetSamplesSequence1D(m_direction_1D_samples_id);
    direction_2D_samples = ip_sample->GetSamplesSequence2D(m_direction_2D_samples_id);
    }
  else
    {
    size_t gather_samples_sqrt = (size_t) ceil(sqrt((double)gather_samples));
    gather_samples = gather_samples_sqrt*gather_samples_sqrt;

    // BSDF samples.
    double *bsdf_samples_1D = (double *)p_pool->Alloc( gather_samples * sizeof(double) );
    Point2D_d *bsdf_samples_2D = (Point2D_d *)p_pool->Alloc( gather_samples * sizeof(Point2D_d) );

    bsdf_1D_samples = SamplesSequence1D(bsdf_samples_1D, bsdf_samples_1D+gather_samples);
    bsdf_2D_samples = SamplesSequence2D(bsdf_samples_2D, bsdf_samples_2D+gather_samples);

    SamplingRoutines::StratifiedSampling1D(bsdf_1D_samples.m_begin, gather_samples, true, p_rng);
    SamplingRoutines::StratifiedSampling2D(bsdf_2D_samples.m_begin, gather_samples_sqrt, gather_samples_sqrt, true, p_rng);

    // Direction samples.
    double *direction_samples_1D = (double *)p_pool->Alloc( gather_samples * sizeof(double) );
    Point2D_d *direction_samples_2D = (Point2D_d *)p_pool->Alloc( gather_samples * sizeof(Point2D_d) );

    direction_1D_samples = SamplesSequence1D(direction_samples_1D, direction_samples_1D+gather_samples);
    direction_2D_samples = SamplesSequence2D(direction_samples_2D, direction_samples_2D+gather_samples);

    SamplingRoutines::StratifiedSampling1D(direction_1D_samples.m_begin, gather_samples, true, p_rng);
    SamplingRoutines::StratifiedSampling2D(direction_2D_samples.m_begin, gather_samples_sqrt, gather_samples_sqrt, true, p_rng);
    }

  // Allocate array for the nearest photons.
  size_t photons_found = 0;
  NearestPhoton *p_nearest_photons = (NearestPhoton*)p_pool->Alloc(32 * sizeof(NearestPhoton));

  // Search for nearest indirect photons.
  if (mp_photon_maps->GetIndirectMap() && mp_photon_maps->GetIndirectMap()->GetNumberOfPoints()>0)
    {
    double indirect_photon_area = m_scene_total_area / mp_photon_maps->GetIndirectMap()->GetNumberOfPoints();
    double max_lookup_dist = sqrt(indirect_photon_area*32*INV_PI);
    PhotonFilter filter(i_intersection.m_dg.m_point, ip_bsdf->GetGeometricNormal(), MAX_NORMAL_DEVIATION_COS);
    photons_found = mp_photon_maps->GetIndirectMap()->GetNearestPoints(i_intersection.m_dg.m_point, 32, p_nearest_photons, filter, max_lookup_dist);
    }
  double inv_photons_found = (photons_found>0) ? 1.0/photons_found : 0.0;

  // Copy photon directions to a local array.
  Vector3D_d *photon_directions = (Vector3D_d*)p_pool->Alloc(photons_found * sizeof(Vector3D_d));
  for (size_t i=0;i<photons_found;++i)
    photon_directions[i] = p_nearest_photons[i].mp_point->m_incident_direction.ToVector3D<double>();

  size_t gather_rays = 0;
  Vector3D_d *p_gather_directions = (Vector3D_d*)p_pool->Alloc( 2 * gather_samples * sizeof(Vector3D_d) );
  SpectrumCoef_d *p_gather_weights = (SpectrumCoef_d*)p_pool->Alloc( 2 * gather_samples * sizeof(SpectrumCoef_d) );

  // Sample BSDF.
  SamplesSequence1D::Iterator iterator_1D = bsdf_1D_samples.m_begin;
  SamplesSequence2D::Iterator iterator_2D = bsdf_2D_samples.m_begin;
  for (size_t i=0;i<gather_samples;++i)
    {
    double component_sample = *iterator_1D;
    Point2D_d bxdf_sample = *iterator_2D;

    double bsdf_pdf = 0.0;
    BxDFType sampled_type;
    Vector3D_d exitant;
    SpectrumCoef_d reflectance = ip_bsdf->Sample(i_incident, exitant, bxdf_sample, component_sample, bsdf_pdf, sampled_type, BxDFType(BSDF_ALL & ~BSDF_SPECULAR));
    if (bsdf_pdf>0.0 && reflectance.IsBlack()==false)
      {
      // Compute PDF for photon-sampling for the sampled direction.
      // We construct a cone around each photon and sample directions in the cone uniformly.
      double photon_pdf = 0.0;
      for (size_t j=0;j<photons_found;++j)
        if (photon_directions[j]*exitant > cos_gather_angle)
          photon_pdf += cone_pdf;
      photon_pdf *= inv_photons_found;

      double weight = SamplingRoutines::PowerHeuristic(gather_samples, bsdf_pdf, gather_samples, photon_pdf);

      p_gather_directions[gather_rays] = exitant;
      p_gather_weights[gather_rays] = reflectance * (weight * fabs(exitant*shading_normal) / bsdf_pdf);
      ++gather_rays;
      }

    ++iterator_1D;
    ++iterator_2D;
    }

  // Sample nearby photons.
  iterator_1D = direction_1D_samples.m_begin;
  iterator_2D = direction_2D_samples.m_begin;
  for (size_t i=0;i<gather_samples && photons_found>0;++i)
    {
    size_t sampled_index = (size_t)( (*iterator_1D) * photons_found);

    // Sample gather ray direction.
    Vector3D_d e2, e3;
    MathRoutines::CoordinateSystem(photon_directions[sampled_index], e2, e3);
    Vector3D_d direction_local = SamplingRoutines::UniformConeSampling(*iterator_2D, cos_gather_angle);
    Vector3D_d exitant = direction_local[0]*e2 + direction_local[1]*e3 + direction_local[2]*photon_directions[sampled_index];

    SpectrumCoef_d reflectance = ip_bsdf->Evaluate(i_incident, exitant);
    if (reflectance.IsBlack()==false)
      {
      double bsdf_pdf = ip_bsdf->PDF(i_incident, exitant);

      // Compute PDF for photon-sampling of direction exitant.
      // We construct a cone around each photon and sample directions in the cone uniformly.
      double photon_pdf = 0.0;
      for (size_t j=0;j<photons_found;++j)
        if (photon_directions[j]*exitant > cos_gather_angle)
          photon_pdf += cone_pdf;
      photon_pdf *= inv_photons_found;

      double weight = SamplingRoutines::PowerHeuristic(gather_samples, photon_pdf, gather_samples, bsdf_pdf);

      p_gather_directions[gather_rays] = exitant;
      p_gather_weights[gather_rays] = reflectance * (weight * fabs(exitant*shading_normal) / photon_pdf);
      ++gather_rays;
      }

    ++iterator_1D;
    ++iterator_2D;
    }

  // Trace final gather rays and compute the radiance.
  Spectrum_d radiance;
  for(size_t i=0;i<gather_rays;++i)
    {
    Ray bounce_ray(i_intersection.m_dg.m_point, p_gather_directions[i]);
    bounce_ray.m_min_t = CoreUtils::GetNextMinT(i_intersection, p_gather_directions[i]);
  
    Intersection gather_isect;
    if (mp_scene->Intersect(RayDifferential(bounce_ray), gather_isect, &bounce_ray.m_max_t))
      {
      // Compute exitant radiance at the final gather intersection.
      Vector3D_d gather_direction = bounce_ray.m_direction*(-1.0);
      const BSDF *p_gather_BSDF = gather_isect.mp_primitive->GetBSDF(gather_isect.m_dg, gather_isect.m_triangle_index, *p_pool);
      Vector3D_d gather_geometric_normal = p_gather_BSDF->GetGeometricNormal();

      IrradiancePhotonFilter filter(gather_isect.m_dg.m_point, gather_geometric_normal, MAX_NORMAL_DEVIATION_COS);
      const IrradiancePhoton *p_irradiance_photon = mp_irradiance_map->GetNearestPoint(gather_isect.m_dg.m_point, filter, m_max_irradiance_lookup_dist);
      if (p_irradiance_photon == NULL)
        continue;

      Spectrum_d tmp;
      if (gather_direction*gather_geometric_normal > 0.0)
        {
        tmp += p_gather_BSDF->TotalScattering(gather_direction, bsdf_scattering_sequence, BxDFType(BSDF_ALL_REFLECTION))  *Convert<double>(p_irradiance_photon->m_external_irradiance);
        tmp += p_gather_BSDF->TotalScattering(gather_direction, bsdf_scattering_sequence, BxDFType(BSDF_ALL_TRANSMISSION))*Convert<double>(p_irradiance_photon->m_internal_irradiance);
        }
      else
        {
        tmp += p_gather_BSDF->TotalScattering(gather_direction, bsdf_scattering_sequence, BxDFType(BSDF_ALL_REFLECTION))  *Convert<double>(p_irradiance_photon->m_internal_irradiance);
        tmp += p_gather_BSDF->TotalScattering(gather_direction, bsdf_scattering_sequence, BxDFType(BSDF_ALL_TRANSMISSION))*Convert<double>(p_irradiance_photon->m_external_irradiance);
        }

      tmp *= INV_PI;
      radiance += tmp * _MediaTransmittance(bounce_ray, i_ts) * p_gather_weights[i];
      }
    }

  return radiance / (double)gather_samples;
  }

Spectrum_d PhotonLTEIntegrator::_LookupCausticRadiance(const BSDF *ip_bsdf, const DifferentialGeometry &i_dg, const Vector3D_d &i_direction, ThreadSpecifics i_ts) const
  {
  ASSERT(ip_bsdf);
  ASSERT(i_direction.IsNormalized());
  MemoryPool *p_pool = i_ts.mp_pool;

  if (mp_photon_maps->GetCausticMap() == NULL)
    return Spectrum_d();

  BxDFType non_specular = BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_DIFFUSE | BSDF_GLOSSY);
  if (ip_bsdf->GetComponentsNum(non_specular) == 0)
    return Spectrum_d();

  // Allocate array for the nearest photons.
  NearestPhoton *p_nearest_photons = (NearestPhoton*)p_pool->Alloc(m_params.m_caustic_lookup_photons_num * sizeof(NearestPhoton));

  PhotonFilter filter(i_dg.m_point, i_dg.m_geometric_normal, MAX_NORMAL_DEVIATION_COS);
  size_t photons_found = mp_photon_maps->GetCausticMap()->GetNearestPoints(i_dg.m_point, m_params.m_caustic_lookup_photons_num, p_nearest_photons, filter, m_params.m_max_caustic_lookup_dist);
  if (photons_found == 0)
    return Spectrum_d();

  double max_dist_sqr = 0.0;
  Spectrum_d radiance;
  for(size_t i=0;i<photons_found;++i)
    {
    Point3D_d photon_position = Convert<double>( p_nearest_photons[i].mp_point->m_point );
    Vector3D_d photon_direction = p_nearest_photons[i].mp_point->m_incident_direction.ToVector3D<double>();
    Spectrum_d photon_weight = Convert<double>( p_nearest_photons[i].mp_point->m_weight );

    double tmp_dist_sqr = Vector3D_d(photon_position - i_dg.m_point).LengthSqr();
    if (tmp_dist_sqr > max_dist_sqr) max_dist_sqr = tmp_dist_sqr;

    double kernel = _PhotonKernel( Vector3D_d(photon_position-i_dg.m_point).LengthSqr(), max_dist_sqr);
    radiance += ip_bsdf->Evaluate(photon_direction, i_direction) * photon_weight * kernel;
    }

  if (photons_found<m_params.m_caustic_lookup_photons_num || max_dist_sqr==0.0)
    max_dist_sqr = m_params.m_max_caustic_lookup_dist * m_params.m_max_caustic_lookup_dist;
  else
    /*
    Since the max_dist_sqr is exactly equal to the squared distance to the farthest photon we need to multiply the area by the correcting factor.
    The easy way to understand it is the following. Think of what will happen if we decrease the radius a little bit.
    The farthest photon will drop out while the area won't change significantly. Thus the resulting radiance value would change by the value brought by the farthest photon.
    We need to increase the total area by a half of a single photon's area.
    */
    max_dist_sqr *= (photons_found) / (photons_found-0.5);

  return radiance / (mp_photon_maps->GetNumberOfCausticPaths() * max_dist_sqr);
  }

double PhotonLTEIntegrator::_PhotonKernel(double i_dist_sqr, double i_max_dist_sqr) const
  {
  ASSERT(i_dist_sqr>=0.0 && i_max_dist_sqr>0.0 && i_dist_sqr<=i_max_dist_sqr);

  // Simpsonís kernel function is used.
  double tmp = (1.0 - i_dist_sqr / i_max_dist_sqr);
  return 3.0 * INV_PI * tmp * tmp;
  }

Spectrum_d PhotonLTEIntegrator::_MediaRadianceAndTranmsittance(const RayDifferential &i_ray, const Sample *ip_sample, SpectrumCoef_d &o_transmittance, ThreadSpecifics i_ts) const
  {
  // TBD
  o_transmittance = SpectrumCoef_d(1.0);
  return Spectrum_d(0.0);
  }

SpectrumCoef_d PhotonLTEIntegrator::_MediaTransmittance(const Ray &i_ray, ThreadSpecifics i_ts) const
  {
  // TBD
  return SpectrumCoef_d(1.0);
  }