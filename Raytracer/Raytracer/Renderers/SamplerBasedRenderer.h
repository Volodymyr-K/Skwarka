#ifndef SAMPLER_BASED_RENDERER_H
#define SAMPLER_BASED_RENDERER_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Renderer.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/Sample.h>
#include <Raytracer/Core/Sampler.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/Core/Scene.h>
#include <Raytracer/Core/SurfaceIntegrator.h>
#include <Raytracer/Core/VolumeIntegrator.h>

/**
* Renders image by shooting camera rays for each camera sample generated by Sampler.
* The renderer combines surface and volume integrators to compute the complete solution of the light transport equation
* taking into account both the surface and volume scattering.
* The class contains SurfaceIntegrator and VolumeIntegrator instances used to compute the surface scattering and volume scattering respectively.
* The volume integrator can be not set, in this case the volume scattering will not be computed.
*/
class SamplerBasedRenderer: public Renderer
  {
  public:
    /**
    * Creates SamplerBasedRenderer instance for the specified scene. The logger implementation must be thread-safe.
    */
    SamplerBasedRenderer(intrusive_ptr<Scene> ip_scene, intrusive_ptr<Sampler> ip_sampler, intrusive_ptr<Log> ip_log = NULL);

    /**
    * Sets surface integrator to be used for computing surface scattering.
    * A non-null surface integrator should be set before calling Radiance() method.
    */
    void SetSurfaceIntegrator(intrusive_ptr<SurfaceIntegrator> ip_surface_integrator);

    /**
    * Sets volume integrator to be used for computing volume scattering.
    * If the volume integrator is not set the volume scattering will not be computed.
    */
    void SetVolumeIntegrator(intrusive_ptr<VolumeIntegrator> ip_volume_integrator);

    /**
    * Renders the scene for the specified camera.
    * The rendered image is saved to the camera's film. The film is cleared before rendering, so the previous image will be lost.
    * @param ip_camera Camera in the scene for which the image is to be rendered.
    */
    virtual void Render(intrusive_ptr<Camera> ip_camera) const;

    /**
    * Computes radiance for the the specified ray.
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param ip_sample Sample instance containing requested samples sequences.
    * Can be null, in this case this is up to the implementation how to generate sample values.
    * @param i_pool Memory pool object that is used for allocating temporary objects.
    * @return Resulting radiance value.
    */
    virtual Spectrum_d Radiance(const RayDifferential &i_ray, const Sample *ip_sample, MemoryPool &i_pool) const;

    /**
    * Computes media transmittance for the specified ray.
    * This method accounts for the media transmittance only and does not account for primitives intersected by the ray.
    * @param i_ray Ray for which the transmittance is to be computed. The direction component of the ray should be normalized.
    * @param ip_sample Sample instance containing requested samples sequences.
    * Can be null, in this case this is up to the implementation how to generate sample values.
    * @return Resulting transmittance value. Each spectrum component will be in [0;1] range.
    */
    virtual Spectrum_d Transmittance(const Ray &i_ray, const Sample *ip_sample) const;

  private:
    // Not implemented, not a value type.
    SamplerBasedRenderer(const SamplerBasedRenderer&);
    SamplerBasedRenderer &operator=(const SamplerBasedRenderer&);

  private:
    // Private types, used for multi-threaded rendering.
    class SamplesChunk;
    class SamplesGeneratorFilter;
    class IntegratorFilter;
    class FilmWriterFilter;

  private:
    intrusive_ptr<Scene> mp_scene;

    intrusive_ptr<Sampler> mp_sampler;

    intrusive_ptr<SurfaceIntegrator> mp_surface_integrator;

    intrusive_ptr<VolumeIntegrator> mp_volume_integrator;

    intrusive_ptr<Log> mp_log;

    // Defines the maximum number of tokens the TBB pipeline can run concurrently.
    // This is also the upper bound on the number of threads the pipeline can utilize at the same time.
    static const size_t MAX_PIPELINE_TOKENS_NUM = 16;

    // Defines the number of samples in each SamplesChunk.
    // Larger values result in larger memory overhead and better performance.
    static const size_t SAMPLES_PER_CHUNK = 16;
  };

#endif // SAMPLER_BASED_RENDERER_H