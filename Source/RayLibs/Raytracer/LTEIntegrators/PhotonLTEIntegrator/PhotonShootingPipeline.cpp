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
#include <Common/MemoryPool.h>
#include <Math/SamplingRoutines.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Core/CoreUtils.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <tbb/pipeline.h>

///////////////////////////////////////// PhotonsInputFilter //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

PhotonLTEIntegrator::PhotonsInputFilter::PhotonsInputFilter(PhotonMaps *ip_photon_maps, size_t i_caustic_photons_required, size_t i_direct_photons_required, size_t i_indirect_photons_required,
                                                            size_t i_number_of_chunks, size_t i_paths_per_chunk):
filter(serial_out_of_order), mp_photon_maps(ip_photon_maps),
m_caustic_photons_required(i_caustic_photons_required), m_direct_photons_required(i_direct_photons_required), m_indirect_photons_required(i_indirect_photons_required),
m_paths_per_chunk(i_paths_per_chunk), m_next_chunk_index(0), m_total_paths(0)
  {
  ASSERT(ip_photon_maps);
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
  // We don't get the size by calling std::vector::size() method because it is not thread-safe.
  bool caustic_done = mp_photon_maps->GetNumberOfCausticPhotons() >= m_caustic_photons_required;
  bool direct_done = mp_photon_maps->GetNumberOfDirectPhotons() >= m_direct_photons_required;
  bool indirect_done = mp_photon_maps->GetNumberOfIndirectPhotons() >= m_indirect_photons_required;

  if (caustic_done && direct_done && indirect_done)
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
  p_chunk->m_paths_num = m_paths_per_chunk;
  p_chunk->m_first_path_index = m_total_paths;

  p_chunk->m_caustic_photons.clear();
  p_chunk->m_direct_photons.clear();
  p_chunk->m_indirect_photons.clear();

  p_chunk->m_caustic_done = caustic_done;
  p_chunk->m_direct_done = direct_done;
  p_chunk->m_indirect_done = indirect_done;

  m_total_paths += m_paths_per_chunk;

  return p_chunk;
  }

/////////////////////////////////////// PhotonsShootingFilter /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

PhotonLTEIntegrator::PhotonsShootingFilter::PhotonsShootingFilter(const PhotonLTEIntegrator *ip_integrator,
                                                                  intrusive_ptr<const Scene> ip_scene, const std::vector<double> &i_lights_CDF, bool i_low_thread_priority):
tbb::filter(parallel), mp_integrator(ip_integrator), mp_scene(ip_scene), m_lights_CDF(i_lights_CDF), m_low_thread_priority(i_low_thread_priority)
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
  MemoryPool *p_pool = p_chunk->mp_memory_pool;
  RandomGenerator<double> *p_rng = p_chunk->mp_rng;

  ThreadSpecifics ts;
  ts.mp_pool = p_pool;
  ts.mp_random_generator = p_rng;

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

  BxDFType non_specular_types = BxDFType(BSDF_ALL & ~BSDF_SPECULAR);

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
    Ray photon_ray;
    Spectrum_d weight;

    if (light_index < delta_lights)
      weight = lights.m_delta_light_sources[light_index]->SamplePhoton(direction_sample, photon_ray, photon_pdf);
    else if (light_index < delta_lights+infinite_lights)
      weight = lights.m_infinite_light_sources[light_index-delta_lights]->SamplePhoton(position_sample, direction_sample, photon_ray, photon_pdf);
    else
      weight = lights.m_area_light_sources[light_index - delta_lights - infinite_lights]->SamplePhoton(
        SamplingRoutines::RadicalInverse((unsigned int)path_index + 1, 13), position_sample, direction_sample, photon_ray, photon_pdf);

    if (photon_pdf == 0.0 || weight.IsBlack())
      continue;

    weight /= photon_pdf*light_pdf;
    bool specular_path = true;
    Intersection photon_isect;
    size_t intersections_num = 0;

    double isect_t;
    while (mp_scene->Intersect(RayDifferential(photon_ray), photon_isect, &isect_t))
      {
      ++intersections_num;

      photon_ray.m_max_t=isect_t;
      weight *= mp_integrator->_MediaTransmittance(photon_ray, ts);

      Vector3D_d incident = photon_ray.m_direction*(-1.0);
      const BSDF *p_photon_BSDF = photon_isect.mp_primitive->GetBSDF(photon_isect.m_dg, photon_isect.m_triangle_index, *p_pool);
      bool has_non_specular = p_photon_BSDF->GetComponentsNum(non_specular_types) > 0;

      // Deposit photon at surface.
      Photon photon(Convert<float>(photon_isect.m_dg.m_point), Convert<float>(weight), CompressedDirection(incident), CompressedDirection(p_photon_BSDF->GetGeometricNormal()));
      if (intersections_num == 1)
        {
        if (has_non_specular && p_chunk->m_direct_done==false)
          p_chunk->m_direct_photons.push_back(photon);
        }
      else if (specular_path)
        {
        if (has_non_specular && p_chunk->m_caustic_done==false)
          p_chunk->m_caustic_photons.push_back(photon);
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
        if (p_chunk->m_indirect_done==false)
          p_chunk->m_indirect_photons.push_back(photon);
        }

      // Sample new photon ray direction.
      Vector3D_d exitant;
      double bsdf_pdf;
      BxDFType sampled_type;

      // Get random numbers for sampling outgoing photon direction.
      Point2D_d bsdf_sample;
      double component_sample;

      // We only use low-discrepancy samples for first intersection because further intersections do not really gain from good stratification.
      if (intersections_num == 1)
        {
        bsdf_sample = Point2D_d(SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 17), SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 19));
        component_sample = SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 23);
        }
      else
        {
        bsdf_sample = Point2D_d((*p_rng)(1.0), (*p_rng)(1.0));
        component_sample = (*p_rng)(1.0);
        }

      SpectrumCoef_d bsdf = p_photon_BSDF->Sample(incident, exitant, bsdf_sample, component_sample, bsdf_pdf, sampled_type);
      if (bsdf_pdf == 0.0)
        break;

      Spectrum_d weight_new = weight * bsdf / bsdf_pdf;

      // We do not multiply the bsdf by the cosine factor for specular scattering; this is already accounted for in the corresponding BxDFs.
      if (IsSpecular(sampled_type) == false)
        weight_new *= fabs(exitant * p_photon_BSDF->GetShadingNormal());

      // Possibly terminate photon path with Russian roulette.
      // We use the termination probability equal to the luminance change due to the scattering.
      // The effect is that the weight's luminance won't change if the pass is not terminated.
      double continue_probability = std::min(1.0, SpectrumRoutines::Luminance(weight_new) / SpectrumRoutines::Luminance(weight));
      if ((*p_rng)(1.0) > continue_probability)
        break;

      weight = weight_new / continue_probability;
      bool previous_specular = (sampled_type & BSDF_SPECULAR) != 0;
      specular_path = previous_specular && specular_path;

      if (specular_path == false && p_chunk->m_indirect_done)
        break;

      photon_ray = Ray(photon_isect.m_dg.m_point, exitant, CoreUtils::GetNextMinT(photon_isect, exitant));
      } // while (mp_scene->Intersect(photon_ray, photon_isect, &isect_t))
  
    // Free all allocated objects since we don't need them anymore at this point.
    p_pool->FreeAll();
    } // for (size_t path_index=path_begin; path_index<path_end; ++path_index)

  if (m_low_thread_priority)
    CoreUtils::SetCurrentThreadPriority(prev_thread_priority);
  return p_chunk;
  }

//////////////////////////////////////// PhotonsMergingFilter /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

PhotonLTEIntegrator::PhotonsMergingFilter::PhotonsMergingFilter(PhotonMaps *ip_photon_maps): tbb::filter(serial_out_of_order),
mp_photon_maps(ip_photon_maps)
  {
  ASSERT(ip_photon_maps);
  }

void* PhotonLTEIntegrator::PhotonsMergingFilter::operator()(void* ip_chunk)
  {
  PhotonsChunk *p_chunk = static_cast<PhotonsChunk*>(ip_chunk);

  /*
  We add photons even if the photon maps already contain enough photons, because since we have already done
  all the expensive computation, we can add the photons just as well.
  */
  mp_photon_maps->AddCausticPhotons(p_chunk->m_caustic_photons, p_chunk->m_paths_num);
  mp_photon_maps->AddDirectPhotons(p_chunk->m_direct_photons, p_chunk->m_paths_num);
  mp_photon_maps->AddIndirectPhotons(p_chunk->m_indirect_photons, p_chunk->m_paths_num);

  // Release the chunk.
  p_chunk->m_available = true;

  return NULL;
  }

///////////////////////////////////////////// PhotonMaps //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

PhotonLTEIntegrator::PhotonMaps::PhotonMaps()
  {
  m_caustic_photons_found = 0;
  m_direct_photons_found = 0;
  m_indirect_photons_found = 0;
  m_caustic_paths = m_direct_paths = m_indirect_paths = 0;
  }

size_t PhotonLTEIntegrator::PhotonMaps::GetNumberOfCausticPhotons() const
  {
  return m_caustic_photons_found;
  }

size_t PhotonLTEIntegrator::PhotonMaps::GetNumberOfDirectPhotons() const
  {
  return m_direct_photons_found;
  }

size_t PhotonLTEIntegrator::PhotonMaps::GetNumberOfIndirectPhotons() const
  {
  return m_indirect_photons_found;
  }

void PhotonLTEIntegrator::PhotonMaps::AddCausticPhotons(const std::vector<Photon> &i_photons, size_t i_paths)
  {
  m_caustic_paths += i_paths;
  m_caustic_photons_found += i_photons.size();

  if (mp_caustic_map)
    _AddPhotonsToKDTree(mp_caustic_map, i_photons);
  else
    if (m_caustic_photons_found >= PhotonLTEIntegrator::PhotonMaps::MAX_PHOTONS_IN_MAP)
      {
      mp_caustic_map.reset( new KDTree<Photon>(m_caustic_photons) );
      m_caustic_photons.swap(std::vector<Photon>());

      _AddPhotonsToKDTree(mp_caustic_map, i_photons);
      }
    else
      m_caustic_photons.insert(m_caustic_photons.end(), i_photons.begin(), i_photons.end());
  }

void PhotonLTEIntegrator::PhotonMaps::AddDirectPhotons(const std::vector<Photon> &i_photons, size_t i_paths)
  {
  m_direct_paths += i_paths;
  m_direct_photons_found += i_photons.size();

  if (mp_direct_map)
    _AddPhotonsToKDTree(mp_direct_map, i_photons);
  else
    if (m_direct_photons_found >= PhotonLTEIntegrator::PhotonMaps::MAX_PHOTONS_IN_MAP)
      {
      mp_direct_map.reset( new KDTree<Photon>(m_direct_photons) );
      m_direct_photons.swap(std::vector<Photon>());

      _AddPhotonsToKDTree(mp_direct_map, i_photons);
      }
    else
      m_direct_photons.insert(m_direct_photons.end(), i_photons.begin(), i_photons.end());
  }

void PhotonLTEIntegrator::PhotonMaps::AddIndirectPhotons(const std::vector<Photon> &i_photons, size_t i_paths)
  {
  m_indirect_paths += i_paths;
  m_indirect_photons_found += i_photons.size();

  if (mp_indirect_map)
    _AddPhotonsToKDTree(mp_indirect_map, i_photons);
  else
    if (m_indirect_photons_found >= PhotonLTEIntegrator::PhotonMaps::MAX_PHOTONS_IN_MAP)
      {
      mp_indirect_map.reset( new KDTree<Photon>(m_indirect_photons) );
      m_indirect_photons.swap(std::vector<Photon>());

      _AddPhotonsToKDTree(mp_indirect_map, i_photons);
      }
    else
      m_indirect_photons.insert(m_indirect_photons.end(), i_photons.begin(), i_photons.end());
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