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

#ifndef BASE_WRAPPER_H
#define BASE_WRAPPER_H

#include <nan.h>
#include <Common/Common.h>

/**
* Wraps any C++ class that can be passed by an intrusive_ptr as a JS object.
*/
template<typename T>
class BaseWrapper : public Nan::ObjectWrap
  {
  public:
    static NAN_MODULE_INIT(Init);

    static v8::Local<v8::Object> Instantiate(intrusive_ptr<T> ip_scene);

    intrusive_ptr<T> GetData() const;

  private:
    explicit BaseWrapper(intrusive_ptr<T> ip_scene);
    static NAN_METHOD(New);

    intrusive_ptr<T> mp_data;

    static Nan::Persistent<v8::Function> m_constructor;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
BaseWrapper<T>::BaseWrapper(intrusive_ptr<T> ip_data) : Nan::ObjectWrap()
  {
  mp_data = ip_data;
  }

template<typename T>
NAN_MODULE_INIT(BaseWrapper<T>::Init)
  {
  std::string class_name = std::string("BaseWrapper<") + typeid(T).name() + std::string(">");
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(BaseWrapper<T>::New);
  tpl->SetClassName(Nan::New(class_name).ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  m_constructor.Reset(tpl->GetFunction());
  }

template<typename T>
v8::Local<v8::Object> BaseWrapper<T>::Instantiate(intrusive_ptr<T> ip_obj)
  {
  BaseWrapper<T> *p_obj = new BaseWrapper<T>(ip_obj);
  v8::Local<v8::Value> arg[1] = { Nan::New<v8::External>(p_obj) };
  v8::Local<v8::Object> handle = Nan::New(m_constructor)->NewInstance(1, arg);
  return handle;
  }

template<typename T>
intrusive_ptr<T> BaseWrapper<T>::GetData() const
  {
  return mp_data;
  }

template<typename T>
NAN_METHOD(BaseWrapper<T>::New)
  {
  if (info.IsConstructCall() && info[0]->IsExternal())
    {
    BaseWrapper<T> *p_obj = static_cast<BaseWrapper<T>*>(v8::External::Cast(*(info[0]))->Value());
    p_obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    }
  else
    Nan::ThrowError("Direct instantiation is not supported");
  }

#endif // BASE_WRAPPER_H