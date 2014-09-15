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

void PbrtSceneImporter::_pbrtTranslate(float dx, float dy, float dz)
  {
  if (_VerifyInitialized("Translate")==false) return;

  m_current_transform = m_current_transform * MakeTranslation(Vector3D_d(dx, dy, dz));
  }

void PbrtSceneImporter::_pbrtTransform(float tr[16])
  {
  if (_VerifyInitialized("Transform")==false) return;

  m_current_transform = Transform(Matrix4x4_d(
    tr[0], tr[4], tr[8], tr[12],
    tr[1], tr[5], tr[9], tr[13],
    tr[2], tr[6], tr[10], tr[14],
    tr[3], tr[7], tr[11], tr[15]));
  }

void PbrtSceneImporter::_pbrtConcatTransform(float tr[16])
  {
  if (_VerifyInitialized("ConcatTransform")==false) return;

  m_current_transform = m_current_transform * Transform(Matrix4x4_d(
    tr[0], tr[4], tr[8], tr[12],
    tr[1], tr[5], tr[9], tr[13],
    tr[2], tr[6], tr[10], tr[14],
    tr[3], tr[7], tr[11], tr[15]));
  }

void PbrtSceneImporter::_pbrtRotate(float angle, float dx, float dy, float dz)
  {
  if (_VerifyInitialized("Rotate")==false) return;

  m_current_transform = m_current_transform * MakeRotation(MathRoutines::DegreesToRadians(angle), Vector3D_d(dx, dy, dz));
  }

void PbrtSceneImporter::_pbrtScale(float sx, float sy, float sz)
  {
  if (_VerifyInitialized("Scale")==false) return;

  m_current_transform = m_current_transform * MakeScale(sx, sy, sz);
  }

void PbrtSceneImporter::_pbrtLookAt(float ex, float ey, float ez, float lx, float ly,
                                    float lz, float ux, float uy, float uz)
  {
  if (_VerifyInitialized("LookAt")==false) return;

  m_current_transform = m_current_transform * MakeLookAt(Point3D_d(ex, ey, ez),
    Vector3D_d(lx-ex, ly-ey, lz-ez).Normalized(), Vector3D_d(ux, uy, uz));
  }

void PbrtSceneImporter::_pbrtCoordinateSystem(const std::string &name)
  {
  if (_VerifyInitialized("CoordinateSystem")==false) return;

  m_namedCoordinateSystems[name] = m_current_transform;
  }

void PbrtSceneImporter::_pbrtCoordSysTransform(const std::string &name)
  {
  if (_VerifyInitialized("CoordSysTransform")==false) return;

  if (m_namedCoordinateSystems.find(name) != m_namedCoordinateSystems.end())
    m_current_transform = m_namedCoordinateSystems[name];
  else
    PbrtImport::Utils::LogWarning(mp_log, std::string("Couldn't find named coordinate system \"") + name + std::string("\"."));
  }

void PbrtSceneImporter::_pbrtPixelFilter(const std::string &name, const PbrtImport::ParamSet &params)
  {
  if (_VerifyOptions("PixelFilter")==false) return;

  mp_renderOptions->FilterName = name;
  mp_renderOptions->FilterParams = params;
  }

void PbrtSceneImporter::_pbrtFilm(const std::string &type, const PbrtImport::ParamSet &params)
  {
  if (_VerifyOptions("Film")==false) return;

  mp_renderOptions->FilmParams = params;
  mp_renderOptions->FilmName = type;
  }

void PbrtSceneImporter::_pbrtCamera(const std::string &name, const PbrtImport::ParamSet &params)
  {
  if (_VerifyOptions("Camera")==false) return;

  mp_renderOptions->CameraName = name;
  mp_renderOptions->CameraParams = params;
  mp_renderOptions->CameraToWorld = (m_current_transform*MakeScale(-1, 1, 1)).Inverted();
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

void PbrtSceneImporter::_pbrtTexture(const std::string &name, const std::string &type,
                                     const std::string &texname, const PbrtImport::ParamSet &params)
  {
  if (_VerifyWorld("Texture")==false) return;
  PbrtImport::TextureFactory texture_factory(mp_log);

  PbrtImport::TextureParams tp(params, params, m_graphicsState.floatTextures, m_graphicsState.spectrumTextures);
  if (type == "float")
    {
    if (m_graphicsState.floatTextures.find(name) != m_graphicsState.floatTextures.end())
      PbrtImport::Utils::LogInfo(mp_log, std::string("Texture ") + name + std::string(" being redefined."));

    intrusive_ptr<const Texture<double> > ft = texture_factory.CreateFloatTexture(texname, m_current_transform, tp);
    if (ft) m_graphicsState.floatTextures[name] = ft;
    }
  else if (type == "color") 
    {
    if (m_graphicsState.spectrumTextures.find(name) != m_graphicsState.spectrumTextures.end())
      PbrtImport::Utils::LogInfo(mp_log, std::string("Texture ") + name + std::string(" being redefined."));

    intrusive_ptr<const Texture<SpectrumCoef_d> > st = texture_factory.CreateSpectrumCoefTexture(texname, m_current_transform, tp);
    if (st) m_graphicsState.spectrumTextures[name] = st;
    }
  else
    PbrtImport::Utils::LogError(mp_log, std::string("Texture type ") + type + std::string(" unknown."));
  }

void PbrtSceneImporter::_pbrtMaterial(const std::string &name, const PbrtImport::ParamSet &params)
  {
  if (_VerifyWorld("Material")==false) return;

  m_graphicsState.material = name;
  m_graphicsState.materialParams = params;
  m_graphicsState.currentNamedMaterial = "";
  }

void PbrtSceneImporter::_pbrtMakeNamedMaterial(const std::string &name, const PbrtImport::ParamSet &params)
  {
  if (_VerifyWorld("MakeNamedMaterial")==false) return;
  PbrtImport::MaterialFactory material_factory(m_graphicsState.m_material_to_bump_map, mp_log);

  PbrtImport::TextureParams mp(params, m_graphicsState.materialParams, m_graphicsState.floatTextures, m_graphicsState.spectrumTextures);
  std::string matName = mp.FindString("type");

  if (matName == "")
    PbrtImport::Utils::LogError(mp_log, "No parameter string \"type\" found in _pbrtMakeNamedMaterial");
  else
    {
    intrusive_ptr<const Material> mtl = material_factory.CreateMaterial(matName, mp, m_graphicsState);
    if (mtl) m_graphicsState.namedMaterials[name] = mtl;
    }
  }

void PbrtSceneImporter::_pbrtNamedMaterial(const std::string &name)
  {
  if (_VerifyWorld("NamedMaterial")==false) return;

  m_graphicsState.currentNamedMaterial = name;
  }

void PbrtSceneImporter::_pbrtLightSource(const std::string &name, const PbrtImport::ParamSet &params)
  {
  if (_VerifyWorld("LightSource")==false) return;

  mp_renderOptions->m_delayed_light_names.push_back(name);
  mp_renderOptions->m_delayed_light_transforms.push_back(m_current_transform);
  mp_renderOptions->m_delayed_light_params.push_back(params);
  }

void PbrtSceneImporter::_pbrtAreaLightSource(const std::string &name, const PbrtImport::ParamSet &params)
  {
  if (_VerifyWorld("AreaLightSource")==false) return;

  m_graphicsState.areaLight = name;
  m_graphicsState.areaLightParams = params;
  }

void PbrtSceneImporter::_pbrtShape(const std::string &name, const PbrtImport::ParamSet &params)
  {
  if (_VerifyWorld("Shape")==false) return;

  intrusive_ptr<const AreaLightSource> p_area;

  // Create primitive for static shape.
  PbrtImport::ShapeFactory shape_factory(mp_log);
  intrusive_ptr<const TriangleMesh> p_mesh = shape_factory.CreateShape(name, m_current_transform, m_graphicsState, params);
  if (!p_mesh) return;

  intrusive_ptr<const Material> mtl = m_graphicsState.CreateMaterial(params);

  // Possibly create area light for shape
  if (m_graphicsState.areaLight != "")
    {
    PbrtImport::LightSourceFactory light_source_factory(mp_log);
    p_area = light_source_factory.CreateAreaLight(m_graphicsState.areaLight, m_current_transform, m_graphicsState.areaLightParams, p_mesh);
    }

  intrusive_ptr<const Texture<double> > p_bump_map;
  if (m_graphicsState.m_material_to_bump_map.find(mtl.get())!=m_graphicsState.m_material_to_bump_map.end())
    p_bump_map = m_graphicsState.m_material_to_bump_map.find(mtl.get())->second;

  intrusive_ptr<const Primitive> prim( new Primitive(p_mesh, Transform(), mtl, p_area, p_bump_map) );

  // Add primitive to scene or current instance
  if (mp_renderOptions->currentInstance)
    {
    if (p_area)
      PbrtImport::Utils::LogWarning(mp_log, "Area lights not supported with object instancing.");
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

void PbrtSceneImporter::_pbrtVolume(const std::string &name, const PbrtImport::ParamSet &params)
  {
  if (_VerifyWorld("Volume")==false) return;

  PbrtImport::VolumeRegionFactory volume_region_factory(mp_log);
  intrusive_ptr<const VolumeRegion> p_vr = volume_region_factory.CreateVolumeRegion(name, m_current_transform, params);
  if (p_vr) mp_renderOptions->volumeRegions.push_back(p_vr);
  }

void PbrtSceneImporter::_pbrtObjectBegin(const std::string &name)
  {
  if (_VerifyWorld("ObjectBegin")==false) return;

  _pbrtAttributeBegin();
  if (mp_renderOptions->currentInstance)
    {
    PbrtImport::Utils::LogError(mp_log, "ObjectBegin called inside of instance definition.");
    return;
    }

  mp_renderOptions->instances[name] = std::vector<intrusive_ptr<const Primitive> >();
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

void PbrtSceneImporter::_pbrtObjectInstance(const std::string &name)
  {
  if (_VerifyWorld("ObjectInstance")==false) return;

  // Object instance error checking
  if (mp_renderOptions->currentInstance)
    {
    PbrtImport::Utils::LogError(mp_log, "ObjectInstance can't be called inside instance definition.");
    return;
    }

  if (mp_renderOptions->instances.find(name) == mp_renderOptions->instances.end())
    {
    PbrtImport::Utils::LogError(mp_log, "Unable to find instance named " + name);
    return;
    }

  std::vector<intrusive_ptr<const Primitive> > &inst = mp_renderOptions->instances[name];
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
    PbrtImport::Utils::LogWarning(mp_log, "Missing end to pbrtAttributeBegin()");
    m_pushedGraphicsStates.pop_back();
    m_pushedTransforms.pop_back();
    }

  while (m_pushedTransforms.size())
    {
    PbrtImport::Utils::LogWarning(mp_log, "Missing end to pbrtTransformBegin()");
    m_pushedTransforms.pop_back();
    }

  m_graphicsState = PbrtImport::GraphicsState();
  m_currentApiState = STATE_OPTIONS_BLOCK;
  m_namedCoordinateSystems.clear();
  }