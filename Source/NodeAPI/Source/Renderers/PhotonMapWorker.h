#include <nan.h>
#include <vector>

#include <Common/Common.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/Samplers/ConsecutiveImagePixelsOrder.h>
#include <Raytracer/Samplers/LDSampler.h>

#include "../Render.h"
#include "FilmUpdateCallback.h"

class PhotonMapWorker : public Nan::AsyncProgressWorker
  {
public:
  PhotonMapWorker(intrusive_ptr<PhotonLTEIntegrator> ip_integrator, intrusive_ptr<const Camera> ip_camera, Nan::Callback *ip_callback, Nan::Callback *ip_progress) :
    AsyncProgressWorker(ip_callback), mp_integrator(ip_integrator), mp_camera(ip_camera), mp_progress(ip_progress) {}
  ~PhotonMapWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute(const AsyncProgressWorker::ExecutionProgress &i_progress)
    {
    intrusive_ptr<DisplayUpdateCallback> p_film_update_callback(new FilmUpdateCallback(i_progress));

    intrusive_ptr<ImagePixelsOrder> pixel_order(new ConsecutiveImagePixelsOrder);
    Point2D_i window_begin, window_end;
    mp_camera->GetFilm()->GetSamplingExtent(window_begin, window_end);
    intrusive_ptr<Sampler> p_sampler(new LDSampler(window_begin, window_end, 16, pixel_order));

    mp_integrator->ShootPhotons(10 * (size_t)1000000, true);

    intrusive_ptr<SamplerBasedRenderer> p_renderer(new SamplerBasedRenderer(p_lte_int, p_sampler));
    p_renderer->SetDisplayUpdateCallback(ip_callback, 2.0);

    p_renderer->Render(ip_camera, true);

    m_png = FilmToPNG(ip_camera->GetFilm());
    }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback()
    {
    Nan::HandleScope scope;

    Nan::MaybeLocal<v8::Object> buffer = Nan::CopyBuffer(reinterpret_cast<const char *>(&m_png[0]), (uint32_t)m_png.size());

    v8::Local<v8::Value> argv[] =
      {
      Nan::Null(),
      buffer.ToLocalChecked()
      };

    callback->Call(2, argv);
    }

  void HandleProgressCallback(const char *ip_data, size_t i_size)
    {
    if (mp_progress)
      {
      Nan::HandleScope scope;

      Nan::MaybeLocal<v8::Object> buffer = Nan::CopyBuffer(reinterpret_cast<const char *>(ip_data), (uint32_t)i_size);

      v8::Local<v8::Value> argv[] = { buffer.ToLocalChecked() };
      mp_progress->Call(1, argv);
      }
    }

private:
  intrusive_ptr<PhotonLTEIntegrator> mp_integrator;
  intrusive_ptr<const Camera> mp_camera;

  std::vector<unsigned char> m_png;
  Nan::Callback *mp_progress;
  };