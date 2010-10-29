#ifndef PBRT_SCENE_IMPORTER_H
#define PBRT_SCENE_IMPORTER_H

#include <Common/Common.h>
#include <CADImport/Core/SceneImporter.h>
#include <Raytracer/Core/Scene.h>
#include <Raytracer/Core/Camera.h>
#include "PbrtSceneImporter/PbrtParamSet.h"
#include "PbrtSceneImporter/PbrtRenderOptions.h"
#include "PbrtSceneImporter/PbrtGraphicsState.h"
#include <string>
#include <vector>

class PbrtSceneImporter: public SceneImporter
  {
  public:

    PbrtSceneImporter(std::string i_filename, intrusive_ptr<Log> ip_log = NULL);

    virtual intrusive_ptr<const Scene> GetScene() const;
    virtual std::vector<intrusive_ptr<const Camera> > GetCameras() const;

  private:
    bool _VerifyInitialized(const std::string &i_method) const;
    bool _VerifyOptions(const std::string &i_method) const;
    bool _VerifyWorld(const std::string &i_method) const;

    void _pbrtInit();
    void _pbrtCleanup();
    void _pbrtIdentity();
    void _pbrtTranslate(float dx, float dy, float dz);
    void _pbrtTransform(float tr[16]);
    void _pbrtConcatTransform(float tr[16]);
    void _pbrtRotate(float angle, float dx, float dy, float dz);
    void _pbrtScale(float sx, float sy, float sz);
    void _pbrtLookAt(float ex, float ey, float ez, float lx, float ly, float lz, float ux, float uy, float uz);
    void _pbrtCoordinateSystem(const std::string &name);
    void _pbrtCoordSysTransform(const std::string &name);
    void _pbrtPixelFilter(const std::string &name, const PbrtImport::ParamSet &params);
    void _pbrtFilm(const std::string &type, const PbrtImport::ParamSet &params);
    void _pbrtCamera(const std::string &name, const PbrtImport::ParamSet &params);
    void _pbrtWorldBegin();
    void _pbrtAttributeBegin();
    void _pbrtAttributeEnd();
    void _pbrtTransformBegin();
    void _pbrtTransformEnd();
    void _pbrtTexture(const std::string &name, const std::string &type, const std::string &texname, const PbrtImport::ParamSet &params);
    void _pbrtMaterial(const std::string &name, const PbrtImport::ParamSet &params);
    void _pbrtMakeNamedMaterial(const std::string &name, const PbrtImport::ParamSet &params);
    void _pbrtNamedMaterial(const std::string &name);
    void _pbrtLightSource(const std::string &name, const PbrtImport::ParamSet &params);
    void _pbrtAreaLightSource(const std::string &name, const PbrtImport::ParamSet &params);
    void _pbrtShape(const std::string &name, const PbrtImport::ParamSet &params);
    void _pbrtReverseOrientation();
    void _pbrtVolume(const std::string &name, const PbrtImport::ParamSet &params);
    void _pbrtObjectBegin(const std::string &name);
    void _pbrtObjectEnd();
    void _pbrtObjectInstance(const std::string &name);
    void _pbrtWorldEnd();

    void _ReadScene();
    void _ParseFile();
    bool _ReadLines(const std::string &i_filename);
    bool _AddLine(const std::string &i_line);
    bool _ProcessCommand();

    bool _CheckParameters(const std::vector<std::string> &i_parts, size_t i_num, const std::string &i_command_name) const;
    bool _ParseParamSet(const std::vector<std::string> &i_parts, PbrtImport::ParamSet &o_params, size_t i_first = 2) const;


  private:
    static const int STATE_UNINITIALIZED = 0;
    static const int STATE_OPTIONS_BLOCK = 1;
    static const int STATE_WORLD_BLOCK = 2;

  private:
    std::string m_filename;
    std::vector<std::string> m_pushed_filenames, m_known_commands;

    std::string m_buffer;

    intrusive_ptr<Log> mp_log;
    intrusive_ptr<const Scene> mp_scene;
    intrusive_ptr<const Camera> mp_camera;

    int m_currentApiState;
    shared_ptr<PbrtImport::RenderOptions> mp_renderOptions;
    PbrtImport::GraphicsState m_graphicsState;

    Transform m_current_transform;
    std::map<std::string, Transform> m_namedCoordinateSystems;
    std::vector<PbrtImport::GraphicsState> m_pushedGraphicsStates;
    std::vector<Transform> m_pushedTransforms;
  };

#endif // PBRT_SCENE_IMPORTER_H
