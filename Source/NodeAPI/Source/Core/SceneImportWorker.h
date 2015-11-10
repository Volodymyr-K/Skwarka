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

#ifndef SCENE_IMPORT_WORKER_H
#define SCENE_IMPORT_WORKER_H

#include <nan.h>
#include <vector>
#include <string>

#include <Common/Common.h>
#include <Raytracer/Core/Scene.h>
#include <Raytracer/Core/Camera.h>

#include "AsyncLoggingProgressWorker.h"

/**
* Worker class that asynchronously imports a Scene from a file.
*/
class SceneImportWorker : public AsyncLoggingProgressWorker
  {
  public:
    SceneImportWorker(std::string &i_filename, Nan::Callback *ip_callback);

    void Execute(const AsyncProgressWorker::ExecutionProgress &i_progress);

    void HandleOKCallback();

    void HandleProgressCallback(const char *ip_data, size_t i_size);

  private:
    std::string m_filename;

    intrusive_ptr<const Scene> mp_scene;
    std::vector<intrusive_ptr<const Camera>> m_cameras;
  };

#endif // SCENE_IMPORT_WORKER_H