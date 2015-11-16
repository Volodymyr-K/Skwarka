/*
* Copyright (C) 2015 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
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

#include "PhotonMapRenderer.h"

#include <Raytracer/Samplers/ConsecutiveImagePixelsOrder.h>
#include <Raytracer/Samplers/LDSampler.h>

#include <Wrappers/SceneWrapper.h>
#include <Wrappers/CameraWrapper.h>
#include <Core/AsyncLoggingProgressWorker.h>
#include <Core/Log.h>
#include <Core/Util.h>

#include "AsyncUpdateCallback.h"
#include "FilmConverters.h"

/////////////// PhotonMapWorker //////////////////

class PhotonMapRenderer::PhotonMapWorker : public AsyncLoggingProgressWorker
  {
  using RunFunction = std::function<std::vector<unsigned char>(const Nan::AsyncProgressWorker::ExecutionProgress &i_progress, intrusive_ptr<Log> ip_log)>;

  public:
    PhotonMapWorker(RunFunction i_run, Nan::Callback *ip_complete_callback, Nan::Callback *ip_image_update_callback) :
      AsyncLoggingProgressWorker(ip_complete_callback, intrusive_ptr<Log>(new CallbackLog())), m_run(i_run), mp_image_update_callback(ip_image_update_callback) {}
    ~PhotonMapWorker()
      {
      delete mp_image_update_callback;
      }

    // Executed inside the worker-thread.
    // It is not safe to access V8, or V8 data structures here, so everything we need for input and output should go on `this`.
    void Execute(const AsyncProgressWorker::ExecutionProgress &i_progress)
      {
      intrusive_ptr<Log> p_log(new AsyncLog(this));
      m_png = m_run(i_progress, p_log);
      }

    // Executed when the async work is complete.
    // This function will be run inside the main event loop so it is safe to use V8 again.
    void HandleOKCallback()
      {
      Nan::HandleScope scope;

      if (m_png.empty())
        {
        v8::Local<v8::Value> argv[] = { Nan::Null(), Nan::Null() };
        callback->Call(2, argv);
        }
      else
        {
        Nan::MaybeLocal<v8::Object> buffer = Nan::CopyBuffer(reinterpret_cast<const char *>(&m_png[0]), (uint32_t)m_png.size());

        v8::Local<v8::Value> argv[] = { Nan::Null(), buffer.ToLocalChecked() };
        callback->Call(2, argv);
        }
      }

    void HandleProgressCallback(const char *ip_data, size_t i_size)
      {
      Nan::HandleScope scope;

      Nan::MaybeLocal<v8::Object> buffer = Nan::CopyBuffer(ip_data, (uint32_t)i_size);

      v8::Local<v8::Value> argv[] = { buffer.ToLocalChecked() };
      mp_image_update_callback->Call(1, argv);
      }

  private:
    RunFunction m_run;

    std::vector<unsigned char> m_png;
    Nan::Callback *mp_image_update_callback;
  };

/////////////// PhotonMapRenderer ////////////////

Nan::Persistent<v8::Function> PhotonMapRenderer::m_constructor;

PhotonMapRenderer::PhotonMapRenderer(PhotonLTEIntegratorParams i_photon_map_params, size_t i_photons_millions, size_t i_samples_per_pixel, double i_refresh_period) :
Nan::ObjectWrap(), m_photon_map_params(i_photon_map_params), m_photons_millions(i_photons_millions), m_samples_per_pixel(i_samples_per_pixel),
m_refresh_period(i_refresh_period), m_stopped(false)
  {
  }

NAN_MODULE_INIT(PhotonMapRenderer::Init)
  {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(PhotonMapRenderer::New);
  tpl->SetClassName(Nan::New("PhotonMapRenderer").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "render", Render);
  Nan::SetPrototypeMethod(tpl, "stop", Stop);

  m_constructor.Reset(tpl->GetFunction());
  }

v8::Local<v8::Object> PhotonMapRenderer::Instantiate(v8::Local<v8::Object> i_params)
  {
  PhotonLTEIntegratorParams photon_map_params;

  if (NodeAPI::Utils::HasProperty(i_params, "photonMapParams"))
    {
    auto paramsObject = NodeAPI::Utils::GetObjectProperty(i_params, "photonMapParams");

    if (NodeAPI::Utils::HasProperty(paramsObject, "directLightSamples"))
      photon_map_params.m_direct_light_samples_num = NodeAPI::Utils::GetUIntProperty(paramsObject, "directLightSamples");
    if (NodeAPI::Utils::HasProperty(paramsObject, "gatherSamplesNum"))
      photon_map_params.m_gather_samples_num = NodeAPI::Utils::GetUIntProperty(paramsObject, "gatherSamplesNum");
    if (NodeAPI::Utils::HasProperty(paramsObject, "causticLookupPhotonsNum"))
      photon_map_params.m_caustic_lookup_photons_num = NodeAPI::Utils::GetUIntProperty(paramsObject, "causticLookupPhotonsNum");
    if (NodeAPI::Utils::HasProperty(paramsObject, "maxCausticLookupDist"))
      photon_map_params.m_max_caustic_lookup_dist = NodeAPI::Utils::GetDoubleProperty(paramsObject, "maxCausticLookupDist");
    if (NodeAPI::Utils::HasProperty(paramsObject, "maxSpecularDepth"))
      photon_map_params.m_max_specular_depth = NodeAPI::Utils::GetUIntProperty(paramsObject, "maxSpecularDepth");
    if (NodeAPI::Utils::HasProperty(paramsObject, "mediaStepSize"))
      photon_map_params.m_media_step_size = NodeAPI::Utils::GetDoubleProperty(paramsObject, "mediaStepSize");
    }
  else
    Nan::ThrowError("Required photonMapParams property is missing");

  size_t photon_millions = NodeAPI::Utils::HasProperty(i_params, "photonsMillions") ? NodeAPI::Utils::GetUIntProperty(i_params, "photonsMillions") : 1;
  size_t samples_per_pixel = NodeAPI::Utils::HasProperty(i_params, "samplesPerPixel") ? NodeAPI::Utils::GetUIntProperty(i_params, "samplesPerPixel") : 1;
  double refresh_period = NodeAPI::Utils::HasProperty(i_params, "refreshPeriod") ? NodeAPI::Utils::GetDoubleProperty(i_params, "refreshPeriod") : 1;

  PhotonMapRenderer *p_obj = new PhotonMapRenderer(photon_map_params, photon_millions, samples_per_pixel, refresh_period);
  v8::Local<v8::Value> arg[1] = { Nan::New<v8::External>(p_obj) };
  v8::Local<v8::Object> handle = Nan::New(m_constructor)->NewInstance(1, arg);
  return handle;
  }

NAN_METHOD(PhotonMapRenderer::New)
  {
  if (info.IsConstructCall() && info[0]->IsExternal())
    {
    PhotonMapRenderer *p_obj = static_cast<PhotonMapRenderer*>(v8::External::Cast(*(info[0]))->Value());
    p_obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    }
  else
    Nan::ThrowError("Direct instantiation is not supported");
  }

NAN_METHOD(PhotonMapRenderer::Render)
  {
  PhotonMapRenderer *p_this = Nan::ObjectWrap::Unwrap<PhotonMapRenderer>(info.This());
  p_this->_StopRendering();

  intrusive_ptr<const Scene> p_scene = Nan::ObjectWrap::Unwrap<SceneWrapper>(info[0]->ToObject())->GetScene();
  intrusive_ptr<const Camera> p_camera = Nan::ObjectWrap::Unwrap<CameraWrapper>(info[1]->ToObject())->GetCamera();
  Nan::Callback *p_complete_callback = new Nan::Callback(info[2].As<v8::Function>());
  Nan::Callback *p_image_update_callback = new Nan::Callback(info[3].As<v8::Function>());

  auto run = [p_this, p_scene, p_camera](const Nan::AsyncProgressWorker::ExecutionProgress &i_progress, intrusive_ptr<Log> ip_log)
    {
    p_this->m_stopped = false;
    p_this->mp_integrator.reset();
    p_this->mp_renderer.reset();

    intrusive_ptr<ImagePixelsOrder> pixel_order(new ConsecutiveImagePixelsOrder);

    Point2D_i window_begin, window_end;
    p_camera->GetFilm()->GetSamplingExtent(window_begin, window_end);
    intrusive_ptr<Sampler> p_sampler(new LDSampler(window_begin, window_end, p_this->m_samples_per_pixel, pixel_order));

    p_this->mp_integrator.reset(new PhotonLTEIntegrator(p_scene, p_this->m_photon_map_params, ip_log));
    p_this->mp_integrator->ShootPhotons(p_this->m_photons_millions * (size_t)1000000, true);

    if (!p_this->m_stopped)
      {
      p_this->mp_renderer.reset(new SamplerBasedRenderer(p_this->mp_integrator, p_sampler, ip_log));
      intrusive_ptr<DisplayUpdateCallback> p_display_update_callback(new AsyncUpdateCallback(i_progress));
      p_this->mp_renderer->SetDisplayUpdateCallback(p_display_update_callback, p_this->m_refresh_period);
      p_this->mp_renderer->Render(p_camera, true);

      // Now that the async part has completed, set the logger to the one directly calling the V8 callback.
      intrusive_ptr<Log> p_callback_log(new CallbackLog());
      p_this->mp_renderer->SetLog(p_callback_log);
      }
    
    return p_this->mp_integrator ? NodeAPI::FilmConverters::FilmToPNG(p_camera->GetFilm()) : std::vector<unsigned char>();
    };

  Nan::AsyncQueueWorker(new PhotonMapWorker(run, p_complete_callback, p_image_update_callback));
  }

NAN_METHOD(PhotonMapRenderer::Stop)
  {
  PhotonMapRenderer *p_this = Nan::ObjectWrap::Unwrap<PhotonMapRenderer>(info.This());
  p_this->_StopRendering();
  }

void PhotonMapRenderer::_StopRendering()
  {
  m_stopped = true;

  if (mp_integrator && mp_integrator->InProgress())
    mp_integrator->StopShooting();

  if (mp_renderer && mp_renderer->InProgress())
    mp_renderer->StopRendering();
  }