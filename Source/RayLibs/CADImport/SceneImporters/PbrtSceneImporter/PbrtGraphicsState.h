#ifndef PBRT_GRAPHICS_STATE_H
#define PBRT_GRAPHICS_STATE_H

#include "PbrtParamSet.h"
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/Spectrum.h>
#include <string>
#include <map>

namespace PbrtImport
  {

  ////////////////////////////////////////// GraphicsState ///////////////////////////////////////////////

  struct GraphicsState
    {
    GraphicsState(intrusive_ptr<Log> ip_log = NULL);

    intrusive_ptr<const Material> CreateMaterial(const ParamSet &i_params) const;

    std::map<std::string, intrusive_ptr<const Texture<double>>> floatTextures;
    std::map<std::string, intrusive_ptr<const Texture<SpectrumCoef_d>>> spectrumTextures;
    ParamSet materialParams;
    std::string material;
    mutable std::map<std::string, intrusive_ptr<const Material>> namedMaterials;
    std::string currentNamedMaterial;
    ParamSet areaLightParams;
    std::string areaLight;
    bool reverseOrientation;

    intrusive_ptr<Log> mp_log;
    mutable std::map<const Material *, intrusive_ptr<const Texture<double>>> m_material_to_bump_map;
    };

  };

#endif // PBRT_GRAPHICS_STATE_H
