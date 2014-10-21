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

#include "../PhotonLTEIntegrator.h"
#include "PhotonInternalTypes.h"
#include "PhotonBeamAccelerator.h"
#include <Common/MemoryPool.h>
#include <Math/Transform.h>
#include <Math/SamplingRoutines.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Core/CoreUtils.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <tbb/pipeline.h>

///////////////////////////////////////// PhotonsInputFilter //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

PhotonLTEIntegrator::PhotonsInputFilter::PhotonsInputFilter(size_t i_photon_paths, size_t i_number_of_chunks, size_t i_paths_per_chunk) :
filter(serial_out_of_order), m_paths_required(i_photon_paths), m_paths_per_chunk(i_paths_per_chunk), m_next_chunk_index(0), m_paths_completed(0)
  {
  ASSERT(i_number_of_chunks>0);
  ASSERT(i_paths_per_chunk>0);

  for(size_t i=0;i<i_number_of_chunks;++i)
    m_chunks.push_back(new PhotonsChunk(i));
  }

PhotonLTEIntegrator::PhotonsInputFilter::~PhotonsInputFilter()
  {
  for(size_t i=0;i<m_chunks.size();++i)
    delete m_chunks[i];
  }

void* PhotonLTEIntegrator::PhotonsInputFilter::operator()(void*)
  {
  if (m_paths_completed >= m_paths_required)
    return NULL;

  /*
  Here we loop over all chunks until we find an available one, i.e. a one that is not locked by other thread.
  Although this is not really a thread-safe approach it works well here since PhotonsInputFilter is serial, so two
  threads will never try to get a lock concurrently.
  */
  while(m_chunks[m_next_chunk_index]->m_available==false)
    m_next_chunk_index = (m_next_chunk_index+1) % m_chunks.size();

  PhotonsChunk *p_chunk = m_chunks[m_next_chunk_index];
  m_next_chunk_index = (m_next_chunk_index+1) % m_chunks.size();

  p_chunk->m_available = false;
  p_chunk->m_paths_num = std::min(m_paths_per_chunk, m_paths_required-m_paths_completed);
  p_chunk->m_first_path_index = m_paths_completed;

  p_chunk->m_caustic_photons.clear();
  p_chunk->m_direct_photons.clear();
  p_chunk->m_indirect_photons.clear();
  p_chunk->m_photon_beams.clear();

  m_paths_completed += p_chunk->m_paths_num;

  return p_chunk;
  }

/////////////////////////////////////// PhotonsShootingFilter /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

PhotonLTEIntegrator::PhotonsShootingFilter::PhotonsShootingFilter(const PhotonLTEIntegrator *ip_integrator, intrusive_ptr<const Scene> ip_scene,
                                                                  size_t i_photon_paths, const std::vector<double> &i_lights_CDF, bool i_low_thread_priority) :
tbb::filter(parallel), mp_integrator(ip_integrator), mp_scene(ip_scene), m_photon_paths(i_photon_paths), m_lights_CDF(i_lights_CDF), m_low_thread_priority(i_low_thread_priority)
  {
  ASSERT(ip_integrator);
  ASSERT(ip_scene);
  }

void* PhotonLTEIntegrator::PhotonsShootingFilter::operator()(void* ip_chunk)
  {
  int prev_thread_priority = 0;
  if (m_low_thread_priority)
    prev_thread_priority = CoreUtils::SetCurrentThreadPriority(THREAD_PRIORITY_LOWEST);

  PhotonsChunk *p_chunk = static_cast<PhotonsChunk*>(ip_chunk);
  const LightSources &lights = mp_scene->GetLightSources();

  size_t delta_lights = lights.m_delta_light_sources.size();
  size_t area_lights = lights.m_area_light_sources.size();
  size_t infinite_lights = lights.m_infinite_light_sources.size();
  size_t num_lights = delta_lights+area_lights+infinite_lights;
  ASSERT(num_lights == m_lights_CDF.size());

  if (num_lights == 0)
    {
    ASSERT(0 && "If there are no lights in the scene we should not have got here.");

    if (m_low_thread_priority)
      CoreUtils::SetCurrentThreadPriority(prev_thread_priority);
    return p_chunk;
    }

  size_t path_begin = p_chunk->m_first_path_index, path_end = p_chunk->m_first_path_index+p_chunk->m_paths_num;
  for (size_t path_index=path_begin; path_index<path_end; ++path_index)
    {
    Point2D_d position_sample(SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 2), SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 3));
    Point2D_d direction_sample(SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 5), SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 7));

    // Binary search for the sampled light source.
    double light_pdf;
    size_t light_index = MathRoutines::BinarySearchCDF(m_lights_CDF.begin(), m_lights_CDF.end(), SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 11), &light_pdf) - m_lights_CDF.begin();
    ASSERT(light_index>=0 && light_index<num_lights);
    ASSERT(light_pdf > 0.0);

    double photon_pdf = 0.0;
    RayDifferential photon_ray;
    Spectrum_d weight;
    size_t photons_per_light = (size_t)(m_photon_paths * light_pdf + 1.0);

    if (light_index < delta_lights)
      weight = lights.m_delta_light_sources[light_index]->SamplePhoton(direction_sample, photons_per_light, photon_ray, photon_pdf);
    else if (light_index < delta_lights+infinite_lights)
      weight = lights.m_infinite_light_sources[light_index-delta_lights]->SamplePhoton(position_sample, photons_per_light, direction_sample, photon_ray, photon_pdf);
    else
      weight = lights.m_area_light_sources[light_index - delta_lights - infinite_lights]->SamplePhoton(
      SamplingRoutines::RadicalInverse((unsigned int)path_index + 1, 13), photons_per_light, position_sample, direction_sample, photon_ray, photon_pdf);

    if (photon_pdf == 0.0 || weight.IsBlack())
      continue;

    _TracePhoton(p_chunk, path_index, weight/(photon_pdf*light_pdf), photon_ray);
    } // for (size_t path_index=path_begin; path_index<path_end; ++path_index)

  if (m_low_thread_priority)
    CoreUtils::SetCurrentThreadPriority(prev_thread_priority);
  return p_chunk;
  }

void PhotonLTEIntegrator::PhotonsShootingFilter::_TracePhoton(PhotonsChunk *ip_chunk, size_t i_path_index, const Spectrum_d &i_weight, const RayDifferential &i_photon_ray) const
  {
  MemoryPool *p_pool = ip_chunk->mp_memory_pool;
  RandomGenerator<double> *p_rng = ip_chunk->mp_rng;
  intrusive_ptr<const VolumeRegion> p_volume = mp_scene->GetVolumeRegion();
  Spectrum_d weight(i_weight);
  RayDifferential photon_ray(i_photon_ray);

  size_t scatterings = 0;
  bool specular_path = true;
  while (true)
    {
    // First, intersect the ray with the surface and volume media to see what comes first.
    double isect_t;
    Intersection photon_isect;
    bool surface_hit = mp_scene->Intersect(photon_ray, photon_isect, &isect_t);

    double t_begin, t_end;
    bool volume_hit = p_volume && p_volume->Intersect(photon_ray.m_base_ray, &t_begin, &t_end);

    //////////////////////// First handle the volume interaction ////////////////////////

    if (surface_hit==false && volume_hit==false) break; // If no intersection at all, break the loop.
    if (volume_hit && (surface_hit==false || t_begin<isect_t)) // If the volume media is intersected first.
      {
      RayDifferential volume_ray(photon_ray);
      volume_ray.m_base_ray.m_min_t = t_begin;
      volume_ray.m_base_ray.m_max_t = t_end;
      if (surface_hit) volume_ray.m_base_ray.m_max_t = std::min(volume_ray.m_base_ray.m_max_t, isect_t);

      bool absorbed = true;
      size_t volume_scatterings = 0;
      do {
        _SplitAndAddPhotonBeam(volume_ray, weight, ip_chunk);

        // Get random numbers for sampling volume scattering
        double scatter_sample, offset;
        Point2D_d direction_sample;

        // We use low-discrepancy samples only for the first scattering because further scatterings do not really gain from good stratification.
        if (scatterings == 0)
          {
          direction_sample = Point2D_d(SamplingRoutines::RadicalInverse((unsigned int)i_path_index+1, 17), SamplingRoutines::RadicalInverse((unsigned int)i_path_index+1, 19));
          offset = SamplingRoutines::RadicalInverse((unsigned int)i_path_index+1, 23);
          scatter_sample = SamplingRoutines::RadicalInverse((unsigned int)i_path_index+1, 29);
          }
        else
          {
          direction_sample = Point2D_d((*p_rng)(1.0), (*p_rng)(1.0));
          offset = (*p_rng)(1.0);
          scatter_sample = (*p_rng)(1.0);
          }

        double scatter_t, pdf;
        SpectrumCoef_d transmittance;
        bool scattered = p_volume->SampleScattering(volume_ray.m_base_ray, scatter_sample, mp_integrator->m_params.m_media_step_size, offset, scatter_t, pdf, transmittance);
        ASSERT(pdf>0.0);

        if (scattered == false)
          {
          absorbed = false;
          weight *= transmittance/pdf;
          break;
          }

        ASSERT(scatter_t>=volume_ray.m_base_ray.m_min_t && scatter_t<=volume_ray.m_base_ray.m_max_t);
        Point3D_d scatter_point = volume_ray.m_base_ray(scatter_t);

        // TODO: Need to add Sample() method to the PhaseFunction interface and sample according to the PDF
        Vector3D_d scatter_direction = SamplingRoutines::UniformSphereSampling(direction_sample);
        double phase = p_volume->Phase(scatter_point, volume_ray.m_base_ray.m_direction, scatter_direction);
        pdf *= SamplingRoutines::UniformSpherePDF();

        // Possibly terminate photon path with Russian roulette.
        SpectrumCoef_d scatter_coef = p_volume->Scattering(scatter_point), attenuation_coef = p_volume->Attenuation(scatter_point);
        double continue_probability = std::min(0.90, SpectrumRoutines::Luminance(scatter_coef) / SpectrumRoutines::Luminance(attenuation_coef)); // give at least 10% to termination
        if ((*p_rng)(1.0) > continue_probability)
          break;
          
        weight *= scatter_coef * transmittance * (phase / (pdf*continue_probability));

        ++scatterings;
        ++volume_scatterings;
        specular_path = false;

        volume_ray = _ScatterRayDiffusely(volume_ray, scatter_t, scatter_direction);

        // Clamp the range of the scattered ray to the bounds of the volume region
        volume_hit = p_volume->Intersect(volume_ray.m_base_ray, &t_begin, &t_end);
        ASSERT(volume_hit && t_begin<=DBL_EPS); // Scattered ray should start within the volume.
        volume_ray.m_base_ray.m_max_t = t_end;
        } while (true);
      
      if (absorbed)
        break;

      // Shoot the ray again only if the original ray has scattered in the media, otherwise we already have the intersection info.
      if (volume_scatterings>0)
        {
        photon_ray = volume_ray;
        photon_ray.m_base_ray.m_min_t = 0.0;
        photon_ray.m_base_ray.m_max_t = DBL_INF;
        surface_hit = mp_scene->Intersect(photon_ray, photon_isect, &isect_t);
        }
      }

    //////////////////////// Now handle the surface interaction ////////////////////////
    if (surface_hit == false)
      break;

    photon_ray.m_base_ray.m_max_t=isect_t;

    Vector3D_d incident = photon_ray.m_base_ray.m_direction*(-1.0);
    const BSDF *p_photon_BSDF = photon_isect.mp_primitive->GetBSDF(photon_isect.m_dg, photon_isect.m_triangle_index, *p_pool);
    bool has_non_specular = p_photon_BSDF->GetComponentsNum(BxDFType(BSDF_ALL & ~BSDF_SPECULAR)) > 0;

    // Deposit photon at surface.
    Photon photon(Convert<float>(photon_isect.m_dg.m_point), Convert<float>(weight), CompressedDirection(incident), CompressedDirection(p_photon_BSDF->GetGeometricNormal()));
    if (scatterings == 0)
      {
      if (has_non_specular)
        ip_chunk->m_direct_photons.push_back(photon);
      }
    else if (specular_path)
      {
      if (has_non_specular)
        ip_chunk->m_caustic_photons.push_back(photon);
      }
    else
      {
      /*
      Important!
      We deposit indirect photons even on specular surfaces.
      They are used later at the final gathering step when specular surfaces are approximated by a lambertian one.
      Although this brings error to the final image it is usually not so bad since indirect photons are pretty equally distributed in the scene.
      This is probably the fastest method to deal with this kind of problem (e.g. pbrt does not account for this at all by returning black radiance for such final gather rays).
      */
      ip_chunk->m_indirect_photons.push_back(photon);
      }

    // Get random numbers for sampling outgoing photon direction.
    Point2D_d bsdf_sample;
    double component_sample;

    // We use low-discrepancy samples only for the first scattering because further scatterings do not really gain from good stratification.
    if (scatterings == 0)
      {
      bsdf_sample = Point2D_d(SamplingRoutines::RadicalInverse((unsigned int)i_path_index+1, 17), SamplingRoutines::RadicalInverse((unsigned int)i_path_index+1, 19));
      component_sample = SamplingRoutines::RadicalInverse((unsigned int)i_path_index+1, 23);
      }
    else
      {
      bsdf_sample = Point2D_d((*p_rng)(1.0), (*p_rng)(1.0));
      component_sample = (*p_rng)(1.0);
      }

    // Sample new photon ray direction.
    Vector3D_d exitant;
    double bsdf_pdf;
    BxDFType sampled_type;
    SpectrumCoef_d bsdf = p_photon_BSDF->Sample(incident, exitant, bsdf_sample, component_sample, bsdf_pdf, sampled_type);
    if (bsdf_pdf == 0.0)
      break;

    SpectrumCoef_d weight_coef = bsdf / bsdf_pdf;

    // We do not multiply the bsdf by the cosine factor for specular scattering; this is already accounted for in the corresponding BxDFs.
    if (IsSpecular(sampled_type) == false)
      weight_coef *= fabs(exitant * p_photon_BSDF->GetShadingNormal());

    // Possibly terminate photon path with Russian roulette.
    // We use the termination probability equal to the luminance change due to the scattering.
    // The effect is that the weight's luminance won't change if the pass is not terminated.
    double continue_probability = std::min(1.0, SpectrumRoutines::Luminance(weight_coef) );
    if ((*p_rng)(1.0) > continue_probability)
      break;

    weight *= weight_coef / continue_probability;
    specular_path = specular_path && IsSpecular(sampled_type);
    ++scatterings;

    RayDifferential scattered_ray(Ray(photon_isect.m_dg.m_point, exitant));
    if (sampled_type==BxDFType(BSDF_REFLECTION | BSDF_SPECULAR))
      CoreUtils::SetReflectedDifferentials(photon_ray, photon_isect.m_dg, scattered_ray);
    else if (sampled_type==BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR))
      CoreUtils::SetTransmittedDifferentials(photon_ray, photon_isect.m_dg, p_photon_BSDF->GetRefractiveIndex(), scattered_ray);
    else
      scattered_ray = _ScatterRayDiffusely(photon_ray, isect_t, exitant);

    photon_ray = scattered_ray;
    photon_ray.m_base_ray.m_min_t = CoreUtils::GetNextMinT(photon_isect, exitant);
    }

  // Free all allocated objects since we don't need them anymore at this point.
  p_pool->FreeAll();
  }

RayDifferential PhotonLTEIntegrator::PhotonsShootingFilter::_ScatterRayDiffusely(const RayDifferential &i_ray, double i_t, const Vector3D_d &i_directon) const
  {
  ASSERT(i_ray.m_has_differentials);
  Point3D_d point = i_ray.m_base_ray(i_t), point_dx = i_ray.m_origin_dx+i_ray.m_direction_dx*i_t, point_dy = i_ray.m_origin_dy + i_ray.m_direction_dy*i_t;
  Transform transform = MakeMatchDirections(i_ray.m_base_ray.m_direction, i_directon);

  RayDifferential ret;
  ret.m_base_ray = Ray(point, i_directon);
  ret.m_origin_dx = point + transform(Vector3D_d(point_dx-point));
  ret.m_direction_dx = transform(i_ray.m_direction_dx);
  ret.m_origin_dy = point + transform(Vector3D_d(point_dy-point));
  ret.m_direction_dy = transform(i_ray.m_direction_dy);
  ret.m_has_differentials = true;

  return ret;
  }

void PhotonLTEIntegrator::PhotonsShootingFilter::_SplitAndAddPhotonBeam(const RayDifferential &i_ray, const Spectrum_d &i_weight, PhotonsChunk *ip_chunk) const
  {
  if (i_weight.IsBlack()) return;

  RayDifferential ray(i_ray);
  double media_step_size = mp_integrator->m_params.m_media_step_size;
  const VolumeRegion *p_volume = mp_scene->GetVolumeRegion_RawPtr();

  Spectrum_d weight(i_weight);
  SpectrumCoef_d optical_thickness;
  double t = ray.m_base_ray.m_min_t, offset = RandomDouble(1.0);
  while (t+DBL_EPS < ray.m_base_ray.m_max_t)
    {
    Point3D_d origin = ray.m_base_ray(t);
    Point3D_d origin_dx = ray.m_origin_dx + ray.m_direction_dx*t, origin_dy = ray.m_origin_dy + ray.m_direction_dy*t;
    double e1 = Vector3D_d(origin_dx-origin).Length(), e2 = Vector3D_d(origin_dy-origin).Length();
    double major_axis = 3*std::max(e1, e2);

    PhotonBeam beam;
    beam.m_origin = Convert<float>(origin);
    beam.m_direction = Convert<float>(ray.m_base_ray.m_direction);
    beam.m_radius_begin = (float)major_axis;
    beam.m_power_begin = Convert<float>(weight * Exp(-1.0*optical_thickness));

    double step = std::min( (major_axis < DBL_EPS ? media_step_size : 2.0*major_axis), ray.m_base_ray.m_max_t-t );
    beam.m_distance = (float)step;

    origin = ray.m_base_ray(t+step);
    origin_dx = ray.m_origin_dx + ray.m_direction_dx*(t+step);
    origin_dy = ray.m_origin_dy + ray.m_direction_dy*(t+step);
    major_axis = 3*std::max(Vector3D_d(origin_dx-origin).Length(), Vector3D_d(origin_dy-origin).Length());
    beam.m_radius_end = (float)major_axis;

    optical_thickness += p_volume->OpticalThickness(Ray(ray.m_base_ray(t), ray.m_base_ray.m_direction, 0.0, step), media_step_size, offset);
    beam.m_power_end = Convert<float>(weight * Exp(-1.0*optical_thickness));
    t += step;

    ip_chunk->m_photon_beams.push_back(beam);
    }
  }

//////////////////////////////////////// PhotonsMergingFilter /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

PhotonLTEIntegrator::PhotonsMergingFilter::PhotonsMergingFilter(shared_ptr<PhotonMaps> ip_photon_maps) : tbb::filter(serial_out_of_order),
mp_photon_maps(ip_photon_maps)
  {
  ASSERT(ip_photon_maps);
  }

void* PhotonLTEIntegrator::PhotonsMergingFilter::operator()(void* ip_chunk)
  {
  PhotonsChunk *p_chunk = static_cast<PhotonsChunk*>(ip_chunk);

  mp_photon_maps->AddPhotons(p_chunk->m_caustic_photons, p_chunk->m_direct_photons, p_chunk->m_indirect_photons, p_chunk->m_paths_num);

  mp_photon_maps->AddPhotonBeams(p_chunk->m_photon_beams);

  // Release the chunk.
  p_chunk->m_available = true;

  return NULL;
  }

///////////////////////////////////////////// PhotonMaps //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

PhotonLTEIntegrator::PhotonMaps::PhotonMaps(intrusive_ptr<const Scene> ip_scene, size_t i_max_caustic_photons,
                                            size_t i_max_direct_photons, size_t i_max_indirect_photons) : mp_scene(ip_scene), m_photon_paths(0)
  {
    m_max_caustic_photons = i_max_caustic_photons == 0 ? MAX_PHOTONS_IN_MAP : std::min(i_max_caustic_photons, MAX_PHOTONS_IN_MAP);
    m_max_direct_photons = i_max_direct_photons == 0 ? MAX_PHOTONS_IN_MAP : std::min(i_max_direct_photons, MAX_PHOTONS_IN_MAP);
    m_max_indirect_photons = i_max_indirect_photons == 0 ? MAX_PHOTONS_IN_MAP : std::min(i_max_indirect_photons, MAX_PHOTONS_IN_MAP);
  }

void PhotonLTEIntegrator::PhotonMaps::AddPhotons(const std::vector<Photon> &i_caustic_photons, const std::vector<Photon> &i_direct_photons,
                                                 const std::vector<Photon> &i_indirect_photons, size_t i_paths)
  {
  m_photon_paths += i_paths;

  // Add caustic photons
  if (mp_caustic_map)
    _AddPhotonsToKDTree(mp_caustic_map, i_caustic_photons);
  else
    if (m_caustic_photons.size() >= m_max_caustic_photons)
      {
      mp_caustic_map.reset( new KDTree<Photon>(m_caustic_photons) );
      m_caustic_photons.swap(std::vector<Photon>());

      _AddPhotonsToKDTree(mp_caustic_map, i_caustic_photons);
      }
    else
      m_caustic_photons.insert(m_caustic_photons.end(), i_caustic_photons.begin(), i_caustic_photons.end());

  // Add direct photons
  if (mp_direct_map)
    _AddPhotonsToKDTree(mp_direct_map, i_direct_photons);
  else
    if (m_direct_photons.size() >= m_max_direct_photons)
      {
      mp_direct_map.reset( new KDTree<Photon>(m_direct_photons) );
      m_direct_photons.swap(std::vector<Photon>());

      _AddPhotonsToKDTree(mp_direct_map, i_direct_photons);
      }
    else
      m_direct_photons.insert(m_direct_photons.end(), i_direct_photons.begin(), i_direct_photons.end());

  // Add indirect photons
  if (mp_indirect_map)
    _AddPhotonsToKDTree(mp_indirect_map, i_indirect_photons);
  else
    if (m_indirect_photons.size() >= m_max_indirect_photons)
      {
      mp_indirect_map.reset( new KDTree<Photon>(m_indirect_photons) );
      m_indirect_photons.swap(std::vector<Photon>());

      _AddPhotonsToKDTree(mp_indirect_map, i_indirect_photons);
      }
    else
      m_indirect_photons.insert(m_indirect_photons.end(), i_indirect_photons.begin(), i_indirect_photons.end());
  }

void PhotonLTEIntegrator::PhotonMaps::AddPhotonBeams(const std::vector<PhotonBeam> &i_photon_beams)
  {
  m_photon_beams.insert(m_photon_beams.end(), i_photon_beams.begin(), i_photon_beams.end());
  }

shared_ptr<const KDTree<PhotonLTEIntegrator::Photon>> PhotonLTEIntegrator::PhotonMaps::GetCausticMap()
  {
  if (mp_caustic_map==NULL)
    {
    mp_caustic_map.reset( new KDTree<Photon>(m_caustic_photons) );
    m_caustic_photons.swap(std::vector<Photon>());
    }
  return mp_caustic_map;
  }

shared_ptr<const KDTree<PhotonLTEIntegrator::Photon>> PhotonLTEIntegrator::PhotonMaps::GetDirectMap()
  {
  if (mp_direct_map==NULL)
    {
    mp_direct_map.reset( new KDTree<Photon>(m_direct_photons) );
    m_direct_photons.swap(std::vector<Photon>());
    }
  return mp_direct_map;
  }

shared_ptr<const KDTree<PhotonLTEIntegrator::Photon>> PhotonLTEIntegrator::PhotonMaps::GetIndirectMap()
  {
  if (mp_indirect_map==NULL)
    {
    mp_indirect_map.reset( new KDTree<Photon>(m_indirect_photons) );
    m_indirect_photons.swap(std::vector<Photon>());
    }
  return mp_indirect_map;
  }

shared_ptr<const PhotonLTEIntegrator::PhotonBeamAccelerator> PhotonLTEIntegrator::PhotonMaps::GetBeamsMap()
  {
  if (mp_beam_map==NULL)
    {
    printf("Beams recorded : %d\n", m_photon_beams.size());
    mp_beam_map.reset(new PhotonBeamAccelerator(mp_scene->GetVolumeRegion(), std::move(m_photon_beams)));
    }
  return mp_beam_map;
  }

void PhotonLTEIntegrator::PhotonMaps::_AddPhotonsToKDTree(shared_ptr<KDTree<Photon>> ip_map, const std::vector<Photon> &i_photons) const
  {
  ASSERT(ip_map);

  for(size_t i=0;i<i_photons.size();++i)
    {
    const Photon &photon = i_photons[i];

    PhotonFilter filter(Convert<double>(photon.m_point), photon.m_normal.ToVector3D<double>(), PhotonLTEIntegrator::MAX_NORMAL_DEVIATION_COS);
    const Photon *p_nearest_photon = ip_map->GetNearestPoint(Convert<double>(photon.m_point), filter);
    if (p_nearest_photon == NULL)
      continue;

    /*
    Yes, we use the "dirty" trick with the const_cast.
    This looks to be the best option since KDTree can not provide a method that returns non-constant reference to a point
    because the calling code will be able to change its coordinates (which will invalidate the tree).
    Since we do not change the photon's position and only change its weight it is relatively safe to use the const_cast.
    */
    const_cast<Photon*>(p_nearest_photon)->m_weight += photon.m_weight;
    }
  }