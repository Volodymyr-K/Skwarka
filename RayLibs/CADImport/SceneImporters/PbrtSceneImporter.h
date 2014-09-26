#ifndef PBRT_SCENE_IMPORTER_H
#define PBRT_SCENE_IMPORTER_H

#include <Common/Common.h>
#include <CADImport/Core/SceneImporter.h>
#include <Raytracer/Core/Scene.h>
#include <Raytracer/Core/Camera.h>
#include "PbrtSceneImporter/PbrtParamSet.h"
#include "PbrtSceneImporter/PbrtRenderOptions.h"
#include "PbrtSceneImporter/PbrtGraphicsState.h"
#include "PbrtSceneImporter/PbrtStringRoutines.h"
#include "PbrtSceneImporter/Factories/TextureFactory.h"
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
    void _pbrtTranslate(float i_dx, float i_dy, float i_dz);
    void _pbrtTransform(float i_tr[16]);
    void _pbrtConcatTransform(float i_tr[16]);
    void _pbrtRotate(float i_angle, float i_dx, float i_dy, float i_dz);
    void _pbrtScale(float i_sx, float i_sy, float i_sz);
    void _pbrtLookAt(float i_ex, float i_ey, float i_ez, float i_lx, float i_ly, float i_lz, float i_ux, float i_uy, float i_uz);
    void _pbrtCoordinateSystem(PbrtImport::SubString i_name);
    void _pbrtCoordSysTransform(PbrtImport::SubString i_name);
    void _pbrtPixelFilter(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params);
    void _pbrtFilm(PbrtImport::SubString i_type, const PbrtImport::ParamSet &i_params);
    void _pbrtCamera(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params);
    void _pbrtWorldBegin();
    void _pbrtAttributeBegin();
    void _pbrtAttributeEnd();
    void _pbrtTransformBegin();
    void _pbrtTransformEnd();
    void _pbrtTexture(PbrtImport::SubString i_name, PbrtImport::SubString i_type, PbrtImport::SubString i_texname, const PbrtImport::ParamSet &i_params);
    void _pbrtMaterial(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params);
    void _pbrtMakeNamedMaterial(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params);
    void _pbrtNamedMaterial(PbrtImport::SubString i_name);
    void _pbrtLightSource(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params);
    void _pbrtAreaLightSource(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params);
    void _pbrtShape(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params);
    void _pbrtReverseOrientation();
    void _pbrtVolume(PbrtImport::SubString i_name, const PbrtImport::ParamSet &i_params);
    void _pbrtObjectBegin(PbrtImport::SubString i_name);
    void _pbrtObjectEnd();
    void _pbrtObjectInstance(PbrtImport::SubString i_name);
    void _pbrtWorldEnd();

    void _ReadScene();
    void _ParseFile();
    bool _ReadLines(const std::string &i_filename);
    bool _AddLine(const char *i_begin, const char *i_end);
    bool _ProcessCommand();

    bool _CheckParameters(const std::vector<PbrtImport::SubString> &i_parts, size_t i_num, const std::string &i_command_name) const;
    bool _ParseParamSet(const std::vector<PbrtImport::SubString> &i_parts, PbrtImport::ParamSet &o_params, size_t i_first = 2) const;


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

    PbrtImport::TextureFactory m_texture_factory;
  };

#endif // PBRT_SCENE_IMPORTER_H
