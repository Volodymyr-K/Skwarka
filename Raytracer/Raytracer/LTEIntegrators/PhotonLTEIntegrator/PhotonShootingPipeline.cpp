#include "../PhotonLTEIntegrator.h"
#include "PhotonInternalTypes.h"
#include <Common/MemoryPool.h>
#include <Math/SamplingRoutines.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Core/CoreUtils.h>
#include <tbb/pipeline.h>

///////////////////////////////////////// PhotonsInputFilter //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

PhotonLTEIntegrator::PhotonsInputFilter::PhotonsInputFilter(PhotonMaps *ip_photon_maps, size_t i_caustic_photons_num, size_t i_direct_photons_num, size_t i_indirect_photons_num,
                                                            size_t i_number_of_chunks, size_t i_paths_per_chunk):
filter(serial_out_of_order), mp_photon_maps(ip_photon_maps),
m_caustic_photons_num(i_caustic_photons_num), m_direct_photons_num(i_direct_photons_num), m_indirect_photons_num(i_indirect_photons_num),
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
  bool caustic_done = mp_photon_maps->m_caustic_size >= m_caustic_photons_num;
  bool direct_done = mp_photon_maps->m_direct_size >= m_direct_photons_num;
  bool indirect_done = mp_photon_maps->m_indirect_size >= m_indirect_photons_num;

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
  m_next_chunk_index = (m_next_chunk_index+1) % m_chunks.size();

  if (caustic_done == false) mp_photon_maps->m_caustic_paths += m_paths_per_chunk;
  if (direct_done == false) mp_photon_maps->m_direct_paths += m_paths_per_chunk;
  if (indirect_done == false) mp_photon_maps->m_indirect_paths += m_paths_per_chunk;

  return p_chunk;
  }

/////////////////////////////////////// PhotonsShootingFilter /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

PhotonLTEIntegrator::PhotonsShootingFilter::PhotonsShootingFilter(const PhotonLTEIntegrator *ip_integrator,
                                                                  intrusive_ptr<const Scene> ip_scene, const std::vector<double> &i_lights_CDF):
tbb::filter(parallel), mp_integrator(ip_integrator), mp_scene(ip_scene), m_lights_CDF(i_lights_CDF)
  {
  ASSERT(ip_integrator);
  ASSERT(ip_scene);
  }

void* PhotonLTEIntegrator::PhotonsShootingFilter::operator()(void* ip_chunk)
  {
  PhotonsChunk *p_chunk = static_cast<PhotonsChunk*>(ip_chunk);
  MemoryPool &pool = *p_chunk->mp_memory_pool;
  RandomGenerator<double> *p_rng = p_chunk->mp_rng;

  const LightSources &lights = mp_scene->GetLightSources();

  size_t delta_lights = lights.m_delta_light_sources.size();
  size_t area_lights = lights.m_area_light_sources.size();
  size_t infinitiy_lights = lights.m_infinitiy_light_sources.size();
  size_t num_lights = delta_lights+area_lights+infinitiy_lights;
  ASSERT(num_lights == m_lights_CDF.size());

  if (num_lights == 0)
    {
    ASSERT(0 && "If there are no lights in the scene we should not have got here.");
    return p_chunk;
    }

  BxDFType non_specular_types = BxDFType(BSDF_ALL & ~BSDF_SPECULAR);

  size_t path_begin = p_chunk->m_first_path_index, path_end = p_chunk->m_first_path_index+p_chunk->m_paths_num;
  for (size_t path_index=path_begin; path_index<path_end; ++path_index)
    {
    Point2D_d position_sample(SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 2), SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 3));
    Point2D_d direction_sample(SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 5), SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 7));

    // Binary search for the sampled light source.
    size_t light_index = MathRoutines::BinarySearchCDF(m_lights_CDF.begin(), m_lights_CDF.end(), SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 11)) - m_lights_CDF.begin();
    ASSERT(light_index>=0 && light_index<num_lights);

    // This is the probability of the sampled light source to be sampled.
    double light_pdf = light_index==0 ? m_lights_CDF[0] : m_lights_CDF[light_index]-m_lights_CDF[light_index-1];
    ASSERT(light_pdf > 0.0);

    double photon_pdf = 0.0;
    Ray photon_ray;
    Spectrum_d weight;

    if (light_index < delta_lights)
      weight = lights.m_delta_light_sources[light_index]->SamplePhoton(direction_sample, photon_ray, photon_pdf);
    else if (light_index < delta_lights+infinitiy_lights)
      weight = lights.m_infinitiy_light_sources[light_index-delta_lights]->SamplePhoton(position_sample, direction_sample, photon_ray, photon_pdf);
    else
      weight = lights.m_area_light_sources[light_index-delta_lights-infinitiy_lights]->SamplePhoton((*p_rng)(1.0), position_sample, direction_sample, photon_ray, photon_pdf);

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
      weight *= mp_integrator->_VolumeTransmittance(photon_ray, NULL);

      Vector3D_d incident = photon_ray.m_direction*(-1.0);
      const BSDF *p_photon_BSDF = photon_isect.mp_primitive->GetBSDF(photon_isect.m_dg, photon_isect.m_triangle_index, pool);
      bool has_non_specular = p_photon_BSDF->GetComponentsNum(non_specular_types) > 0;

      // Deposit photon at surface.
      Photon photon(Convert<float>(photon_isect.m_dg.m_point), Convert<float>(weight), CompressedDirection(incident), CompressedDirection(photon_isect.m_dg.m_shading_normal));
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
        These are used later at the final gathering step when specular surfaces are approximated by a lambertian one.
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
        bsdf_sample = Point2D_d(SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 13), SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 17));
        component_sample = SamplingRoutines::RadicalInverse((unsigned int)path_index+1, 19);
        }
      else
        {
        bsdf_sample = Point2D_d((*p_rng)(1.0), (*p_rng)(1.0));
        component_sample = (*p_rng)(1.0);
        }

      Spectrum_d bsdf = p_photon_BSDF->Sample(incident, exitant, bsdf_sample, component_sample, bsdf_pdf, sampled_type);
      if (bsdf_pdf == 0.0)
        break;

      Spectrum_d weight_new = weight * bsdf / bsdf_pdf;

      // We do not multiply the bsdf by the cosine factor for specular scattering; this is already accounted for in the corresponding BxDFs.
      if (IsSpecular(sampled_type) == false)
        weight_new *= fabs(exitant * photon_isect.m_dg.m_shading_normal);

      // Possibly terminate photon path with Russian roulette.
      // We use the termination probability equal to the luminance change due to the scattering.
      // The effect is that the weight's luminance won't change if the pass is not terminated.
      double continue_probability = std::min(1.0, weight_new.Luminance() / weight.Luminance());
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
    pool.FreeAll();
    } // for (size_t path_index=path_begin; path_index<path_end; ++path_index)

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

  // Merge photons.
  mp_photon_maps->m_caustic_photons.insert(mp_photon_maps->m_caustic_photons.end(), p_chunk->m_caustic_photons.begin(), p_chunk->m_caustic_photons.end());
  mp_photon_maps->m_direct_photons.insert(mp_photon_maps->m_direct_photons.end(), p_chunk->m_direct_photons.begin(), p_chunk->m_direct_photons.end());
  mp_photon_maps->m_indirect_photons.insert(mp_photon_maps->m_indirect_photons.end(), p_chunk->m_indirect_photons.begin(), p_chunk->m_indirect_photons.end());

  // Update number of photons in the shared repository.
  mp_photon_maps->m_caustic_size = mp_photon_maps->m_caustic_photons.size();
  mp_photon_maps->m_direct_size = mp_photon_maps->m_direct_photons.size();
  mp_photon_maps->m_indirect_size = mp_photon_maps->m_indirect_photons.size();

  // Release the chunk.
  p_chunk->m_available = true;

  return NULL;
  }