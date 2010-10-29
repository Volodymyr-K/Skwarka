#ifndef PBRT_RENDER_OPTIONS_H
#define PBRT_RENDER_OPTIONS_H

#include "PbrtParamSet.h"
#include <Math/Geometry.h>
#include <Raytracer/Core/LightSources.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/Scene.h>
#include <Raytracer/Core/Camera.h>
#include <Raytracer/Core/FilmFilter.h>
#include <string>
#include <map>

namespace PbrtImport
  {

  ////////////////////////////////////////// RenderOptions ///////////////////////////////////////////////

  struct RenderOptions
    {
    RenderOptions(intrusive_ptr<Log> ip_log = NULL);

    intrusive_ptr<const Scene> MakeScene();
    intrusive_ptr<const Camera> MakeCamera() const;

    bool _VerifyInitialized(const std::string &i_method) const;
    bool _VerifyOptions(const std::string &i_method) const;

    std::string FilterName;
    ParamSet FilterParams;
    std::string FilmName;
    ParamSet FilmParams;
    std::string CameraName;
    ParamSet CameraParams;
    Transform CameraToWorld;

    LightSources lights;

    std::vector<intrusive_ptr<const Primitive> > primitives;
    std::vector<intrusive_ptr<const VolumeRegion> > volumeRegions;
    std::map<std::string, std::vector<intrusive_ptr<const Primitive> > > instances;
    std::vector<intrusive_ptr<const Primitive> > *currentInstance;

    std::vector<std::string> m_delayed_light_names;
    std::vector<Transform> m_delayed_light_transforms;
    std::vector<PbrtImport::ParamSet> m_delayed_light_params;

    intrusive_ptr<Log> mp_log;
    };

  };

#endif // PBRT_RENDER_OPTIONS_H
