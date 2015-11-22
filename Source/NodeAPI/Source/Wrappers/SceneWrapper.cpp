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

#include "SceneWrapper.h"
#include <Core/Util.h>

#include <map>

Nan::Persistent<v8::Function> SceneWrapper::m_constructor;

SceneWrapper::SceneWrapper(intrusive_ptr<const Scene> ip_scene) : Nan::ObjectWrap()
  {
  mp_scene = ip_scene;
  }

NAN_MODULE_INIT(SceneWrapper::Init)
  {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(SceneWrapper::New);
  tpl->SetClassName(Nan::New("SceneWrapper").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "getSceneObjects", GetSceneObjects);

  m_constructor.Reset(tpl->GetFunction());
  }

v8::Local<v8::Object> SceneWrapper::Instantiate(intrusive_ptr<const Scene> ip_scene)
  {
  SceneWrapper *p_scene = new SceneWrapper(ip_scene);
  v8::Local<v8::Value> arg[1] = { Nan::New<v8::External>(p_scene) };
  v8::Local<v8::Object> handle = Nan::New(m_constructor)->NewInstance(1, arg);
  return handle;
  }

intrusive_ptr<const Scene> SceneWrapper::GetScene() const
  {
  return mp_scene;
  }

NAN_METHOD(SceneWrapper::New)
  {
  if (info.IsConstructCall() && info[0]->IsExternal())
    {
    SceneWrapper *p_obj = static_cast<SceneWrapper*>(v8::External::Cast(*(info[0]))->Value());
    p_obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    }
  else
    Nan::ThrowError("Direct instantiation is not supported");
  }

NAN_METHOD(SceneWrapper::GetSceneObjects)
  {
  SceneWrapper *p_this = Nan::ObjectWrap::Unwrap<SceneWrapper>(info.This());

  v8::Local<v8::Object> ret = Nan::New<v8::Object>();
  intrusive_ptr<const Scene> p_scene = p_this->GetScene();
  _ExportPrimitives(p_scene, ret);

  info.GetReturnValue().Set(ret);
  }

void SceneWrapper::_ExportPrimitives(intrusive_ptr<const Scene> ip_scene, v8::Local<v8::Object> &o_export)
  {
  std::map<const TriangleMesh*, std::vector<intrusive_ptr<const Primitive>>> primitives_by_meshes;

  // First step: partition all primitives by meshes (one mesh can be used by multiple instances).
  const std::vector<intrusive_ptr<const Primitive>> &primitives = ip_scene->GetPrimitives();
  for (auto p_primitive : primitives)
    {
    intrusive_ptr<const TriangleMesh> p_mesh = p_primitive->GetTriangleMesh();
    primitives_by_meshes[p_mesh.get()].push_back(p_primitive);
    }

  // Second step: go through every mesh, and for each mesh add it to JSON and add all primitives using this mesh to JSON too.
  uint32_t mesh_counter = 0, primitive_counter = 0;
  v8::Local<v8::Array> meshes_array = Nan::New<v8::Array>((uint32_t)primitives_by_meshes.size());
  v8::Local<v8::Array> primitives_array = Nan::New<v8::Array>((uint32_t)primitives.size());
  for (auto &it : primitives_by_meshes)
    {
    const TriangleMesh *p_mesh = it.first;

    // Export all mesh vertices.
    v8::Local<v8::Array> vertices = Nan::New<v8::Array>((uint32_t)p_mesh->GetNumberOfVertices());
    for (size_t j = 0; j < p_mesh->GetNumberOfVertices(); ++j)
      {
      Point3D_f vertex = p_mesh->GetVertex(j);
      Nan::Set(vertices, (uint32_t)j, NodeAPI::Utils::ToArray3(vertex));
      }

    // Export all mesh triangles.
    v8::Local<v8::Array> triangles = Nan::New<v8::Array>((uint32_t)p_mesh->GetNumberOfTriangles());
    for (size_t j = 0; j < p_mesh->GetNumberOfTriangles(); ++j)
      {
      MeshTriangle triangle = p_mesh->GetTriangle(j);
      v8::Local<v8::Array> triangle_array = Nan::New<v8::Array>(3);
      for (unsigned char k = 0; k < 3; ++k) Nan::Set(triangle_array, k, Nan::New((uint32_t)triangle.m_vertices[k]));

      Nan::Set(triangles, (uint32_t)j, triangle_array);
      }

    v8::Local<v8::Object> mesh = Nan::New<v8::Object>();
    Nan::Set(mesh, Nan::New("vertices").ToLocalChecked(), vertices);
    Nan::Set(mesh, Nan::New("triangles").ToLocalChecked(), triangles);
    Nan::Set(meshes_array, mesh_counter, mesh);

    // Export all primitives using this mesh.
    const std::vector<intrusive_ptr<const Primitive>> &primitive_instances = it.second;
    for (auto p_primitive : primitive_instances)
      {
      Matrix4x4_d meshToWorld = p_primitive->GetMeshToWorldTransform().GetMatrix();
      v8::Local<v8::Array> matrix_array = Nan::New<v8::Array>(16);
      for (unsigned char j = 0; j < 4; ++j)
        for (unsigned char k = 0; k < 4; ++k)
          Nan::Set(matrix_array, 4 * j + k, Nan::New(meshToWorld.m_values[j][k]));

      v8::Local<v8::Object> primitive = Nan::New<v8::Object>();
      Nan::Set(primitive, Nan::New("meshIndex").ToLocalChecked(), Nan::New(mesh_counter));
      Nan::Set(primitive, Nan::New("transformationMatrix").ToLocalChecked(), matrix_array);
      Nan::Set(primitives_array, primitive_counter++, primitive);
      }

    ++mesh_counter;
    }

  Nan::Set(o_export, Nan::New("triangleMeshes").ToLocalChecked(), meshes_array);
  Nan::Set(o_export, Nan::New("primitives").ToLocalChecked(), primitives_array);
  }