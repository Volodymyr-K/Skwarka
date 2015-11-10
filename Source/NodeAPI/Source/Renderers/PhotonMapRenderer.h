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

#ifndef PHOTON_MAP_RENDERER_H
#define PHOTON_MAP_RENDERER_H

#include <nan.h>
#include <Common/Common.h>
#include <Raytracer/Renderers/SamplerBasedRenderer.h>
#include <Raytracer/LTEIntegrators/PhotonLTEIntegrator.h>

/**
* Wraps the SamplerBasedRenderer with PhotonLTEIntegrator as a JS class.
* The class exports two methods to JS: render() and stop().
* The class supports asynchronous logging and displaying partial result as it renders the image.
*/
class PhotonMapRenderer : public Nan::ObjectWrap
  {
  public:
    static NAN_MODULE_INIT(Init);

    static v8::Local<v8::Object> Instantiate(v8::Local<v8::Object> i_params);

  private:
    class PhotonMapWorker;

  private:
    explicit PhotonMapRenderer(PhotonLTEIntegratorParams i_photon_map_params, size_t i_photons_millions, size_t i_samples_per_pixel, double i_refresh_period);

    static NAN_METHOD(New);

    static NAN_METHOD(Render);
    static NAN_METHOD(Stop);

    void _StopRendering();

  private:
    static Nan::Persistent<v8::Function> m_constructor;

    PhotonLTEIntegratorParams m_photon_map_params;
    size_t m_photons_millions, m_samples_per_pixel;
    double m_refresh_period;

    bool m_stopped;
    intrusive_ptr<PhotonLTEIntegrator> mp_integrator;
    intrusive_ptr<SamplerBasedRenderer> mp_renderer;
  };

#endif // PHOTON_MAP_RENDERER_H