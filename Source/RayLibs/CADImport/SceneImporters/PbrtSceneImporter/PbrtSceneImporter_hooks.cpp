/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
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

#include "../PbrtSceneImporter.h"
#include "Factories/TextureFactory.h"
#include "Factories/MaterialFactory.h"
#include "Factories/ShapeFactory.h"
#include "Factories/LightSourceFactory.h"
#include "Factories/VolumeRegionFactory.h"
#include <Math/MathRoutines.h>

void PbrtSceneImporter::_pbrtInit()
  {
  if (m_currentApiState != STATE_UNINITIALIZED)
    PbrtImport::Utils::LogError(mp_log, "_pbrtInit() has already been called.");

  m_currentApiState = STATE_OPTIONS_BLOCK;
  mp_renderOptions.reset( new PbrtImport::RenderOptions(mp_log) );
  m_graphicsState = PbrtImport::GraphicsState(mp_log);
  }

void PbrtSceneImporter::_pbrtCleanup()
  {
  if (m_currentApiState == STATE_UNINITIALIZED)
    PbrtImport::Utils::LogError(mp_log, "_PbrtCleanup() called without _pbrtInit().");
  else if (m_currentApiState == STATE_WORLD_BLOCK)
    PbrtImport::Utils::LogError(mp_log, "_PbrtCleanup() called while inside world block.");

  m_currentApiState = STATE_UNINITIALIZED;
  mp_renderOptions.reset((PbrtImport::RenderOptions *)NULL);
  }

void PbrtSceneImporter::_pbrtIdentity()
  {
  if (_VerifyInitialized("Identity")==false) return;
  m_current_transform = Transform();
  }

void PbrtSceneImporter::_pbrtTranslate(float i_dx, float i_dy, float i_dz)
  {
  if (_VerifyInitialized("Translate")==false) return;

  m_current_transform = m_current_transform * MakeTranslation(Vector3D_d(i_dx, i_dy, i_dz));
  }

void PbrtSceneImporter::_pbrtTransform(float i_tr[16])
  {
  if (_VerifyInitialized("Transform")==false) return;

  m_current_transform = Transform(Matrix4x4_d(
    i_tr[0], i_tr[4], i_tr[8], i_tr[12],
    i_tr[1], i_tr[5], i_tr[9], i_tr[13],
    i_tr[2], i_tr[6], i_tr[10], i_tr[14],
    i_tr[3], i_tr[7], i_tr[11], i_tr[15]));
  }

void PbrtSceneImporter::_pbrtConcatTransform(float i_tr[16])
  {
  if (_VerifyInitialized("ConcatTransform")==false) return;

  m_current_transform = m_current_transform * Transform(Matrix4x4_d(
    i_tr[0], i_tr[4], i_tr[8], i_tr[12],
    i_tr[1], i_tr[5], i_tr[9], i_tr[13],
    i_tr[2], i_tr[6], i_tr[10], i_tr[14],
    i_tr[3], i_tr[7], i_tr[11], i_tr[15]));
  }

void PbrtSceneImporter::_pbrtRotate(float i_angle, float i_dx, float i_dy, float i_dz)
  {
  if (_VerifyInitialized("Rotate")==false) return;

  m_current_transform = m_current_transform * MakeRotation(MathRoutines::DegreesToRadians(i_angle), Vector3D_d(i_dx, i_dy, i_dz));
  }

void PbrtSceneImporter::_pbrtScale(float i_sx, float i_sy, float i_sz)
  {
  if (_VerifyInitialized("Scale")==false) return;

  m_current_transform = m_current_transform * MakeScale(i_sx, i_sy, i_sz);
  }

void PbrtSceneImporter::_pbrtLookAt(float i_ex, float i_ey, float i_ez, float i_lx, float i_ly,
                                    float i_lz, float i_ux, float i_uy, float i_uz)
  {
  if (_VerifyInitialized("LookAt")==false) return;

  m_current_transform = m_current_transform * MakeLookAt(Point3D_d(i_ex, i_ey, i_ez),
    Vector3D_d(i_lx-i_ex, i_ly-i_ey, i_lz-i_ez).Normalized(), Vector3D_d(i_ux, i_uy, i_uz));
  }

void PbrtSceneImporter::_pbrtCoordinateSystem(PbrtImport::SubString i_name)
  {
  if (_VerifyInitialized("CoordinateSystem")==false) return;

  m_namedCoordinateSystems[i_name.to_string()] = m_current_transform;
  }

void PbrtSceneImporter::_pbrtCoordSysTransform(PbrtImport::SubString i_name)
  {
  if (_VerifyInitialized("CoordSysTransform")==false) return;

  std::string name(i_name.to_string());
  if (m_namedCoordinateSystems.find(name) != m_namedCoordinateSystems.end())
    m_current_transform = m_namedCoordinateSystems[name];
  else
    PbrtImport::Utils::LogError(mp_log, std::string("Couldn't find named coordinate system \"") + name + std::string("\"."));
  }

void PbrtSceneImporter::_pbrtPixelFilter(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params)
  {
  if (_VerifyOptions("PixelFilter")==false) return;

  mp_renderOptions->FilterName = i_name.to_string();
  mp_renderOptions->FilterParams = i_params;
  }

void PbrtSceneImporter::_pbrtFilm(PbrtImport::SubString i_type, const PbrtImport::ParamSet &i_params)
  {
  if (_VerifyOptions("Film")==false) return;

  mp_renderOptions->FilmParams = i_params;
  mp_renderOptions->FilmName = i_type.to_string();
  }

void PbrtSceneImporter::_pbrtCamera(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params)
  {
  if (_VerifyOptions("Camera")==false) return;

  mp_renderOptions->CameraName = i_name.to_string();
  mp_renderOptions->CameraParams = i_params;

  // Need to reflect the camera against the X axis to account for the fact that pbrt is based on the left-handed coordinate system, while our system is right-handed
  mp_renderOptions->CameraToWorld = (MakeScale(-1, 1, 1)*m_current_transform*MakeScale(1, 1, 1)).Inverted();
  m_namedCoordinateSystems["camera"] = mp_renderOptions->CameraToWorld;
  }

void PbrtSceneImporter::_pbrtWorldBegin()
  {
  if (_VerifyOptions("WorldBegin")==false) return;

  m_currentApiState = STATE_WORLD_BLOCK;
  m_current_transform = Transform();
  m_namedCoordinateSystems["world"] = m_current_transform;
  }

void PbrtSceneImporter::_pbrtAttributeBegin()
  {
  if (_VerifyWorld("AttributeBegin")==false) return;

  m_pushedGraphicsStates.push_back(m_graphicsState);
  m_pushedTransforms.push_back(m_current_transform);
  }

void PbrtSceneImporter::_pbrtAttributeEnd()
  {
  if (_VerifyWorld("AttributeEnd")==false) return;

  if (!m_pushedGraphicsStates.size())
    {
    PbrtImport::Utils::LogError(mp_log, "Unmatched AttributeEnd encountered. Ignoring it.");
    return;
    }

  m_graphicsState = m_pushedGraphicsStates.back();
  m_pushedGraphicsStates.pop_back();
  m_current_transform = m_pushedTransforms.back();
  m_pushedTransforms.pop_back();
  }

void PbrtSceneImporter::_pbrtTransformBegin()
  {
  if (_VerifyWorld("TransformBegin")==false) return;

  m_pushedTransforms.push_back(m_current_transform);
  }

void PbrtSceneImporter::_pbrtTransformEnd()
  {
  if (_VerifyWorld("TransformEnd")==false) return;

  if (!m_pushedTransforms.size())
    {
    PbrtImport::Utils::LogError(mp_log, "Unmatched TransformEnd encountered. Ignoring it.");
    return;
    }

  m_current_transform = m_pushedTransforms.back();
  m_pushedTransforms.pop_back();
  }

void PbrtSceneImporter::_pbrtTexture(PbrtImport::SubString i_name, PbrtImport::SubString i_type, PbrtImport::SubString i_texname, const PbrtImport::ParamSet &i_params)
  {
  if (_VerifyWorld("Texture")==false) return;

  std::string name(i_name.to_string()), type(i_type.to_string()), texname(i_texname.to_string());
  PbrtImport::TextureParams tp(i_params, i_params, m_graphicsState.floatTextures, m_graphicsState.spectrumTextures);
  if (type == "float")
    {
    intrusive_ptr<const Texture<double>> ft = m_texture_factory.CreateFloatTexture(texname, m_current_transform, tp);
    if (ft) m_graphicsState.floatTextures[name] = ft;
    }
  else if (type == "color") 
    {
    intrusive_ptr<const Texture<SpectrumCoef_d>> st = m_texture_factory.CreateSpectrumCoefTexture(texname, m_current_transform, tp);
    if (st) m_graphicsState.spectrumTextures[name] = st;
    }
  else
    PbrtImport::Utils::LogError(mp_log, std::string("Texture type ") + type + std::string(" unknown."));
  }

void PbrtSceneImporter::_pbrtMaterial(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params)
  {
  if (_VerifyWorld("Material")==false) return;

  m_graphicsState.material = i_name.to_string();
  m_graphicsState.materialParams = i_params;
  m_graphicsState.currentNamedMaterial = "";
  }

void PbrtSceneImporter::_pbrtMakeNamedMaterial(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params)
  {
  if (_VerifyWorld("MakeNamedMaterial")==false) return;
  PbrtImport::MaterialFactory material_factory(m_graphicsState.m_material_to_bump_map, mp_log);

  PbrtImport::TextureParams mp(i_params, m_graphicsState.materialParams, m_graphicsState.floatTextures, m_graphicsState.spectrumTextures);
  std::string matName = mp.FindString("type");

  if (matName == "")
    PbrtImport::Utils::LogError(mp_log, "No parameter string \"type\" found in _pbrtMakeNamedMaterial");
  else
    {
    intrusive_ptr<const Material> mtl = material_factory.CreateMaterial(matName, mp, m_graphicsState);
    if (mtl) m_graphicsState.namedMaterials[i_name.to_string()] = mtl;
    }
  }

void PbrtSceneImporter::_pbrtNamedMaterial(PbrtImport::SubString i_name)
  {
  if (_VerifyWorld("NamedMaterial")==false) return;

  m_graphicsState.currentNamedMaterial = i_name.to_string();
  }

void PbrtSceneImporter::_pbrtLightSource(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params)
  {
  if (_VerifyWorld("LightSource")==false) return;

  mp_renderOptions->m_delayed_light_names.push_back(i_name.to_string());
  mp_renderOptions->m_delayed_light_transforms.push_back(m_current_transform);
  mp_renderOptions->m_delayed_light_params.push_back(i_params);
  }

void PbrtSceneImporter::_pbrtAreaLightSource(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params)
  {
  if (_VerifyWorld("AreaLightSource")==false) return;

  m_graphicsState.areaLight = i_name.to_string();
  m_graphicsState.areaLightParams = i_params;
  }

void PbrtSceneImporter::_pbrtShape(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params)
  {
  if (_VerifyWorld("Shape")==false) return;

  intrusive_ptr<const AreaLightSource> p_area;

  // Create primitive for static shape.
  PbrtImport::ShapeFactory shape_factory(mp_log);
  intrusive_ptr<const TriangleMesh> p_mesh = shape_factory.CreateShape(i_name.to_string(), m_current_transform, m_graphicsState, i_params);
  if (!p_mesh) return;

  intrusive_ptr<const Material> mtl = m_graphicsState.CreateMaterial(i_params);

  // Possibly create area light for shape
  if (m_graphicsState.areaLight != "")
    {
    PbrtImport::LightSourceFactory light_source_factory(mp_log);
    p_area = light_source_factory.CreateAreaLight(m_graphicsState.areaLight, m_current_transform, m_graphicsState.areaLightParams, p_mesh);
    }

  intrusive_ptr<const Texture<double>> p_bump_map;
  if (m_graphicsState.m_material_to_bump_map.find(mtl.get())!=m_graphicsState.m_material_to_bump_map.end())
    p_bump_map = m_graphicsState.m_material_to_bump_map.find(mtl.get())->second;

  intrusive_ptr<const Primitive> prim( new Primitive(p_mesh, Transform(), mtl, p_area, p_bump_map) );

  // Add primitive to scene or current instance
  if (mp_renderOptions->currentInstance)
    {
    if (p_area)
      PbrtImport::Utils::LogError(mp_log, "Area lights not supported with object instancing.");
    mp_renderOptions->currentInstance->push_back(prim);
    }
  else
    {
    mp_renderOptions->primitives.push_back(prim);
    if (p_area != NULL)
      mp_renderOptions->lights.m_area_light_sources.push_back(p_area);
    }
  }

void PbrtSceneImporter::_pbrtReverseOrientation()
  {
  if (_VerifyWorld("ReverseOrientation")==false) return;

  m_graphicsState.reverseOrientation = !m_graphicsState.reverseOrientation;
  }

void PbrtSceneImporter::_pbrtVolume(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params)
  {
  if (_VerifyWorld("Volume")==false) return;

  PbrtImport::VolumeRegionFactory volume_region_factory(mp_log);
  intrusive_ptr<const VolumeRegion> p_vr = volume_region_factory.CreateVolumeRegion(i_name.to_string(), m_current_transform, i_params);
  if (p_vr) mp_renderOptions->volumeRegions.push_back(p_vr);
  }

void PbrtSceneImporter::_pbrtObjectBegin(PbrtImport::SubString i_name)
  {
  if (_VerifyWorld("ObjectBegin")==false) return;

  _pbrtAttributeBegin();
  if (mp_renderOptions->currentInstance)
    {
    PbrtImport::Utils::LogError(mp_log, "ObjectBegin called inside of instance definition.");
    return;
    }

  std::string name(i_name.to_string());
  mp_renderOptions->instances[name] = std::vector<intrusive_ptr<const Primitive>>();
  mp_renderOptions->currentInstance = &mp_renderOptions->instances[name];
  }

void PbrtSceneImporter::_pbrtObjectEnd()
  {
  if (_VerifyWorld("ObjectEnd")==false) return;

  if (!mp_renderOptions->currentInstance)
    {
    PbrtImport::Utils::LogError(mp_log, "ObjectEnd called outside of instance definition");
    return;
    }

  mp_renderOptions->currentInstance = NULL;
  _pbrtAttributeEnd();
  }

void PbrtSceneImporter::_pbrtObjectInstance(PbrtImport::SubString i_name)
  {
  if (_VerifyWorld("ObjectInstance")==false) return;

  // Object instance error checking
  if (mp_renderOptions->currentInstance)
    {
    PbrtImport::Utils::LogError(mp_log, "ObjectInstance can't be called inside instance definition.");
    return;
    }

  std::string name(i_name.to_string());
  if (mp_renderOptions->instances.find(name) == mp_renderOptions->instances.end())
    {
    PbrtImport::Utils::LogError(mp_log, "Unable to find instance named " + name);
    return;
    }

  std::vector<intrusive_ptr<const Primitive>> &inst = mp_renderOptions->instances[name];
  if (inst.empty()) return;

  for(size_t i=0;i<inst.size();++i)
    {
    intrusive_ptr<const Primitive> p_prim = inst[i];
    intrusive_ptr<const Primitive> p_copy( new Primitive(p_prim->GetTriangleMesh(), m_current_transform,
      p_prim->GetMaterial(), p_prim->GetAreaLightSource(), p_prim->GetBumpMap()) );
    mp_renderOptions->primitives.push_back(p_copy);
    }
  }

void PbrtSceneImporter::_pbrtWorldEnd()
  {
  if (_VerifyWorld("WorldEnd")==false) return;

  // Ensure there are no pushed graphics states
  while (m_pushedGraphicsStates.size())
    {
    PbrtImport::Utils::LogError(mp_log, "Missing AttributeEnd command.");
    m_pushedGraphicsStates.pop_back();
    m_pushedTransforms.pop_back();
    }

  while (m_pushedTransforms.size())
    {
    PbrtImport::Utils::LogError(mp_log, "Missing TransformEnd command.");
    m_pushedTransforms.pop_back();
    }

  m_graphicsState = PbrtImport::GraphicsState();
  m_currentApiState = STATE_OPTIONS_BLOCK;
  m_namedCoordinateSystems.clear();
  }