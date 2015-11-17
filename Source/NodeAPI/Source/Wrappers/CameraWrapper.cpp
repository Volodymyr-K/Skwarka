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
#include <Core/Util.h>

#include <Raytracer/FilmFilters/MitchellFilter.h>
#include <Raytracer/Films/ImageFilm.h>
#include <Raytracer/Cameras/PerspectiveCamera.h>
#include <Math/Transform.h>
#include <Math/MathRoutines.h>

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

  Nan::SetPrototypeMethod(tpl, "getCameraParams", GetCameraParams);

  m_constructor.Reset(tpl->GetFunction());
  }

v8::Local<v8::Object> CameraWrapper::Instantiate(intrusive_ptr<const Camera> ip_camera)
  {
  CameraWrapper *p_camera_wrapper = new CameraWrapper(ip_camera);
  v8::Local<v8::Value> arg[1] = { Nan::New<v8::External>(p_camera_wrapper) };
  v8::Local<v8::Object> handle = Nan::New(m_constructor)->NewInstance(1, arg);
  return handle;
  }

v8::Local<v8::Object> CameraWrapper::Instantiate(v8::Local<v8::Object> i_params)
  {
  if (
    !NodeAPI::Utils::HasProperty(i_params, "width") ||
    !NodeAPI::Utils::HasProperty(i_params, "height") ||
    !NodeAPI::Utils::HasProperty(i_params, "fov") || 
    !NodeAPI::Utils::HasProperty(i_params, "position") || 
    !NodeAPI::Utils::HasProperty(i_params, "up") || 
    !NodeAPI::Utils::HasProperty(i_params, "lookAt"))
    Nan::ThrowError("Some of the required properties are missing");

  size_t width = NodeAPI::Utils::GetUIntProperty(i_params, "width");
  size_t height = NodeAPI::Utils::GetUIntProperty(i_params, "height");
  double fov = NodeAPI::Utils::GetDoubleProperty(i_params, "fov");

  double lens_radius = NodeAPI::Utils::HasProperty(i_params, "lensRadius") ? NodeAPI::Utils::GetDoubleProperty(i_params, "lensRadius") : 0.0;
  double focal_distance = NodeAPI::Utils::HasProperty(i_params, "focalDistance") ? NodeAPI::Utils::GetDoubleProperty(i_params, "focalDistance") : 0.0;

  Point3D_d origin = NodeAPI::Utils::FromArray3<Point3D_d>(NodeAPI::Utils::GetObjectProperty(i_params, "position"));
  Point3D_d lookAt = NodeAPI::Utils::FromArray3<Point3D_d>(NodeAPI::Utils::GetObjectProperty(i_params, "lookAt"));
  Vector3D_d up = NodeAPI::Utils::FromArray3<Vector3D_d>(NodeAPI::Utils::GetObjectProperty(i_params, "up"));

  intrusive_ptr<const FilmFilter> p_filter(new MitchellFilter(2.0, 2.0));
  intrusive_ptr<Film> p_film(new ImageFilm(width, height, p_filter));

  Transform world2Camera = MakeLookAt(origin, Vector3D_d(lookAt - origin).Normalized(), up);
  intrusive_ptr<const Camera> p_camera(new PerspectiveCamera(world2Camera.Inverted(), p_film, lens_radius, focal_distance, MathRoutines::DegreesToRadians(fov)));

  return CameraWrapper::Instantiate(p_camera);
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

NAN_METHOD(CameraWrapper::GetCameraParams)
  {
  CameraWrapper *p_this = Nan::ObjectWrap::Unwrap<CameraWrapper>(info.This());

  v8::Local<v8::Object> camera = Nan::New<v8::Object>();
  Transform camera2world = p_this->mp_camera->GetCamera2WorldTransform();

  Point3D_d origin = camera2world(Point3D_d());
  Nan::Set(camera, Nan::New("origin").ToLocalChecked(), NodeAPI::Utils::ToArray3(origin));
  Nan::Set(camera, Nan::New("lookAt").ToLocalChecked(), NodeAPI::Utils::ToArray3(origin + camera2world(Vector3D_d(0, 0, 1))));
  Nan::Set(camera, Nan::New("up").ToLocalChecked(), NodeAPI::Utils::ToArray3(camera2world(Vector3D_d(0, -1, 0))));

  Nan::Set(camera, Nan::New("width").ToLocalChecked(), Nan::New((uint32_t)p_this->mp_camera->GetFilm()->GetXResolution()));
  Nan::Set(camera, Nan::New("height").ToLocalChecked(), Nan::New((uint32_t)p_this->mp_camera->GetFilm()->GetYResolution()));

  const PerspectiveCamera *p_perspective_camera = dynamic_cast<const PerspectiveCamera *>(p_this->mp_camera.get());
  if (p_perspective_camera)
    {
    Nan::Set(camera, Nan::New("fov").ToLocalChecked(), Nan::New(MathRoutines::RadiansToDegrees(p_perspective_camera->GetXViewAngle())));
    Nan::Set(camera, Nan::New("lensRadius").ToLocalChecked(), Nan::New(p_perspective_camera->GetLensRadius()));
    Nan::Set(camera, Nan::New("focalDistance").ToLocalChecked(), Nan::New(p_perspective_camera->GetFocalDistance()));
    }
  else
    Nan::Set(camera, Nan::New("fov").ToLocalChecked(), Nan::New(60));

  info.GetReturnValue().Set(camera);
  }