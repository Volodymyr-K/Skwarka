#ifndef PHOTON_LTE_INTEGRATOR_H
#define PHOTON_LTE_INTEGRATOR_H

#include <Common/Common.h>
#include <Raytracer/Core/LTEIntegrator.h>
#include <Raytracer/Core/DirectLightingIntegrator.h>
#include <Raytracer/Core/KDTree.h>

/**
* DTO for parameters for PhotonLTEIntegrator.
* @sa PhotonLTEIntegrator
*/
struct PhotonLTEIntegratorParams
  {
  /**
  * Number of samples for direct lighting estimation.
  * The actual number of shadow rays traced will be twice the number because we sample both BSDF and light sources.
  */
  size_t m_direct_light_samples_num;

  /**
  * Number of nearby caustic photons to be interpolated when estimating caustic radiance.
  */
  size_t m_caustic_lookup_photons_num;

  /**
  * Maximum distance to search caustic photons within.
  */
  double m_max_caustic_lookup_dist;

  /**
  * Number of final gather rays.
  * The actual number of rays traced is actually twice the number because we sample both BSDF and nearby (indirect) photons for final gather directions.
  */
  size_t m_gather_samples_num;

  /**
  * Maximum specular depth for specular reflections and refractions.
  */
  size_t m_max_specular_depth;
  };

/**
* LTEIntegrator implementation that uses photon maps to estimate indirect illumination.
* The integrator has pre-rendering phase for shooting photons (see ShootPhotons() method). All traced photons are divided in three groups: direct photons, indirect photons and caustic photons.
* Direct photons are those that have not undergo any scattering yet, indirect photons are those that have already been scattered non-specularly and caustic photons are those that
* have (only) been scattered specularly yet.
* After shooting all photons the integrator also computes irradiance photons which already store precomputed irradiance value interpolated from nearby direct, indirect and caustic photons.
* The integrator uses final gathering and each final gather ray uses nearest irradiance photon instead of interpolating nearby photons.
* The class uses DirectLightingIntegrator class to compute the direct lighting and traces rays for specular reflection and specular refraction.
* @sa PhotonLTEIntegratorParams
*/
class PhotonLTEIntegrator: public LTEIntegrator
  {
  private:
    struct Photon;
    struct IrradiancePhoton;
    typedef KDTree<Photon>::NearestPoint NearestPhoton;

  public:
    /**
    * Creates PhotonLTEIntegrator instance.
    * @param ip_scene Scene instance. Should not be NULL.
    * @param ip_volume_integrator Volume integrator for computing volume radiance and transmittance. Can be NULL in which case it is assumed there is no participating media.
    * @param i_params Integrator parameters.
    */
    PhotonLTEIntegrator(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<VolumeIntegrator> ip_volume_integrator, PhotonLTEIntegratorParams i_params);

    /**
    * Shoot photons and construct photon maps.
    * The method takes number of photons of each type and traces photons until specified number of photons of each type is store.
    * The method also constructs irradiance photon map.
    * The old maps are cleared.
    * @param i_caustic_photons Minimum number of caustic photons to be stored.
    * @param i_direct_photons Minimum number of direct photons to be stored.
    * @param i_indirect_photons Minimum number of indirect photons to be stored.
    */
    void ShootPhotons(size_t i_caustic_photons, size_t i_direct_photons, size_t i_indirect_photons);

  private:
    class PhotonFilter;
    class IrradiancePhotonFilter;

    // Used by multi-threaded TBB loop for estimating irradiance values for irradiance photons.
    class IrradiancePhotonProcess;

    // Private types, used for multi-threaded photon shooting.
    struct PhotonMaps;
    struct PhotonsChunk;
    class PhotonsInputFilter;
    class PhotonsShootingFilter;
    class PhotonsMergingFilter;

  private:
    /**
    * Requests 1D and 2D samples sequences needed for the surface part of the LTE integration.
    * The method requests samples sequences needed for final gathering.
    * This method also redirects call to the DirectLightingIntegrator::RequestSamples() method.
    */
    virtual void _RequestSamples(intrusive_ptr<Sampler> ip_sampler);

    /**
    * Computes surface radiance along the specified ray.
    * The method will only be called when the ray does intersect some primitive in the scene.
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param i_intersection Intersection of the specified ray with the nearest primitive in the scene.
    * @param ip_sample Sample instance containing requested samples sequences. Can be null.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @return Resulting radiance value.
    */
    virtual Spectrum_d _SurfaceRadiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, MemoryPool &i_pool) const;

    /**
    * Helper private method that traces final gather rays to estimate indirect illumination (caustic aside).
    * The method traces rays based on BSDF's PDF and also based on nearby photon's directions (and combines result via multiple importance sampling).
    */
    Spectrum_d _FinalGather(const Intersection &i_intersection, const Vector3D_d &i_incident, const BSDF *ip_bsdf, const Sample *ip_sample, MemoryPool &i_pool) const;

    /**
    * Estimates caustic radiance by doing lookup in caustic photon map and interpolating nearby photons.
    */
    Spectrum_d _LookupCausticRadiance(const BSDF *ip_bsdf, const DifferentialGeometry &i_dg, const Vector3D_d &i_direction, MemoryPool &i_pool) const;

    /**
    * Computes CDF for sampling lights.
    * The CDF is proportional to lights power.
    */
    void _GetLightsPowerCDF(const LightSources &i_light_sources, std::vector<double> &o_lights_CDF);

    /**
    * Simpson�s kernel function used to interpolate nearby photons.
    * The function returns lower weights for farther photons and bigger weights for near photons.
    */
    double _PhotonKernel(double i_dist_sqr, double i_max_dist_sqr) const;

    /**
    * Estimates irradiance based on the specified photon map.
    * The method returns pair of irradiance values for two sides of the surface.
    */
    std::pair<Spectrum_f, Spectrum_f> _LookupPhotonIrradiance(const Point3D_d &i_point, const Vector3D_d &i_normal,
      shared_ptr<const KDTree<Photon> > ip_photon_map, size_t i_photon_paths, size_t i_lookup_photons_num, double i_max_lookup_dist, NearestPhoton *ip_nearest_photons) const;

    /**
    * Creates irradiance photons and constructs KDTree for them.
    * The method selects a fraction (10%) of all of the indirect photons as irradiance photons positions.
    * The indirect photons are selected so that they are distributed as uniform across the scene as much as possible.
    * The method also estimates maximum lookup distance for irradiance photons.  
    */
    void _ConstructIrradiancePhotonMap(const PhotonMaps &i_maps);

  private:
    /**
    * Number of nearby photons to be interpolated when estimating irradiance photons.
    */
    static const size_t LOOKUP_PHOTONS_NUM_FOR_IRRADIANCE = 100;
    
    /**
    * Defines the maximum number of tokens (for shooting photons) the TBB pipeline can run concurrently.
    * This is also the upper bound on the number of threads the pipeline can utilize concurrently.
    */
    static const size_t MAX_PIPELINE_TOKENS_NUM = 64;

    /**
    * Cosine of the maximum angle between the surface normal and the normals of interpolated nearby photons.
    * Only photons with normals inside the range will be used for estimating radiance value.
    * Besides filtering normals this value is also used to filter photon locations (see implementation details).
    */
    static const double MAX_NORMAL_DEVIATION_COS;

  private:
    intrusive_ptr<const Scene> mp_scene;

    /**
    * Total area of all primitives in the scene.
    * The value is precomputed once in constructor and used later for estimating maximum search radius.
    */
    double m_scene_total_area;

    double m_max_irradiance_lookup_dist;

    intrusive_ptr<DirectLightingIntegrator> mp_direct_lighting_integrator;

    PhotonLTEIntegratorParams m_params;

    // IDs of the samples sequences used for final gathering.
    size_t m_bsdf_1D_samples_id, m_bsdf_2D_samples_id, m_direction_1D_samples_id, m_direction_2D_samples_id;

    // Photon maps.
    shared_ptr<const KDTree<Photon> > mp_caustic_map, mp_direct_map, mp_indirect_map, mp_indirect_specular_map;
    
    // Number of photon paths used to gather photons of the corresponding type.
    size_t m_caustic_paths, m_direct_paths, m_indirect_paths, m_indirect_specular_paths;

    // Irradiance photon map.
    shared_ptr<const KDTree<IrradiancePhoton> > mp_irradiance_map;
  };

#endif // PHOTON_LTE_INTEGRATOR_H