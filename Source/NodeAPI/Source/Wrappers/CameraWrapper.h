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

#ifndef CAMERA_WRAPPER_H
#define CAMERA_WRAPPER_H

#include <nan.h>
#include <Common/Common.h>
#include <Raytracer/Core/Camera.h>

/**
* Wraps Camera as a JS object.
*/
class CameraWrapper : public Nan::ObjectWrap
  {
  public:
    static NAN_MODULE_INIT(Init);

    static v8::Local<v8::Object> Instantiate(intrusive_ptr<const Camera> ip_camera);
    static v8::Local<v8::Object> Instantiate(v8::Local<v8::Object> i_params);

    intrusive_ptr<const Camera> GetCamera() const;

  protected:
    intrusive_ptr<const Camera> mp_camera;

  private:
    explicit CameraWrapper(intrusive_ptr<const Camera> ip_camera);
    static NAN_METHOD(New);

    static NAN_METHOD(GetCameraParams);

    static Nan::Persistent<v8::Function> m_constructor;
  };

#endif // CAMERA_WRAPPER_H