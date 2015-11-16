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

#include "CameraWrapper.h"
#include <Raytracer/Cameras/PerspectiveCamera.h>

Nan::Persistent<v8::Function> CameraWrapper::m_constructor;

CameraWrapper::CameraWrapper(intrusive_ptr<const Camera> ip_camera) : Nan::ObjectWrap()
  {
  mp_camera = ip_camera;
  }

NAN_MODULE_INIT(CameraWrapper::Init)
  {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(CameraWrapper::New);
  tpl->SetClassName(Nan::New("CameraWrapper").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "getCameraProperties", GetCameraProperties);

  m_constructor.Reset(tpl->GetFunction());
  }

v8::Local<v8::Object> CameraWrapper::Instantiate(intrusive_ptr<const Camera> ip_camera)
  {
  CameraWrapper *p_camera = new CameraWrapper(ip_camera);
  v8::Local<v8::Value> arg[1] = { Nan::New<v8::External>(p_camera) };
  v8::Local<v8::Object> handle = Nan::New(m_constructor)->NewInstance(1, arg);
  return handle;
  }

intrusive_ptr<const Camera> CameraWrapper::GetCamera() const
  {
  return mp_camera;
  }

NAN_METHOD(CameraWrapper::New)
  {
  if (info.IsConstructCall() && info[0]->IsExternal())
    {
    CameraWrapper *p_obj = static_cast<CameraWrapper*>(v8::External::Cast(*(info[0]))->Value());
    p_obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    }
  else
    Nan::ThrowError("Direct instantiation is not supported");
  }

NAN_METHOD(CameraWrapper::GetCameraProperties)
  {
  CameraWrapper *p_this = Nan::ObjectWrap::Unwrap<CameraWrapper>(info.This());

  v8::Local<v8::Object> camera = Nan::New<v8::Object>();
  Transform camera2world = p_this->mp_camera->GetCamera2WorldTransform();

  Point3D_d origin = camera2world(Point3D_d());
  v8::Local<v8::Array> origin_array = Nan::New<v8::Array>(3);
  for (unsigned char k = 0; k < 3; ++k) Nan::Set(origin_array, k, Nan::New(origin[k]));
  Nan::Set(camera, Nan::New("origin").ToLocalChecked(), origin_array);

  Vector3D_d direction = camera2world(Vector3D_d(0, 0, 1));
  v8::Local<v8::Array> direction_array = Nan::New<v8::Array>(3);
  for (unsigned char k = 0; k < 3; ++k) Nan::Set(direction_array, k, Nan::New(direction[k]));
  Nan::Set(camera, Nan::New("direction").ToLocalChecked(), direction_array);

  Vector3D_d up = camera2world(Vector3D_d(0, -1, 0));
  v8::Local<v8::Array> up_array = Nan::New<v8::Array>(3);
  for (unsigned char k = 0; k < 3; ++k) Nan::Set(up_array, k, Nan::New(up[k]));
  Nan::Set(camera, Nan::New("up").ToLocalChecked(), up_array);

  Nan::Set(camera, Nan::New("width").ToLocalChecked(), Nan::New((uint32_t)p_this->mp_camera->GetFilm()->GetXResolution()));
  Nan::Set(camera, Nan::New("height").ToLocalChecked(), Nan::New((uint32_t)p_this->mp_camera->GetFilm()->GetYResolution()));

  const PerspectiveCamera *p_perspective_camera = dynamic_cast<const PerspectiveCamera *>(p_this->mp_camera.get());
  if (p_perspective_camera)
    {
    double fov = p_perspective_camera->GetXViewAngle() * 180 / M_PI;
    Nan::Set(camera, Nan::New("fov").ToLocalChecked(), Nan::New(fov));
    }
  else
    Nan::Set(camera, Nan::New("fov").ToLocalChecked(), Nan::New(60));

  info.GetReturnValue().Set(camera);
  }