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

#include "SamplerBasedRenderer.h"
#include <Common/Numerics.h>
#include <Math/Constants.h>
#include <Math/RandomGenerator.h>
#include <Raytracer/Core/CoreCommon.h>
#include <Raytracer/Core/CoreUtils.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <tbb/pipeline.h>
#include <vector>
#include <chrono>

/////////////////////////////////////////// Internal Types ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* This is a DTO class used to store sub-sampler and resulting radiance values returned by a renderer.
* The class is passed through the TBB pipeline by SamplesGeneratorFilter, IntegratorFilter and FilmWriterFilter.
* The class also keeps MemoryPool and RandomGenerator instances used by integrators and samplers respectively.
* Since the class is used by multiple threads it has a simple locking mechanism implemented by Acquire(), Release() and IsAvailable() methods.
* Although this locking strategy is not really thread-safe it works well for SamplesGeneratorFilter because this filter is serial and multiple
* threads will never race to acquire the lock over the same PixelsChunk.
* @sa SamplesGeneratorFilter, IntegratorFilter and FilmWriterFilter
*/
class SamplerBasedRenderer::PixelsChunk
  {
  public:
    /**
    * Creates PixelsChunk instance.
    * @param ip_sample Sample instance. The instance will be populated with sample values by a SubSampler and used by the LTEIntegrator. Should not be NULL.
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
* This is the input filter for the TBB pipeline (i.e. the first filter in the chain).
* It picks up an arbitrary available chunk, locks it to prevent other threads from using it, fills it with the data from the sampler and returns it.
* The filter is serial which means that two threads never execute it concurrently.
*/
class SamplerBasedRenderer::SamplesGeneratorFilter: public tbb::filter
  {
  public:
    SamplesGeneratorFilter(intrusive_ptr<Sampler> ip_sampler, size_t i_number_of_chunks, size_t i_pixels_per_chunk, const SamplerBasedRenderer *ip_renderer);

    ~SamplesGeneratorFilter();

    void* operator()(void*);

  private:
    intrusive_ptr<Sampler> mp_sampler;

    std::vector<PixelsChunk*> m_chunks;

    size_t m_next_chunk_index, m_pixels_per_chunk;

    const SamplerBasedRenderer *mp_renderer;
  };

/**
* This is the processing filter for the TBB pipeline.
* The filter gets the chunk with the sub-sampler set by SamplesGeneratorFilter filter and computes radiance value for all samples the sub-sampler produces.
* The filter is parallel which means that it can be executed by multiple threads concurrently.
*/
class SamplerBasedRenderer::IntegratorFilter: public tbb::filter
  {
  public:
    IntegratorFilter(intrusive_ptr<const LTEIntegrator> ip_lte_integrator, intrusive_ptr<const Camera> ip_camera, const SamplerBasedRenderer *ip_renderer,
      intrusive_ptr<Log> ip_log, bool i_low_thread_priority);

    void* operator()(void* ip_chunk);

  private:
    intrusive_ptr<const LTEIntegrator> mp_lte_integrator;

    intrusive_ptr<const Camera> mp_camera;

    const SamplerBasedRenderer *mp_renderer;

    intrusive_ptr<Log> mp_log;
    bool m_low_thread_priority;
  };

/**
* This is the output filter for the TBB pipeline.
* The filter gets the chunk with the image samples and radiance values generated by IntegratorFilter filter and adds them to the camera's film.
* The filter is serial which means that two threads never execute it concurrently.
*/
class SamplerBasedRenderer::FilmWriterFilter: public tbb::filter
  {
  public:
    FilmWriterFilter(intrusive_ptr<Film> ip_film, const SamplerBasedRenderer *ip_renderer);

    void* operator()(void* ip_chunk);
  private:

    intrusive_ptr<Film> mp_film;
    const SamplerBasedRenderer *mp_renderer;
  };

//////////////////////////////////////// SamplerBasedRenderer /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

SamplerBasedRenderer::SamplerBasedRenderer(intrusive_ptr<LTEIntegrator> ip_lte_integrator, intrusive_ptr<Sampler> ip_sampler, intrusive_ptr<Log> ip_log): Renderer(),
mp_lte_integrator(ip_lte_integrator), mp_sampler(ip_sampler), mp_log(ip_log), m_rendering_in_progress(false), m_rendering_stopped(false)
  {
  ASSERT(ip_lte_integrator);
  ASSERT(ip_sampler);
  }

void SamplerBasedRenderer::SetLog(intrusive_ptr<Log> ip_log)
  {
  mp_log = ip_log;
  }

bool SamplerBasedRenderer::Render(intrusive_ptr<const Camera> ip_camera, bool i_low_thread_priority)
  {
  ASSERT(ip_camera);
  auto start_time = std::chrono::system_clock::now();

  m_rendering_in_progress = true;
  m_rendering_stopped = false;

  ip_camera->GetFilm()->ClearFilm();

  mp_sampler->Reset();
  mp_sampler->ClearSamplesSequences();

  mp_lte_integrator->RequestSamples(mp_sampler);

  SamplesGeneratorFilter samples_generator(mp_sampler, MAX_PIPELINE_TOKENS_NUM, PIXELS_PER_CHUNK, this);
  IntegratorFilter integrator(mp_lte_integrator, ip_camera, this, mp_log, i_low_thread_priority);
  FilmWriterFilter film_writer(ip_camera->GetFilm(), this);

  tbb::pipeline pipeline;
  pipeline.add_filter(samples_generator);
  pipeline.add_filter(integrator);
  pipeline.add_filter(film_writer);

  pipeline.run(MAX_PIPELINE_TOKENS_NUM);
  pipeline.clear();
  m_rendering_in_progress = false;

  // Force display update (even if the time period has not passed yet).
  _UpdateDisplay(ip_camera->GetFilm(), true);

  if (mp_log && m_rendering_stopped == false)
    {
    auto end_time = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    mp_log->LogMessage(Log::INFO_LEVEL, "Rendering complete in " + std::to_string(duration) + " ms.");
    }

  return m_rendering_stopped==false;
  }

bool SamplerBasedRenderer::StopRendering()
  {
  if (m_rendering_in_progress == false)
    {
    if (mp_log)
      mp_log->LogMessage(Log::WARNING_LEVEL, "Rendering is not active. Nothing to stop.");

    return false;
    }

  if (m_rendering_stopped==true)
    {
    if (mp_log)
      mp_log->LogMessage(Log::WARNING_LEVEL, "Rendering has already been stopped.");

    return false;
    }

  if (mp_log)
    mp_log->LogMessage(Log::INFO_LEVEL, "Rendering has been stopped.");

  m_rendering_stopped = true;
  return true;
  }

bool SamplerBasedRenderer::InProgress() const
  {
  return m_rendering_in_progress;
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
    ip_film->AddSample(m_image_points[i], m_radiances[i]);
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

SamplerBasedRenderer::SamplesGeneratorFilter::SamplesGeneratorFilter(intrusive_ptr<Sampler> ip_sampler, size_t i_number_of_chunks, size_t i_pixels_per_chunk,
                                                                     const SamplerBasedRenderer *ip_renderer):
filter(serial_out_of_order), mp_sampler(ip_sampler), m_pixels_per_chunk(i_pixels_per_chunk), mp_renderer(ip_renderer), m_next_chunk_index(0)
  {
  ASSERT(ip_sampler);
  ASSERT(ip_renderer);
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
  // Stop rendering if it was stopped by user.
  if (mp_renderer->m_rendering_stopped)
    return NULL;

  /*
  Here we loop over all chunks until we find an available one, i.e. a one that is not locked by other thread.
  Although this is not really a thread-safe approach it works well here since SamplesGeneratorFilter is serial, so two
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
    {
    p_chunk->Release();
    return NULL;
    }
  }

////////////////////////////////////////// IntegratorFilter ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

SamplerBasedRenderer::IntegratorFilter::IntegratorFilter(intrusive_ptr<const LTEIntegrator> ip_lte_integrator, intrusive_ptr<const Camera> ip_camera, const SamplerBasedRenderer *ip_renderer,
                                                         intrusive_ptr<Log> ip_log, bool i_low_thread_priority)
: tbb::filter(parallel), mp_lte_integrator(ip_lte_integrator), mp_camera(ip_camera), mp_renderer(ip_renderer), mp_log(ip_log), m_low_thread_priority(i_low_thread_priority)
  {
  ASSERT(ip_lte_integrator);
  ASSERT(ip_camera);
  }

void* SamplerBasedRenderer::IntegratorFilter::operator()(void* ip_chunk)
  {
  int prev_thread_priority = 0;
  if (m_low_thread_priority)
    prev_thread_priority = CoreUtils::SetCurrentThreadPriority(THREAD_PRIORITY_LOWEST);

  PixelsChunk *p_chunk = static_cast<PixelsChunk*>(ip_chunk);
  MemoryPool *p_pool = p_chunk->GetMemoryPool();

  ThreadSpecifics ts;
  ts.mp_pool = p_pool;
  ts.mp_random_generator = p_chunk->GetRandomGenerator();

  const Sample *p_sample = NULL;
  while((p_sample = p_chunk->GetNextSample()) && mp_renderer->m_rendering_stopped==false)
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

    if (weight_dx > DBL_EPS && weight_dy > DBL_EPS)
      ray.m_has_differentials = true;

    ray.m_origin_dx=r_dx.m_origin;
    ray.m_origin_dy=r_dy.m_origin;
    ray.m_direction_dx=r_dx.m_direction;
    ray.m_direction_dy=r_dy.m_direction;

    Spectrum_d radiance = weight > DBL_EPS ? mp_lte_integrator->Radiance(ray, p_sample, ts) : Spectrum_d(0.0);

    // Log unexpected radiance values.
    if (IsNaN(radiance))
      {
      if (mp_log)
        mp_log->LogMessage(Log::WARNING_LEVEL, "Not-a-number radiance value returned for image sample. Setting to black.");
      radiance = Spectrum_d(0.0);
      }
    else
      if (SpectrumRoutines::Luminance(radiance) < -DBL_EPS)
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

  // Set the thread priority back to its original value.
  if (m_low_thread_priority)
    CoreUtils::SetCurrentThreadPriority(prev_thread_priority);

  return p_chunk;
  }

////////////////////////////////////////// FilmWriterFilter ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

SamplerBasedRenderer::FilmWriterFilter::FilmWriterFilter(intrusive_ptr<Film> ip_film, const SamplerBasedRenderer *ip_renderer): tbb::filter(serial_out_of_order),
mp_film(ip_film), mp_renderer(ip_renderer)
  {
  ASSERT(ip_film);
  ASSERT(ip_renderer);
  }

void* SamplerBasedRenderer::FilmWriterFilter::operator()(void* ip_chunk)
  {
  PixelsChunk *p_chunk = static_cast<PixelsChunk*>(ip_chunk);
  p_chunk->SaveToFilm(mp_film);
  p_chunk->Release();

  // Update display only if the time period has passed.
  mp_renderer->_UpdateDisplay(mp_film, false);

  return NULL;
  }