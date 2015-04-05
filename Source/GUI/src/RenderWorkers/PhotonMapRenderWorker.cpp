#include "PhotonMapRenderWorker.h"

#include <Raytracer/Samplers/ConsecutiveImagePixelsOrder.h>
#include <Raytracer/Samplers/LDSampler.h>
#include <Raytracer/Renderers/SamplerBasedRenderer.h>
#include <Raytracer/LTEIntegrators/PhotonLTEIntegrator.h>

#include "../RenderUpdateCallback.h"

PhotonMapRenderWorker::PhotonMapRenderWorker(RenderCanvas *ip_canvas, intrusive_ptr<const Scene> ip_scene, intrusive_ptr<const Camera> ip_camera,
                                             intrusive_ptr<Log> ip_log, PhotonMapParams *ip_params):
  RenderWorker(ip_canvas, ip_scene, ip_camera, ip_log), mp_params(ip_params)
  {
  ASSERT(ip_params);
  }

void PhotonMapRenderWorker::process()
  {
  intrusive_ptr<ImagePixelsOrder> pixel_order(new ConsecutiveImagePixelsOrder);

  Point2D_i window_begin, window_end;
  getCamera()->GetFilm()->GetSamplingExtent(window_begin, window_end);
  getCamera()->GetFilm()->ClearFilm();

  intrusive_ptr<Sampler> p_sampler( new LDSampler(window_begin, window_end, mp_params->getSamplesPerPixel(), pixel_order) );

  PhotonLTEIntegratorParams params;
  params.m_direct_light_samples_num=mp_params->getDirectLightSamples();
  params.m_gather_samples_num=mp_params->getFinalGatherSamples();
  params.m_caustic_lookup_photons_num=mp_params->getCausticLookupPhotonsNum();
  params.m_max_caustic_lookup_dist=mp_params->getCausticLookupDist();
  params.m_max_specular_depth=mp_params->getSpecularDepth();
  params.m_media_step_size=mp_params->getMediaStepSize();
  params.m_max_caustic_photons=mp_params->getMaxCausticPhotons() * (size_t)1000000;
  params.m_max_direct_photons=mp_params->getMaxDirectPhotons() * (size_t)1000000;
  params.m_max_indirect_photons=mp_params->getMaxIndirectPhotons() * (size_t)1000000;
  intrusive_ptr<PhotonLTEIntegrator> p_lte_int( new PhotonLTEIntegrator(getScene(), params) );

  getLog()->LogMessage(Log::INFO_LEVEL, "Shooting photons...");
  p_lte_int->ShootPhotons(mp_params->getPhotonPaths() * (size_t)1000000, true);
  getLog()->LogMessage(Log::INFO_LEVEL, "Shooting photons complete.");

  mp_renderer.reset( new SamplerBasedRenderer(p_lte_int, p_sampler) );

  intrusive_ptr<DisplayUpdateCallback> p_callback( new RenderUpdateCallback(getCanvas()) );
  mp_renderer->SetDisplayUpdateCallback(p_callback, 5.0);

  getLog()->LogMessage(Log::INFO_LEVEL, "Rendering...");
  bool complete = mp_renderer->Render(getCamera(), true);
  if (complete)
    getLog()->LogMessage(Log::INFO_LEVEL, "Rendering complete.");
  else
    getLog()->LogMessage(Log::INFO_LEVEL, "Rendering was not completed.");

  mp_renderer.reset(NULL);
  emit finished();
  }

void PhotonMapRenderWorker::stop()
  {
  if(mp_renderer != NULL)
    {
    mp_renderer->StopRendering();
    getLog()->LogMessage(Log::INFO_LEVEL, "Rendering stopped by user.");

    emit finished();
    }
  }
