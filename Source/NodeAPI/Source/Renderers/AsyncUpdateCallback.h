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

#ifndef ASYNC_UPDATE_CALLBACK_H
#define ASYNC_UPDATE_CALLBACK_H

#include <nan.h>
#include <vector>
#include <Common/Common.h>
#include <Raytracer/Core/Film.h>

#include "FilmConverters.h"

/**
* DisplayUpdateCallback implementation that asynchronously sends image from a worker thread to the V8 main thread.
*/
class AsyncUpdateCallback : public DisplayUpdateCallback
  {
  public:
    AsyncUpdateCallback(const Nan::AsyncProgressWorker::ExecutionProgress& i_progress) :
      m_progress(i_progress) {}

    void Update(intrusive_ptr<const Film> ip_film)
      {
      std::vector<unsigned char> png = NodeAPI::FilmConverters::FilmToPNG(ip_film);
      m_progress.Send(reinterpret_cast<const char*>(&png[0]), png.size());
      }

  private:
    const Nan::AsyncProgressWorker::ExecutionProgress& m_progress;
  };

#endif // ASYNC_UPDATE_CALLBACK_H