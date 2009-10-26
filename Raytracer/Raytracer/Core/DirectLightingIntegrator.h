#ifndef DIRECT_LIGHTING_INTEGRATOR_H
#define DIRECT_LIGHTING_INTEGRATOR_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include "Renderer.h"
#include "Scene.h"
#include "BSDF.h"
#include "Sample.h"
#include "Sampler.h"
#include "Intersection.h"
#include "LightsSamplingStrategy.h"
#include <vector>
#include <utility>

/**
* Computes direct illumination at a surface point.
* The direct illumination is computed by sampling all scene lights and BSDF specified number of times.
* Multiple importance sampling is used to sample the integral with different PDFs.
* The class uses pluggable LightsSamplingStrategy implementation which defines the probability for each light to be sampled.
*/
class DirectLightingIntegrator: public ReferenceCounted
  {
  private:
    struct DirectLightingSamples;

  public:
    /**
    * Creates DirectLightingIntegrator instance with the specified number of lights and BSDF samples and the specified light sampling strategy.
    * The strategy defines the probability for each light to be sampled. If NULL, the default power-based implementation will be used.
    * Depending on the sampler, the actual number of lights and BSDF samples can be greater then the ones requested.
    * @param ip_renderer Renderer instance. Used to get the scene and compute media transmittance.
    * @param i_lights_samples_num Number of lights samples. Should be equal or greater than zero.
    * @param i_bsdf_samples_num Number of BSDF samples. Should be equal or greater than zero.
    * @param ip_lights_sampling_strategy Light sampling strategy implementation. If null, the default power-based implementation will be used.
    */
    DirectLightingIntegrator(intrusive_ptr<Renderer> ip_renderer, size_t i_lights_samples_num, size_t i_bsdf_samples_num, intrusive_ptr<LightsSamplingStrategy> ip_lights_sampling_strategy = NULL);

    /**
    * Requests 1D and 2D samples sequences needed for the direct lighting integrator.
    * This method should be called before the ComputeDirectLighting() method is called, or NULL sample should be passed to ComputeDirectLighting() method otherwise.
    */
    void RequestSamples(intrusive_ptr<Sampler> ip_sampler);

    /**
    * Computes direct lighting for the specified surface point.
    * The method first computes illumination from the delta light sources and then samples the other lights and BSDF to estimate the direct lighting from the other light sources.
    * @param i_intersection Intersection defining the surface point.
    * @param i_view_direction View direction from the surface point. Should be normalized.
    * @param ip_bsdf Defines the scattering properties at the surface point. Should not be NULL.
    * @param ip_sample Sample instance containing requested samples sequences. If NULL, stratified samples will be generated.
    * @param i_pool Memory pool object that is used for allocating the temporary objects.
    * @return Resulting radiance value for the direct lighting.
    */
    Spectrum_d ComputeDirectLighting(const Intersection &i_intersection, const Vector3D_d &i_view_direction,
      const BSDF *ip_bsdf, const Sample *ip_sample, MemoryPool &i_pool) const;

  private:
    // Not implemented, not a value type.
    DirectLightingIntegrator(const DirectLightingIntegrator&);
    DirectLightingIntegrator &operator=(const DirectLightingIntegrator&); 

    /**
    * Helper private method. Estimates direct lighting by sampling infinite and area lights.
    */
    Spectrum_d _SampleLights(const Intersection &i_intersection, const Vector3D_d &i_view_direction,
      const BSDF *ip_bsdf, const DirectLightingSamples &i_samples, double *ip_lights_CDF) const;

    /**
    * Helper private method. Estimates direct lighting by sampling the BSDF.
    */
    Spectrum_d _SampleBSDF(const Intersection &i_intersection, const Vector3D_d &i_view_direction,
      const BSDF *ip_bsdf, const DirectLightingSamples &i_samples, double *ip_lights_CDF) const;

    /**
    * Returns the index of the specified area light in the LightSources::m_area_light_sources vector returned by the Scene.
    * Uses binary search in the cached sorted vector (see m_area_lights_sorted field).
    */
    size_t _GetAreaLightIndex(const AreaLightSource *ip_area_light) const;

  private:
    // Internal types.

    /**
    * Internal structure used as a DTO for samples sequences.
    * Created in ComputeDirectLighting() method and passed to _SampleLights() and _SampleBSDF() methods.
    */
    struct DirectLightingSamples
      {
      SamplesSequence1D m_light_1D_samples, m_bsdf_1D_samples;
      SamplesSequence2D m_light_2D_samples, m_bsdf_2D_samples;
      };

  private:
    intrusive_ptr<Renderer> mp_renderer;
    intrusive_ptr<Scene> mp_scene;
    size_t m_lights_samples_num, m_bsdf_samples_num;

    intrusive_ptr<LightsSamplingStrategy> mp_lights_sampling_strategy;
    std::vector<std::pair<const AreaLightSource *,size_t> > m_area_lights_sorted;

    // IDs of the samples sequences returned by the Sampler.
    size_t m_light_1D_samples_id, m_light_2D_samples_id, m_bsdf_1D_samples_id, m_bsdf_2D_samples_id;

    /**
    * True if the samples have been already requested. Used for asserts only.
    */
    bool m_samples_requested;
  };

#endif // DIRECT_LIGHTING_INTEGRATOR_H