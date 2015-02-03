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

#ifndef PHOTON_INTERNAL_TYPES_H
#define PHOTON_INTERNAL_TYPES_H

#include "../PhotonLTEIntegrator.h"
#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include <Math/RandomGenerator.h>
#include <MAth/CompressedDirection.h>
#include <Raytracer/Core/Spectrum.h>
#include <tbb/pipeline.h>
#include <tbb/tbb.h>
#include <vector>

////////////////////////////////////////////// Photon ///////////////////////////////////////////////////

/**
* Structure describing a single photon.
* Direction and surface normal vectors are packed in a 2-byte representation so that the whole structure takes 28 bytes.
*/
struct PhotonLTEIntegrator::Photon
  {
  Photon()
    {
    }

  Photon(const Point3D_f &i_point, const Spectrum_f &i_weight, const CompressedDirection &i_incident_direction, const CompressedDirection &i_normal):
    m_point(i_point),m_weight(i_weight), m_incident_direction(i_incident_direction), m_normal(i_normal)
    {
    }

  float operator[](unsigned char i_index) const
    {
    return m_point[i_index];
    }

  float &operator[](unsigned char i_index)
    {
    return m_point[i_index];
    }

  Point3D_f m_point;
  Spectrum_f m_weight;
  CompressedDirection m_incident_direction, m_normal;
  };

///////////////////////////////////////// IrradiancePhoton //////////////////////////////////////////////

/**
* Irradiance photon.
* Surface normal vector is packed in a 2-byte representation.
*/
struct PhotonLTEIntegrator::IrradiancePhoton
  {
  IrradiancePhoton()
    {
    }

  IrradiancePhoton(const Point3D_f &i_point, const Spectrum_f &i_external_irradiance, const Spectrum_f &i_internal_irradiance, const CompressedDirection &i_normal):
    m_point(i_point), m_external_irradiance(i_external_irradiance), m_internal_irradiance(i_internal_irradiance), m_normal(i_normal)
    {
    }

  float operator[](unsigned char i_index) const
    {
    return m_point[i_index];
    }

  float &operator[](unsigned char i_index)
    {
    return m_point[i_index];
    }

  Point3D_f m_point;

  /**
  * Surface irradiance on the positive side of the surface.
  */
  Spectrum_f m_external_irradiance;

  /**
  * Surface irradiance on the negative side of the surface.
  */
  Spectrum_f m_internal_irradiance;

  CompressedDirection m_normal;
  };

//////////////////////////////////////////// PhotonFilter ////////////////////////////////////////////////

/**
* This is the filter used by KDTree for filtering photons.
* The filter is basically a predicate defining operator()(const Photon &) method that returns true only for those photons that should be considered for radiance estimation.
* The filter returns true only for photons that satisfy both conditions:
*   - Surface normal associated with the photon should not deviate from the input surface normal more than the specified threshold angle.
*   - There should be a plane embedding the photon and the input surface point with a normal deviating from the input surface normal not more than the specified threshold angle.
* @sa IrradiancePhotonFilter
*/
class PhotonLTEIntegrator::PhotonFilter
  {
  public:
    PhotonFilter(const Point3D_d &i_surface_point, const Vector3D_d &i_surface_normal, double i_cos_threshold):
      m_surface_point(i_surface_point), m_surface_normal(i_surface_normal), m_cos_threshold(i_cos_threshold)
      {
      ASSERT(i_surface_normal.IsNormalized());
      ASSERT(i_cos_threshold>=0.0 && i_cos_threshold<=1.0);
      m_sin_threshold = sqrt(1.0-m_cos_threshold*m_cos_threshold);
      }

    bool operator()(const Photon &i_photon)
      {
      ASSERT(i_photon.m_normal.ToVector3D<double>().IsNormalized());
      Vector3D_d to_photon = Vector3D_d(Convert<double>(i_photon.m_point)-m_surface_point);

      return (i_photon.m_normal.ToVector3D<double>() * m_surface_normal) > m_cos_threshold &&
        fabs( to_photon * m_surface_normal ) < m_sin_threshold * to_photon.Length();
      }

  private:
    Point3D_d m_surface_point;
    Vector3D_d m_surface_normal;
    double m_cos_threshold, m_sin_threshold;
  };

/////////////////////////////////////// IrradiancePhotonFilter ///////////////////////////////////////////

/**
* This is the filter used by KDTree for filtering irradiance photons.
* The filter is basically a predicate defining operator()(const Photon &) method that returns true only for those photons that should be considered for radiance estimation.
* The filter returns true only for photons that satisfy both conditions:
*   - Surface normal associated with the photon should not deviate from the input surface normal more than the specified threshold angle.
*   - There should be a plane embedding the photon and the input surface point with a normal deviating from the input surface normal not more than the specified threshold angle.
* @sa PhotonFilter
*/
class PhotonLTEIntegrator::IrradiancePhotonFilter
  {
  public:
    IrradiancePhotonFilter(const Point3D_d &i_surface_point, const Vector3D_d &i_surface_normal, double i_cos_threshold):
      m_surface_point(i_surface_point), m_surface_normal(i_surface_normal), m_cos_threshold(i_cos_threshold)
      {
      ASSERT(i_surface_normal.IsNormalized());
      ASSERT(i_cos_threshold>=0.0 && i_cos_threshold<=1.0);
      m_sin_threshold = sqrt(1.0-m_cos_threshold*m_cos_threshold);
      }

    bool operator()(const IrradiancePhoton &i_photon)
      {
      ASSERT(i_photon.m_normal.ToVector3D<double>().IsNormalized());
      Vector3D_d to_photon = Vector3D_d(Convert<double>(i_photon.m_point)-m_surface_point);

      return (i_photon.m_normal.ToVector3D<double>() * m_surface_normal) > m_cos_threshold &&
        fabs( to_photon * m_surface_normal ) < m_sin_threshold * to_photon.Length();
      }

  private:
    Point3D_d m_surface_point;
    Vector3D_d m_surface_normal;
    double m_cos_threshold, m_sin_threshold;
  };

///////////////////////////////////////////// PhotonMaps //////////////////////////////////////////////////

/**
* The class contains caustic, direct and indirect photon maps.
* It is used by the photon shooting TBB pipeline as the storage for all found photons.
* The class provides methods to merge a vector of photons into a specific photon map.
* 
* For each photon map the merged photons are added to an internal vector of photons until the vector reaches the size threshold (see MAX_PHOTONS_IN_MAP constant).
* After that the KDTree is built from that vector of photons. For all future photons to be merged the nearest photon from the tree is found and the photon's
* weight is simply added to the photon from the tree.
* That allows us to keep the size of the photon maps reasonable while still being able to shoot as many photon paths as needed.
*/
class PhotonLTEIntegrator::PhotonMaps
  {
  public:
    PhotonMaps();

    void AddCausticPhotons(const std::vector<Photon> &i_photons, size_t i_paths);

    void AddDirectPhotons(const std::vector<Photon> &i_photons, size_t i_paths);

    void AddIndirectPhotons(const std::vector<Photon> &i_photons, size_t i_paths);

    /**
    * Returns caustic photons map.
    * The method builds the KDTree if it is not built yet.
    */
    shared_ptr<const KDTree<Photon>> GetCausticMap();

    /**
    * Returns direct photons map.
    * The method builds the KDTree if it is not built yet.
    */
    shared_ptr<const KDTree<Photon>> GetDirectMap();

    /**
    * Returns indirect photons map.
    * The method builds the KDTree if it is not built yet.
    */
    shared_ptr<const KDTree<Photon>> GetIndirectMap();

    size_t GetNumberOfCausticPhotons() const { return m_caustic_photons_found; }

    size_t GetNumberOfDirectPhotons() const { return m_direct_photons_found; }

    size_t GetNumberOfIndirectPhotons() const { return m_indirect_photons_found; }

    size_t GetNumberOfCausticPaths() const { return m_caustic_paths; }

    size_t GetNumberOfDirectPaths() const { return m_direct_paths; }

    size_t GetNumberOfIndirectPaths() const { return m_indirect_paths; }

  private:
    std::vector<Photon> m_caustic_photons, m_direct_photons, m_indirect_photons;

    shared_ptr<KDTree<Photon>> mp_caustic_map, mp_direct_map, mp_indirect_map;

    // Current size of the photon vectors. We use atomic variables because std::vector::size() method is not thread-safe.
    tbb::atomic<size_t> m_caustic_photons_found, m_direct_photons_found, m_indirect_photons_found;

    size_t m_caustic_paths, m_direct_paths, m_indirect_paths;
  };

/////////////////////////////////////// IrradiancePhotonProcess ///////////////////////////////////////////

/**
* This class is a functor used by the TBB loop for estimating irradiance values for irradiance photons.
*/
class PhotonLTEIntegrator::IrradiancePhotonProcess
  {
  public:
    IrradiancePhotonProcess(
      const PhotonLTEIntegrator *ip_integrator, std::vector<IrradiancePhoton> &i_irradiance_photons,
      double i_max_caustic_lookup_dist, double i_max_direct_lookup_dist, double i_max_indirect_lookup_dist) :
      mp_integrator(ip_integrator), m_irradiance_photons(i_irradiance_photons),
      m_max_caustic_lookup_dist(i_max_caustic_lookup_dist), m_max_direct_lookup_dist(i_max_direct_lookup_dist), m_max_indirect_lookup_dist(i_max_indirect_lookup_dist)
      {
      ASSERT(ip_integrator);

      mp_nearest_photons = new NearestPhoton[PhotonLTEIntegrator::LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE];
      }

    /**
    * Copy constructor.
    * Each copy should have its own mp_nearest_photons array since it can't be shared by multiple threads.
    */
    IrradiancePhotonProcess(const IrradiancePhotonProcess &i_process) :
      mp_integrator(i_process.mp_integrator), m_irradiance_photons(i_process.m_irradiance_photons),
      m_max_caustic_lookup_dist(i_process.m_max_caustic_lookup_dist), m_max_direct_lookup_dist(i_process.m_max_direct_lookup_dist), m_max_indirect_lookup_dist(i_process.m_max_indirect_lookup_dist)
      {
      mp_nearest_photons = new NearestPhoton[PhotonLTEIntegrator::LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE];
      }

    void operator()(const tbb::blocked_range<size_t> &i_range) const
      {
      for (size_t i=i_range.begin(); i!=i_range.end(); ++i)
        {
        Point3D_d point = Convert<double>(m_irradiance_photons[i].m_point);
        Vector3D_d normal = m_irradiance_photons[i].m_normal.ToVector3D<double>();

        std::pair<Spectrum_f, Spectrum_f> caustic_irradiance = mp_integrator->_LookupPhotonIrradiance(
          point, normal, mp_integrator->mp_photon_maps->GetCausticMap(), mp_integrator->mp_photon_maps->GetNumberOfCausticPaths(), m_max_caustic_lookup_dist, mp_nearest_photons);

        std::pair<Spectrum_f, Spectrum_f> direct_irradiance = mp_integrator->_LookupPhotonIrradiance(
          point, normal, mp_integrator->mp_photon_maps->GetDirectMap(), mp_integrator->mp_photon_maps->GetNumberOfDirectPaths(), m_max_direct_lookup_dist, mp_nearest_photons);

        std::pair<Spectrum_f, Spectrum_f> indirect_irradiance = mp_integrator->_LookupPhotonIrradiance(
          point, normal, mp_integrator->mp_photon_maps->GetIndirectMap(), mp_integrator->mp_photon_maps->GetNumberOfIndirectPaths(), m_max_indirect_lookup_dist, mp_nearest_photons);

        m_irradiance_photons[i].m_external_irradiance = caustic_irradiance.first+direct_irradiance.first+indirect_irradiance.first;
        m_irradiance_photons[i].m_internal_irradiance = caustic_irradiance.second+direct_irradiance.second+indirect_irradiance.second;
        }
      }

    ~IrradiancePhotonProcess()
      {
      delete[] mp_nearest_photons;
      }

  private:
    const PhotonLTEIntegrator *mp_integrator;
    std::vector<IrradiancePhoton> &m_irradiance_photons;

    // This array is used for storing nearest photons returned by the KDTree.
    // This array is NOT shared by different copies of the functor.
    NearestPhoton *mp_nearest_photons;

    double m_max_caustic_lookup_dist, m_max_direct_lookup_dist, m_max_indirect_lookup_dist;
  };

//////////////////////////////////////////// PhotonsChunk /////////////////////////////////////////////////

/**
* This is a DTO class used to store the range of photon paths to be traced and resulting photons accumulated during photon shooting.
* The class is passed through the TBB pipeline by PhotonsInputFilter, PhotonsShootingFilter and PhotonsMergingFilter.
* The class also keeps MemoryPool and RandomGenerator instances used by the PhotonsShootingFilter.
* Since the class is used by multiple threads it has a simple locking mechanism implemented by m_available field.
* When PhotonsInputFilter picks a chunk it checks if it is available by checking this field and if it is set the field to false.
* PhotonsMergingFilter releases the chunk by setting it back to true.
* Although this locking strategy is not really thread-safe it works well for PhotonsInputFilter because this filter is serial and multiple
* threads will never race to acquire the lock over the same PhotonsChunk.
* @sa PhotonsInputFilter, PhotonsShootingFilter and PhotonsMergingFilter
*/
struct PhotonLTEIntegrator::PhotonsChunk
  {
  /**
  * Creates PhotonsChunk instance.
  * @param i_rng_seed Seed number for the random generator.
  */
  PhotonsChunk(size_t i_rng_seed) : m_caustic_done(true), m_direct_done(true), m_indirect_done(true), m_available(true)
    {
    mp_rng = new RandomGenerator<double>(i_rng_seed);
    mp_memory_pool = new MemoryPool();
    }

  ~PhotonsChunk()
    {
    delete mp_memory_pool;
    delete mp_rng;
    }

  /*
  * These vectors are used to store photons gathered during tracing photon paths defined by the chunk.
  * These photons will be merged to a shared repository by PhotonsMergingFilter. 
  */
  std::vector<Photon> m_caustic_photons, m_direct_photons, m_indirect_photons;

  // Hold true if required number of photons of the corresponding type have been already gathered.
  bool m_caustic_done, m_direct_done, m_indirect_done;

  /*
  * Index of the first photon path defined by the chunk.
  * All photon paths traced by the integrator have unique indices because these indices are used to generate well-distributed paths.
  */ 
  size_t m_first_path_index;

  // Number of photon paths to be traced.
  size_t m_paths_num;

  // True if the chunk is not being used by the TBB pipeline at the moment.
  bool m_available;

  MemoryPool *mp_memory_pool;

  RandomGenerator<double> *mp_rng;
  };

///////////////////////////////////////// PhotonsInputFilter //////////////////////////////////////////////

/**
* This is the input filter for the TBB pipeline (i.e. the first filter in the chain).
* It picks up an arbitrary available chunk, locks it to prevent other threads from using it, sets the range of the photon paths to be traced and returns it.
* The filter is serial which means that two threads never execute it concurrently.
*/
class PhotonLTEIntegrator::PhotonsInputFilter: public tbb::filter
  {
  public:
    PhotonsInputFilter(shared_ptr<PhotonMaps> ip_photon_maps, size_t i_photon_paths,
                       size_t i_caustic_photons_required, size_t i_direct_photons_required, size_t i_indirect_photons_required,
                       size_t i_number_of_chunks, size_t i_paths_per_chunk);

    ~PhotonsInputFilter();

    void* operator()(void*);

  private:
    shared_ptr<PhotonMaps> mp_photon_maps;
    std::vector<PhotonsChunk*> m_chunks;

    size_t m_paths_required, m_total_paths;
    size_t m_caustic_photons_required, m_direct_photons_required, m_indirect_photons_required;
    size_t m_next_chunk_index, m_paths_per_chunk;
  };

//////////////////////////////////////// PhotonsShootingFilter /////////////////////////////////////////////

/**
* This is the processing filter for the TBB pipeline.
* The filter gets the chunk with the range of the photon paths to be traced set by PhotonsInputFilter filter, traces the paths and stores all found photons in the chunk.
* The filter is parallel which means that it can be executed by multiple threads concurrently.
*/
class PhotonLTEIntegrator::PhotonsShootingFilter: public tbb::filter
  {
  public:
    PhotonsShootingFilter(const PhotonLTEIntegrator *ip_integrator, intrusive_ptr<const Scene> ip_scene, const std::vector<double> &i_lights_CDF, bool i_low_thread_priority);

    void* operator()(void* ip_chunk);

  private:
    const PhotonLTEIntegrator *mp_integrator;
    intrusive_ptr<const Scene> mp_scene;
    std::vector<double> m_lights_CDF;
    bool m_low_thread_priority;
  };

//////////////////////////////////////// PhotonsMergingFilter /////////////////////////////////////////////

/**
* This is the output filter for the TBB pipeline.
* The filter gets the chunk with the found photons and merges them to the shared repository (in PhotonMaps instance).
* The filter is serial which means that two threads never execute it concurrently.
*/
class PhotonLTEIntegrator::PhotonsMergingFilter: public tbb::filter
  {
  public:
    PhotonsMergingFilter(shared_ptr<PhotonMaps> ip_photon_maps);

    void* operator()(void* ip_chunk);

  private:
    shared_ptr<PhotonMaps> mp_photon_maps;
  };

#endif // PHOTON_LTE_INTEGRATOR_H