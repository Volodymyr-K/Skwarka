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

#include <nan.h>
#include <string>

#include <Wrappers/SceneWrapper.h>
#include <Wrappers/CameraWrapper.h>
#include "SceneImportWorker.h"
#include "Renderers/PhotonMapRenderer.h"
#include "Log.h"

NAN_METHOD(CreateScene)
  {
  std::string filename(*(Nan::Utf8String(info[0])));
  Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
  Nan::AsyncQueueWorker(new SceneImportWorker(filename, callback));
  }

NAN_METHOD(CreateRenderer)
  {
  std::string renderer_name( *(Nan::Utf8String(info[0])) );

  v8::Local<v8::Object> handle;
  if (renderer_name == "Photon Mapping")
    handle = PhotonMapRenderer::Instantiate(info[1]->ToObject());

  info.GetReturnValue().Set(handle);
  }

NAN_METHOD(CreateCamera)
  {
  v8::Local<v8::Object> handle = CameraWrapper::Instantiate(info[0]->ToObject());
  info.GetReturnValue().Set(handle);
  }

NAN_METHOD(SetLog)
  {
  global_log_callback.Reset(info[0].As<v8::Function>());
  }

NAN_MODULE_INIT(InitAll)
  {
  SceneWrapper::Init(target);
  CameraWrapper::Init(target);
  PhotonMapRenderer::Init(target);

  Nan::Set(target, Nan::New("createScene").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(CreateScene)).ToLocalChecked());

  Nan::Set(target, Nan::New("createRenderer").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(CreateRenderer)).ToLocalChecked());

  Nan::Set(target, Nan::New("createCamera").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(CreateCamera)).ToLocalChecked());

  Nan::Set(target, Nan::New("setLog").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(SetLog)).ToLocalChecked());
  }

NODE_MODULE(NodeAPI, InitAll)