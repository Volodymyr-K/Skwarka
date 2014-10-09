#include "PbrtGraphicsState.h"
#include "Factories/MaterialFactory.h"
#include "PbrtUtils.h"

namespace PbrtImport
  {

  GraphicsState::GraphicsState(intrusive_ptr<Log> ip_log): mp_log(ip_log)
    {
    material = "matte";
    reverseOrientation = false;
    }

  intrusive_ptr<const Material> GraphicsState::CreateMaterial(const ParamSet &i_params) const
    {
    TextureParams mp(i_params, materialParams, floatTextures, spectrumTextures);
    PbrtImport::MaterialFactory material_factory(m_material_to_bump_map, mp_log);

    intrusive_ptr<const Material> mtl;
    if (currentNamedMaterial != "" && namedMaterials.find(currentNamedMaterial) != namedMaterials.end())
      mtl = namedMaterials[currentNamedMaterial];

    if (!mtl)
      mtl = material_factory.CreateMaterial(material, mp, *this);

    if (!mtl)
      mtl = material_factory.CreateMaterial("matte", mp, *this);

    if (!mtl)
      PbrtImport::Utils::LogError(mp_log, "Unable to create \"matte\" material?!");

    return mtl;
    }

  };