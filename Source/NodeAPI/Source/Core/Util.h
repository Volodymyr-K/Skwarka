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

#ifndef SKWARKA_NODE_UTIL_H
#define SKWARKA_NODE_UTIL_H

#include <nan.h>
#include <string>

namespace NodeAPI
  {
  namespace Utils
    {
    inline bool HasProperty(v8::Local<v8::Object> i_params, const std::string &i_property_name)
      {
      Nan::Maybe<bool> hasValue = Nan::Has(i_params, Nan::New(i_property_name).ToLocalChecked());
      return hasValue.FromJust();
      }

    inline v8::Local<v8::Object> GetObjectProperty(v8::Local<v8::Object> i_params, const std::string &i_property_name)
      {
      Nan::MaybeLocal<v8::Value> value = Nan::Get(i_params, Nan::New(i_property_name).ToLocalChecked());
      Nan::MaybeLocal<v8::Object> maybeValue = Nan::To<v8::Object>(value.ToLocalChecked());
      return maybeValue.ToLocalChecked();
      }

    inline std::string GetStringProperty(v8::Local<v8::Object> i_params, const std::string &i_property_name)
      {
      Nan::MaybeLocal<v8::Value> value = Nan::Get(i_params, Nan::New(i_property_name).ToLocalChecked());
      Nan::MaybeLocal<v8::String> maybeValue = Nan::To<v8::String>(value.ToLocalChecked());
      std::string ret(*(Nan::Utf8String(maybeValue.ToLocalChecked())));
      return ret;
      }

    inline size_t GetUIntProperty(v8::Local<v8::Object> i_params, const std::string &i_property_name)
      {
      Nan::MaybeLocal<v8::Value> value = Nan::Get(i_params, Nan::New(i_property_name).ToLocalChecked());
      Nan::Maybe<uint32_t> maybeValue = Nan::To<uint32_t>(value.ToLocalChecked());
      return maybeValue.FromJust();
      }

    inline double GetDoubleProperty(v8::Local<v8::Object> i_params, const std::string &i_property_name)
      {
      Nan::MaybeLocal<v8::Value> value = Nan::Get(i_params, Nan::New(i_property_name).ToLocalChecked());
      Nan::Maybe<double> maybeValue = Nan::To<double>(value.ToLocalChecked());
      return maybeValue.FromJust();
      }

    template<typename Type3D>
    Type3D FromArray3(v8::Local<v8::Object> i_array)
      {
      double x = Nan::To<double>(Nan::Get(i_array, 0).ToLocalChecked()).FromJust();
      double y = Nan::To<double>(Nan::Get(i_array, 1).ToLocalChecked()).FromJust();
      double z = Nan::To<double>(Nan::Get(i_array, 2).ToLocalChecked()).FromJust();
      return Type3D(x, y, z);
      }

    template<typename Type3D>
    v8::Local<v8::Array> ToArray3(Type3D i_3d_value)
      {
      v8::Local<v8::Array> array = Nan::New<v8::Array>(3);
      for (unsigned char i = 0; i < 3; ++i)
        Nan::Set(array, i, Nan::New(i_3d_value[i]));
      return array;
      }
    }
  }

#endif // SKWARKA_NODE_UTIL_H