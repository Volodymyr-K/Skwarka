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

#ifndef SCENE_WRAPPER_H
#define SCENE_WRAPPER_H

#include <nan.h>
#include <Common/Common.h>
#include <Raytracer/Core/Scene.h>

/**
* Wraps Scene as a JS object.
* The class exports GetSceneObjects method that returns all scene information in JSON.
*/
class SceneWrapper : public Nan::ObjectWrap
  {
  public:
    static NAN_MODULE_INIT(Init);

    static v8::Local<v8::Object> Instantiate(intrusive_ptr<const Scene> ip_scene);

    intrusive_ptr<const Scene> GetScene() const;

  protected:
    intrusive_ptr<const Scene> mp_scene;

  private:
    explicit SceneWrapper(intrusive_ptr<const Scene> ip_scene);
    static NAN_METHOD(New);

    static NAN_METHOD(GetSceneObjects);

    static Nan::Persistent<v8::Function> m_constructor;
  };

#endif // SCENE_WRAPPER_H