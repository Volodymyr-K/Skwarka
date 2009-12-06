#include "SamplerBasedRenderer.h"
#include <Common/Numerics.h>
#include <Math/Constants.h>
#include <Math/RandomGenerator.h>
#include <tbb/pipeline.h>
#include <vector>

/////////////////////////////////////////// Internal Types ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* This is a DTO class used to store sub-sampler and resulting radiance values returned by a renderer.
* The class is passed through the TBB pipeline by SamplesGeneratorFilter, IntegratorFilter and FilmWriterFilter.
* The class also keeps MemoryPool and RandomGenerator instances used by integrators and samplers respectively.
* Since the class is used by multiple threads it has a simple locking mechanism implemented by Acquire(), Release() and IsAvailable() methods.
* Although this locking strategy is not really thread-safe it works well for SamplesGeneratorFilter because this filter is serial and multiple
* threads will never race to acquire the lock over the chunk.
* @sa SamplesGeneratorFilter, IntegratorFilter and FilmWriterFilter
*/
class SamplerBasedRenderer::PixelsChunk
  {
  public:
    /**
    * Creates PixelsChunk instance.
    * @param ip_sample Sample instance. The instance will be populated with sample values by a SubSampler and used by the integrators. Should not be NULL.
    * @param i_rng_seed Seed number for the random generator.
    */
    PixelsChunk(intrusive_ptr<Sample> ip_sample, size_t i_rng_seed);

    ~PixelsChunk();

    /**
    * Locks the chunk in a multi-threaded environment. The other threads will check if the chunk is available before locking it.
    */
    void Acquire();

    /**
    * Releases the chunk so the other threads can acquire it.
    */
    void Release();

    /**
    * Returns true if the chunk is not locked.
    */
    bool IsAvailable() const;

    /**
    * Sets sub-sampler for this pixels chunk. Should be called before GetNextSample() method is called.
    */
    void SetSubSampler(intrusive_ptr<SubSampler> ip_sub_sampler);   

    /**
    * Returns (raw) pointer to the next sample populated by the SubSampler or NULL if there are no more samples.
    * The method always returns pointer to the same Sample instance but calling this method makes SubSampler fill the Sample with the data.
    */
    const Sample *GetNextSample();

    /**
    * Sets resulting radiance for the specified camera sample.
    * @param i_image_point Image point.
    * @param i_radiance Radiance value.
    */
    void AddImageSample(const Point2D_d &i_image_point, const Spectrum_d &i_radiance);

    /**
    * Clears all image samples added so far.
    */
    void ClearImageSamples();

    /**
    * Saves all image samples added so far to the specified film.
    */
    void SaveToFilm(intrusive_ptr<Film> ip_film) const;

    /**
    * Returns memory pool.
    */
    MemoryPool *GetMemoryPool() const;

    /**
    * Returns random generator.
    */
    RandomGenerator<double> *GetRandomGenerator() const;

  private:
    // Not implemented, should only be passed by a reference.
    PixelsChunk(const PixelsChunk&);
    PixelsChunk &operator=(const PixelsChunk&);

  private:
    intrusive_ptr<Sample> mp_sample;
    intrusive_ptr<SubSampler> mp_sub_sampler;
    bool m_available;

    std::vector<Point2D_d > m_image_points;
    std::vector<Spectrum_d> m_radiances;

    MemoryPool *mp_memory_pool;
    RandomGenerator<double> *mp_rng;
  };

/**
* This is the input filter for the TBB pipeline.
* It picks up an arbitrary available chunk, locks it to prevent other threads using it, fills it with the data from the sampler and returns.
* The filter is serial which means that two threads never execute it concurrently.
*/
class SamplerBasedRenderer::SamplesGeneratorFilter: public tbb::filter
  {
  public:
    SamplesGeneratorFilter(intrusive_ptr<Sampler> ip_sampler, size_t i_number_of_chunks, size_t i_pixels_per_chunk);

    ~SamplesGeneratorFilter();

    void* operator()(void*);

  private:
    intrusive_ptr<Sampler> mp_sampler;

    std::vector<PixelsChunk*> m_chunks;

    size_t m_next_chunk_index, m_pixels_per_chunk;
  };

/**
* This is the processing filter for the TBB pipeline.
* The filter gets the chunk with the sub-sampler set by SamplesGeneratorFilter filter and computes radiance value for all samples the sub-sampler produces.
* The filter is parallel which means that it can be executed by multiple threads concurrently.
*/
class SamplerBasedRenderer::IntegratorFilter: public tbb::filter
  {
  public:
    IntegratorFilter(const SamplerBasedRenderer *ip_renderer, intrusive_ptr<const Camera> ip_camera, intrusive_ptr<Log> ip_log);

    void* operator()(void* ip_chunk);

  private:
    const SamplerBasedRenderer *mp_renderer;

    intrusive_ptr<const Camera> mp_camera;
    intrusive_ptr<Log> mp_log;
  };

/**
* This is the output filter for the TBB pipeline.
* The filter gets the chunk with the image samples and radiance values generated by IntegratorFilter filter and adds them to the camera's film.
* The filter is serial which means that two threads never execute it concurrently.
*/
class SamplerBasedRenderer::FilmWriterFilter: public tbb::filter
  {
  public:
    FilmWriterFilter(intrusive_ptr<Film> ip_film);

    void* operator()(void* ip_chunk);
  private:

    intrusive_ptr<Film> mp_film;
  };

//////////////////////////////////////// SamplerBasedRenderer /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

SamplerBasedRenderer::SamplerBasedRenderer(intrusive_ptr<const Scene> ip_scene, intrusive_ptr<Sampler> ip_sampler, intrusive_ptr<Log> ip_log): Renderer(ip_scene),
mp_scene(ip_scene), mp_sampler(ip_sampler), mp_log(ip_log), mp_surface_integrator(NULL), mp_volume_integrator(NULL)
  {
  ASSERT(ip_scene);
  ASSERT(ip_sampler);
  }

void SamplerBasedRenderer::SetSurfaceIntegrator(intrusive_ptr<SurfaceIntegrator> ip_surface_integrator)
  {
  mp_surface_integrator=ip_surface_integrator;
  }

void SamplerBasedRenderer::SetVolumeIntegrator(intrusive_ptr<VolumeIntegrator> ip_volume_integrator)
  {
  mp_volume_integrator=ip_volume_integrator;
  }

Spectrum_d SamplerBasedRenderer::Radiance(const RayDifferential &i_ray, const Sample *ip_sample, MemoryPool &i_pool) const
  {
  ASSERT(mp_surface_integrator);
  ASSERT(i_ray.m_base_ray.m_direction.IsNormalized());

  Intersection isect;
  double intersection_t;
  bool hit=mp_scene->Intersect(i_ray, isect, &intersection_t);

  RayDifferential intersection_ray(i_ray);
  Spectrum_d radiance;
  if (hit)
    {
    radiance = mp_surface_integrator->Radiance(i_ray, isect, ip_sample, i_pool);
    intersection_ray.m_base_ray.m_max_t=intersection_t;
    }
  else
    if (IsInf(i_ray.m_base_ray.m_max_t)) // Check if the ray is unbounded.
      {
      // Add contribution of infinite light sources for an unbounded ray that does not intersect any primitive.
      const LightSources &lights = mp_scene->GetLightSources();
      for (size_t i = 0; i < lights.m_infinitiy_light_sources.size(); ++i)
        radiance += lights.m_infinitiy_light_sources[i]->Radiance(i_ray);
      }

  if (mp_volume_integrator)
    {
    Spectrum_d volume_radiance(0.0);
    Spectrum_d transmittance(1.0);

    volume_radiance = mp_volume_integrator->Radiance(intersection_ray, ip_sample, transmittance);
    ASSERT(InRange(transmittance, 0.0, 1.0));

    return radiance * transmittance + volume_radiance;
    }
  else
    return radiance;
  }

Spectrum_d SamplerBasedRenderer::Transmittance(const Ray &i_ray, const Sample *ip_sample) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());

  if (mp_volume_integrator)
    {
    Spectrum_d transmittance = mp_volume_integrator->Transmittance(i_ray, ip_sample);
    ASSERT(InRange(transmittance, 0.0, 1.0));
    return transmittance;
    }
  else
    return Spectrum_d(1.0);
  }

void SamplerBasedRenderer::Render(intrusive_ptr<const Camera> ip_camera) const
  {
  ASSERT(ip_camera);
  ASSERT(mp_surface_integrator);

  ip_camera->GetFilm()->ClearFilm();

  mp_sampler->Reset();
  mp_sampler->ClearSamplesSequences();

  mp_surface_integrator->RequestSamples(mp_sampler);
  if (mp_volume_integrator)
    mp_volume_integrator->RequestSamples(mp_sampler);

  SamplesGeneratorFilter samples_generator(mp_sampler, MAX_PIPELINE_TOKENS_NUM, PIXELS_PER_CHUNK);
  IntegratorFilter integrator(this, ip_camera, mp_log);
  FilmWriterFilter film_writer(ip_camera->GetFilm());

  tbb::pipeline pipeline;
  pipeline.add_filter(samples_generator);
  pipeline.add_filter(integrator);
  pipeline.add_filter(film_writer);

  pipeline.run(MAX_PIPELINE_TOKENS_NUM);

  pipeline.clear();
  }

//////////////////////////////////////////// PixelsChunk /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

SamplerBasedRenderer::PixelsChunk::PixelsChunk(intrusive_ptr<Sample> ip_sample, size_t i_rng_seed): mp_sample(ip_sample), mp_sub_sampler(NULL), m_available(true)
  {
  ASSERT(ip_sample);

  mp_rng = new RandomGenerator<double>(i_rng_seed);
  mp_memory_pool = new MemoryPool();
  }

SamplerBasedRenderer::PixelsChunk::~PixelsChunk()
  {
  delete mp_memory_pool;
  delete mp_rng;
  }

void SamplerBasedRenderer::PixelsChunk::Acquire()
  {
  ASSERT(m_available);
  m_available = false;
  }

void SamplerBasedRenderer::PixelsChunk::Release()
  {
  ASSERT(m_available==false);
  m_available = true;
  }

bool SamplerBasedRenderer::PixelsChunk::IsAvailable() const
  {
  return m_available;
  }

void SamplerBasedRenderer::PixelsChunk::SetSubSampler(intrusive_ptr<SubSampler> ip_sub_sampler)
  {
  ASSERT(ip_sub_sampler);
  mp_sub_sampler=ip_sub_sampler;
  }

const Sample *SamplerBasedRenderer::PixelsChunk::GetNextSample()
  {
  ASSERT(mp_sample);
  ASSERT(mp_sub_sampler);

  if (mp_sub_sampler->GetNextSample(mp_sample))
    return mp_sample.get();
  else
    return NULL;
  }

void SamplerBasedRenderer::PixelsChunk::AddImageSample(const Point2D_d &i_image_point, const Spectrum_d &i_radiance)
  {
  m_image_points.push_back(i_image_point);
  m_radiances.push_back(i_radiance);
  }

void SamplerBasedRenderer::PixelsChunk::ClearImageSamples()
  {
  m_image_points.clear();
  m_radiances.clear();
  }

void SamplerBasedRenderer::PixelsChunk::SaveToFilm(intrusive_ptr<Film> ip_film) const
  {
  ASSERT(ip_film);
  ASSERT(m_image_points.size() == m_radiances.size());

  for(size_t i=0;i<m_image_points.size();++i)
    ip_film->AddSample(m_image_points[i], Convert<float>(m_radiances[i]));
  }

MemoryPool *SamplerBasedRenderer::PixelsChunk::GetMemoryPool() const
  {
  return mp_memory_pool;
  }

RandomGenerator<double> *SamplerBasedRenderer::PixelsChunk::GetRandomGenerator() const
  {
  return mp_rng;
  }

/////////////////////////////////////// SamplesGeneratorFilter ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

SamplerBasedRenderer::SamplesGeneratorFilter::SamplesGeneratorFilter(intrusive_ptr<Sampler> ip_sampler, size_t i_number_of_chunks, size_t i_pixels_per_chunk):
filter(serial_out_of_order), mp_sampler(ip_sampler), m_pixels_per_chunk(i_pixels_per_chunk), m_next_chunk_index(0)
  {
  ASSERT(ip_sampler);
  ASSERT(i_number_of_chunks>0);
  ASSERT(i_pixels_per_chunk>0);

  for(size_t i=0;i<i_number_of_chunks;++i)
    m_chunks.push_back(new PixelsChunk( ip_sampler->CreateSample(), i ));
  }

SamplerBasedRenderer::SamplesGeneratorFilter::~SamplesGeneratorFilter()
  {
  for(size_t i=0;i<m_chunks.size();++i)
    delete m_chunks[i];
  }

void* SamplerBasedRenderer::SamplesGeneratorFilter::operator()(void*)
  {
  /*
  Here we loop over all chunks until we find an available one, i.e. a one that is not locked by other thread.
  Although this is not really a thread-safe strategy it works well here since SamplesGeneratorFilter is serial, so two
  threads will never try to get a lock concurrently.
  */
  while(m_chunks[m_next_chunk_index]->IsAvailable()==false)
    m_next_chunk_index = (m_next_chunk_index+1) % m_chunks.size();

  PixelsChunk *p_chunk = m_chunks[m_next_chunk_index];
  p_chunk->Acquire();
  p_chunk->ClearImageSamples();

  m_next_chunk_index = (m_next_chunk_index+1) % m_chunks.size();

  intrusive_ptr<SubSampler> p_sub_sampler = mp_sampler->GetNextSubSampler(m_pixels_per_chunk, p_chunk->GetRandomGenerator());
  if (p_sub_sampler)
    {
    p_chunk->SetSubSampler(p_sub_sampler);
    return p_chunk;
    }
  else
    return NULL;
  }

////////////////////////////////////////// IntegratorFilter ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

SamplerBasedRenderer::IntegratorFilter::IntegratorFilter(const SamplerBasedRenderer *ip_renderer, intrusive_ptr<const Camera> ip_camera, intrusive_ptr<Log> ip_log): tbb::filter(parallel),
mp_renderer(ip_renderer), mp_camera(ip_camera), mp_log(ip_log)
  {
  ASSERT(ip_renderer);
  ASSERT(ip_camera);
  }

void* SamplerBasedRenderer::IntegratorFilter::operator()(void* ip_chunk)
  {
  PixelsChunk *p_chunk = static_cast<PixelsChunk*>(ip_chunk);
  MemoryPool *p_pool = p_chunk->GetMemoryPool();

  while(const Sample *p_sample = p_chunk->GetNextSample())
    {
    Point2D_d image_point = p_sample->GetImagePoint();
    Point2D_d lens_uv = p_sample->GetLensUV();

    double x_filter_width, y_filter_width;
    p_sample->GetImageFilterWidth(x_filter_width, y_filter_width);

    // Compute ray differentials by computing camera rays for adjacent image pixels.
    RayDifferential ray;
    Ray r_dx, r_dy;
    double weight = mp_camera->GenerateRay(image_point, lens_uv, ray.m_base_ray);
    double weight_dx = mp_camera->GenerateRay(image_point+Point2D_d(x_filter_width, 0.0), lens_uv, r_dx);
    double weight_dy = mp_camera->GenerateRay(image_point+Point2D_d(0.0, y_filter_width), lens_uv, r_dy);

    if (weight_dx != 0.0 && weight_dy != 0.0)
      ray.m_has_differentials = true;

    ray.m_origin_dx=r_dx.m_origin;
    ray.m_origin_dy=r_dy.m_origin;
    ray.m_direction_dx=r_dx.m_direction;
    ray.m_direction_dy=r_dy.m_direction;

    Spectrum_d radiance=Spectrum_d(0.0);
    if (weight != 0.0)
      radiance = mp_renderer->Radiance(ray, p_sample, *p_pool);

    // Log unexpected radiance values.
    if (IsNaN(radiance))
      {
      if (mp_log)
        mp_log->LogMessage(Log::WARNING_LEVEL, "Not-a-number radiance value returned for image sample. Setting to black.");
      radiance = Spectrum_d(0.0);
      }
    else
      if (radiance.Luminance() < -DBL_EPS)
        {
        if (mp_log)
          mp_log->LogMessage(Log::WARNING_LEVEL, "Negative luminance value returned for image sample. Setting to black.");
        radiance = Spectrum_d(0.0);
        }
      else
        if (IsInf(radiance))
          {
          if (mp_log)
            mp_log->LogMessage(Log::WARNING_LEVEL, "Infinite spectrum value returned for image sample. Setting to black.");
          radiance = Spectrum_d(0.0);
          }

    p_chunk->AddImageSample(p_sample->GetImagePoint(), radiance * weight);

    // Free all allocated objects since we don't need them anymore at this point.
    p_pool->FreeAll();
    }

  return p_chunk;
  }

////////////////////////////////////////// FilmWriterFilter ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

SamplerBasedRenderer::FilmWriterFilter::FilmWriterFilter(intrusive_ptr<Film> ip_film): tbb::filter(serial_out_of_order), mp_film(ip_film)
  {
  ASSERT(ip_film);
  }

void* SamplerBasedRenderer::FilmWriterFilter::operator()(void* ip_chunk)
  {
  PixelsChunk *p_chunk = static_cast<PixelsChunk*>(ip_chunk);
  p_chunk->SaveToFilm(mp_film);
  p_chunk->Release();

  return NULL;
  }