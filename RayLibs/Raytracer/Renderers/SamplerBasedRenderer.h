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
#include <Raytracer/Core/LTEIntegrator.h>

/**
* Renders image by shooting camera rays for each camera sample generated by Sampler.
* The renderer uses LTEIntegrator as a strategy for computing the radiance.
*/
class SamplerBasedRenderer: public Renderer
  {
  public:
    /**
    * Creates SamplerBasedRenderer instance with the specified LTEIntegrator and Sampler. The logger implementation must be thread-safe.
    */
    SamplerBasedRenderer(intrusive_ptr<LTEIntegrator> ip_lte_integrator, intrusive_ptr<Sampler> ip_sampler, intrusive_ptr<Log> ip_log = NULL);

    /**
    * Renders the scene for the specified camera.
    * The rendered image is saved to the camera's film. The film is cleared before rendering, so the previous image will be lost.
    * The method calls LTEIntegrator::RequestSamples to let the integrator request the samples sequences.
    * @param ip_camera Camera in the scene for which the image is to be rendered.
    * @param i_low_thread_priority Specifies OS scheduling priority for tbb threads that perform rendering. Use true to set low priority and false for default priority.
    */
    virtual void Render(intrusive_ptr<const Camera> ip_camera, bool i_low_thread_priority = false) const;

  private:
    // Not implemented, not a value type.
    SamplerBasedRenderer(const SamplerBasedRenderer&);
    SamplerBasedRenderer &operator=(const SamplerBasedRenderer&);

  private:
    // Private types, used for multi-threaded rendering.
    class PixelsChunk;
    class SamplesGeneratorFilter;
    class IntegratorFilter;
    class FilmWriterFilter;

  private:
    intrusive_ptr<Sampler> mp_sampler;

    intrusive_ptr<LTEIntegrator> mp_lte_integrator;

    intrusive_ptr<Log> mp_log;

    // Defines the maximum number of tokens the TBB pipeline can run concurrently.
    // This is also the upper bound on the number of threads the pipeline can utilize concurrently.
    static const size_t MAX_PIPELINE_TOKENS_NUM = 64;

    // Defines the number of pixels in each PixelsChunk.
    // Larger values decrease the number of tokens (and thus reduce the locking overhead) but may decrease the parallelism.
    static const size_t PIXELS_PER_CHUNK = 16;
  };

#endif // SAMPLER_BASED_RENDERER_H