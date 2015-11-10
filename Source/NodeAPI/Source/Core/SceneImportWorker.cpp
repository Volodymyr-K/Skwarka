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

#include "SceneImportWorker.h"

#include <CADImport/SceneImporters/PbrtSceneImporter.h>

#include "Wrappers/SceneWrapper.h"
#include "Wrappers/CameraWrapper.h"
#include "Log.h"

SceneImportWorker::SceneImportWorker(std::string &i_filename, Nan::Callback *ip_callback) :
AsyncLoggingProgressWorker(ip_callback, intrusive_ptr<Log>(new CallbackLog())), m_filename(i_filename)
  {
  }

// Executed inside the worker-thread.
// It is not safe to access V8, or V8 data structures here, so everything we need for input and output should go on `this`.
void SceneImportWorker::Execute(const AsyncProgressWorker::ExecutionProgress &i_progress)
  {
  intrusive_ptr<Log> p_log(new AsyncLog(this));
  PbrtSceneImporter importer(m_filename, p_log);

  mp_scene = importer.GetScene();
  m_cameras = importer.GetCameras();
  }

// Executed when the async work is complete.
// This function will be run inside the main event loop so it is safe to use V8 again.
void SceneImportWorker::HandleOKCallback()
  {
  Nan::HandleScope scope;

  if (mp_scene && m_cameras.empty() == false)
    {
    auto scene_handle = SceneWrapper::Instantiate(mp_scene);

    v8::Local<v8::Array> cameras = Nan::New<v8::Array>((int)m_cameras.size());
    for (uint32_t i = 0; i < m_cameras.size(); ++i)
      Nan::Set(cameras, i, CameraWrapper::Instantiate(m_cameras[i]));

    v8::Local<v8::Object> ret_obj = Nan::New<v8::Object>();
    Nan::Set(ret_obj, Nan::New("scene").ToLocalChecked(), scene_handle);
    Nan::Set(ret_obj, Nan::New("cameras").ToLocalChecked(), cameras);

    v8::Local<v8::Value> argv[] = { Nan::Null(), ret_obj };
    callback->Call(2, argv);
    }
  else
    {
    v8::Local<v8::Value> argv[] = { Nan::New("Error when reading the scene file.").ToLocalChecked(), Nan::Null() };
    callback->Call(2, argv);
    }
  }

void SceneImportWorker::HandleProgressCallback(const char *ip_data, size_t i_size)
  {
  // Can't do anything here until we add some progress reporting to the scene importer.
  }