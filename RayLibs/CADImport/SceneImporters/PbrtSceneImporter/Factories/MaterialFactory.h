#ifndef PBRT_MATERIAL_FACTORY
#define PBRT_MATERIAL_FACTORY

#include <Common/Common.h>
#include "../PbrtParamSet.h"
#include <Raytracer/Materials/MatteMaterial.h>
#include <Raytracer/Materials/MERLMeasuredMaterial.h>
#include <Raytracer/Materials/MetalMaterial.h>
#include <Raytracer/Materials/MixMaterial.h>
#include <Raytracer/Materials/PlasticMaterial.h>
#include <Raytracer/Materials/SubstrateMaterial.h>
#include <Raytracer/Materials/TransparentMaterial.h>
#include <Raytracer/Materials/UberMaterial.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Textures/ScaleTexture.h>
#include <boost/algorithm/string.hpp>
#include "../PbrtUtils.h"
#include <fstream>

namespace PbrtImport
  {

  class MaterialFactory
    {
    public:
      MaterialFactory(std::map<const Material *, intrusive_ptr<const Texture<double>>> &i_material_to_bump_map,
        intrusive_ptr<Log> ip_log): m_material_to_bump_map(i_material_to_bump_map), mp_log(ip_log) {}
      
      intrusive_ptr<const Material> CreateMaterial(const std::string &i_name, const TextureParams &i_mp, const GraphicsState &i_graphics_state) const
        {
        intrusive_ptr<const Material> p_material;

        if (i_name == "matte")
          p_material = _CreateMatteMaterial(i_mp);
        else if (i_name == "plastic")
          p_material = _CreatePlasticMaterial(i_mp);
        else if (i_name == "translucent")
          p_material = _CreateTranslucentMaterial(i_mp);
        else if (i_name == "glass")
          p_material = _CreateTransparentMaterial(i_mp);
        else if (i_name == "mirror")
          p_material = _CreateMirrorMaterial(i_mp);
        else if (i_name == "mix")
          {
          std::string m1 = i_mp.FindString("namedmaterial1", "");
          std::string m2 = i_mp.FindString("namedmaterial2", "");
          intrusive_ptr<const Material> p_mat1 = i_graphics_state.namedMaterials[m1];
          intrusive_ptr<const Material> p_mat2 = i_graphics_state.namedMaterials[m2];
          if (!p_mat1)
            {
            PbrtImport::Utils::LogError(mp_log, std::string("Named material \"") + m1 + std::string(" undefined.  Using \"matte\""));
            p_mat1 = CreateMaterial("matte", i_mp, i_graphics_state);
            }
          if (!p_mat2)
            {
            PbrtImport::Utils::LogError(mp_log, std::string("Named material \"") + m2 + std::string(" undefined.  Using \"matte\""));
            p_mat2 = CreateMaterial("matte", i_mp, i_graphics_state);
            }

          p_material = _CreateMixMaterial(i_mp, p_mat1, p_mat2);
          }
        else if (i_name == "metal")
          p_material = _CreateMetalMaterial(i_mp);
        else if (i_name == "substrate")
          p_material = _CreateSubstrateMaterial(i_mp);
        else if (i_name == "uber")
          p_material = _CreateUberMaterial(i_mp);
        else if (i_name == "subsurface")
          p_material = _CreateSubsurfaceMaterial(i_mp);
        else if (i_name == "kdsubsurface")
          p_material = _CreateKdSubsurfaceMaterial(i_mp);
        else if (i_name == "measured")
          p_material = _CreateMeasuredMaterial(i_mp);
        else
          PbrtImport::Utils::LogError(mp_log, std::string("Material \"") + i_name + std::string("\" unknown."));

        if (!p_material)
          PbrtImport::Utils::LogError(mp_log, std::string("Unable to create material \"") + i_name + std::string("\"."));

        return p_material;
        }

    private:
      void _AddBumpMap(intrusive_ptr<const Material> ip_ret, intrusive_ptr<const Texture<double>> ip_bumpMap)const
        {
        if (ip_bumpMap == NULL) return;
        const Texture<double> *p_bump_map = ip_bumpMap.get();
        const ConstantTexture<double> *p_constant_bump_map = dynamic_cast<const ConstantTexture<double>*>(p_bump_map);
        if (p_constant_bump_map == NULL)
          m_material_to_bump_map[ip_ret.get()] = ip_bumpMap;
        }

      intrusive_ptr<const Material> _CreateMatteMaterial(const TextureParams &i_params) const
        {
        intrusive_ptr<const Texture<SpectrumCoef_d>> Kd = i_params.GetSpectrumCoefTexture("Kd", SpectrumCoef_d(0.5), mp_log);
        intrusive_ptr<const Texture<double>> sigma = i_params.GetFloatTexture("sigma", 0.0, mp_log);
        intrusive_ptr<const Texture<double>> bumpMap = i_params.GetFloatTexture("bumpmap", 0.0, mp_log);

        intrusive_ptr<const Material> p_ret(new MatteMaterial(Kd, sigma));
        _AddBumpMap(p_ret, bumpMap);
        return p_ret;
        }

      intrusive_ptr<const Material> _CreatePlasticMaterial(const TextureParams &i_params) const
        {
        intrusive_ptr<const Texture<SpectrumCoef_d>> Kd = i_params.GetSpectrumCoefTexture("Kd", SpectrumCoef_d(0.25), mp_log);
        intrusive_ptr<const Texture<SpectrumCoef_d>> Ks = i_params.GetSpectrumCoefTexture("Ks", SpectrumCoef_d(0.25), mp_log);
        intrusive_ptr<const Texture<double>> roughness = i_params.GetFloatTexture("roughness", 0.1f, mp_log);
        intrusive_ptr<const Texture<double>> bumpMap = i_params.GetFloatTexture("bumpmap", 0.0f, mp_log);

        intrusive_ptr<const Material> p_ret(new PlasticMaterial(Kd, Ks, roughness));
        _AddBumpMap(p_ret, bumpMap);
        return p_ret;
        }

      intrusive_ptr<const Material> _CreateTranslucentMaterial(const TextureParams &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Translucent material is not supported");
        return NULL;
        }

      intrusive_ptr<const Material> _CreateTransparentMaterial(const TextureParams &i_params) const
        {
        intrusive_ptr<const Texture<SpectrumCoef_d>> Kr = i_params.GetSpectrumCoefTexture("Kr", SpectrumCoef_d(1.0), mp_log);
        intrusive_ptr<const Texture<SpectrumCoef_d>> Kt = i_params.GetSpectrumCoefTexture("Kt", SpectrumCoef_d(1.0), mp_log);
        intrusive_ptr<const Texture<double>> index = i_params.GetFloatTexture("index", 1.5, mp_log);
        intrusive_ptr<const Texture<double>> bumpMap = i_params.GetFloatTexture("bumpmap", 0.0, mp_log);
        
        double refractive_index = 1.5;
        if (dynamic_cast<const ConstantTexture<float> *>(index.get()) != NULL)
          refractive_index = index->Evaluate(DifferentialGeometry(), 0);

        intrusive_ptr<const Material> p_ret(new TransparentMaterial(Kr, Kt, refractive_index));
        _AddBumpMap(p_ret, bumpMap);
        return p_ret;
        }

      intrusive_ptr<const Material> _CreateMirrorMaterial(const TextureParams &i_params) const
        {
        intrusive_ptr<const Texture<SpectrumCoef_d>> Kr = i_params.GetSpectrumCoefTexture("Kr", SpectrumCoef_d(1.0), mp_log);
        intrusive_ptr<const Texture<double>> bumpMap = i_params.GetFloatTexture("bumpmap", 0.0, mp_log);

        intrusive_ptr<const Texture<SpectrumCoef_d>> Kt(new ConstantTexture<SpectrumCoef_d>(SpectrumCoef_d(0.0)));
        intrusive_ptr<const Material> p_ret(new TransparentMaterial(Kr, Kt, 1e10));
        _AddBumpMap(p_ret, bumpMap);
        return p_ret;
        }

      intrusive_ptr<const Material> _CreateMixMaterial(const TextureParams &i_params, intrusive_ptr<const Material> ip_mat1, intrusive_ptr<const Material> ip_mat2) const
        {
        intrusive_ptr<const Texture<SpectrumCoef_d>> scale = i_params.GetSpectrumCoefTexture("amount", SpectrumCoef_d(0.5), mp_log);

        intrusive_ptr<const Material> p_ret(new MixMaterial(ip_mat1, ip_mat2, scale));
        intrusive_ptr<const Texture<double>> bumpMap1 = m_material_to_bump_map[ip_mat1.get()];
        intrusive_ptr<const Texture<double>> bumpMap2 = m_material_to_bump_map[ip_mat1.get()];
        intrusive_ptr<const Texture<double>> bumpMap;
        if (bumpMap1!=NULL && bumpMap2==NULL) bumpMap = bumpMap1;
        if (bumpMap2!=NULL && bumpMap1==NULL) bumpMap = bumpMap2;
        if (bumpMap1!=NULL && bumpMap2!=NULL)
          bumpMap = new ScaleTexture<double, double>(bumpMap1, bumpMap2);

        _AddBumpMap(p_ret, bumpMap);
        return p_ret;
        }

      intrusive_ptr<const Material> _CreateMetalMaterial(const TextureParams &i_params) const
        {
        SpectrumCoef_d copperN(1.165312, 0.950375, 0.351250);
        intrusive_ptr<const Texture<SpectrumCoef_d>> eta = i_params.GetSpectrumCoefTexture("eta", copperN, mp_log);

        SpectrumCoef_d copperK(2.397625, 2.576500, 3.010750);
        intrusive_ptr<const Texture<SpectrumCoef_d>> k = i_params.GetSpectrumCoefTexture("k", copperK, mp_log);

        intrusive_ptr<const Texture<double>> roughness = i_params.GetFloatTexture("roughness", 0.1f, mp_log);
        intrusive_ptr<const Texture<double>> bumpMap = i_params.GetFloatTexture("bumpmap", 0.0f, mp_log);

        intrusive_ptr<const Material> p_ret(new MetalMaterial(eta, k, roughness));
        if (bumpMap) m_material_to_bump_map[p_ret.get()] = bumpMap;
        _AddBumpMap(p_ret, bumpMap);
        return p_ret;
        }

      intrusive_ptr<const Material> _CreateSubstrateMaterial(const TextureParams &i_params) const
        {
        intrusive_ptr<const Texture<SpectrumCoef_d>> Kd = i_params.GetSpectrumCoefTexture("Kd", SpectrumCoef_d(0.5), mp_log);
        intrusive_ptr<const Texture<SpectrumCoef_d>> Ks = i_params.GetSpectrumCoefTexture("Ks", SpectrumCoef_d(0.5), mp_log);

        intrusive_ptr<const Texture<double>> uroughness = i_params.GetFloatTexture("uroughness", 0.1f, mp_log);
        intrusive_ptr<const Texture<double>> vroughness = i_params.GetFloatTexture("vroughness", 0.1f, mp_log);
        intrusive_ptr<const Texture<double>> bumpMap = i_params.GetFloatTexture("bumpmap", 0.0f, mp_log);

        intrusive_ptr<const Material> p_ret(new SubstrateMaterial(Kd, Ks, uroughness, vroughness));
        _AddBumpMap(p_ret, bumpMap);
        return p_ret;
        }

      intrusive_ptr<const Material> _CreateUberMaterial(const TextureParams &i_params) const
        {
        intrusive_ptr<const Texture<SpectrumCoef_d>> Kd = i_params.GetSpectrumCoefTexture("Kd", SpectrumCoef_d(0.25), mp_log);
        intrusive_ptr<const Texture<SpectrumCoef_d>> Ks = i_params.GetSpectrumCoefTexture("Ks", SpectrumCoef_d(0.25), mp_log);
        intrusive_ptr<const Texture<SpectrumCoef_d>> Kr = i_params.GetSpectrumCoefTexture("Kr", SpectrumCoef_d(0.0), mp_log);
        intrusive_ptr<const Texture<double>> roughness = i_params.GetFloatTexture("roughness", 0.1f, mp_log);
        intrusive_ptr<const Texture<double>> eta = i_params.GetFloatTexture("index", 1.5f, mp_log);
        intrusive_ptr<const Texture<SpectrumCoef_d>> opacity = i_params.GetSpectrumCoefTexture("opacity", SpectrumCoef_d(1.0), mp_log);
        intrusive_ptr<const Texture<double>> bumpMap = i_params.GetFloatTexture("bumpmap", 0.f, mp_log);

        double refractive_index = 1.5;
        if (dynamic_cast<const ConstantTexture<float> *>(eta.get()) != NULL)
          refractive_index = eta->Evaluate(DifferentialGeometry(), 0);

        intrusive_ptr<const Material> p_ret(new UberMaterial(Kd, Ks, Kr, roughness, opacity, refractive_index));
        _AddBumpMap(p_ret, bumpMap);
        return p_ret;
        }

      intrusive_ptr<const Material> _CreateSubsurfaceMaterial(const TextureParams &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "Subsurface material is not supported");
        return NULL;
        }

      intrusive_ptr<const Material> _CreateKdSubsurfaceMaterial(const TextureParams &i_params) const
        {
        PbrtImport::Utils::LogError(mp_log, "KdSubsurface material is not supported");
        return NULL;
        }

      intrusive_ptr<const Material> _CreateMeasuredMaterial(const TextureParams &i_params) const
        {
        intrusive_ptr<const Texture<double>> bumpMap = i_params.GetFloatTexture("bumpmap", 0.f, mp_log);
        std::string filename = i_params.FindFilename("filename");
        if (boost::iends_with(filename, "binary")==false)
          {
          PbrtImport::Utils::LogError(mp_log, "Unknown file format for the measured material.");
          return NULL;
          }

        std::ifstream data_file(filename.c_str(), std::ios::in | std::ios::binary);
        if (data_file.eof() || data_file.fail())
          {
          PbrtImport::Utils::LogError(mp_log, "MERL data file does not exist or empty.");
          return NULL;
          }

        intrusive_ptr<MERLMeasuredData> p_merl_measured_data( new MERLMeasuredData(data_file) );
        intrusive_ptr<const Material> p_ret(new MERLMeasuredMaterial(p_merl_measured_data));
        _AddBumpMap(p_ret, bumpMap);
        return p_ret;
        }

    private:
      intrusive_ptr<Log> mp_log;

      std::map<const Material *, intrusive_ptr<const Texture<double>>> &m_material_to_bump_map;
    };

  };

#endif // PBRT_MATERIAL_FACTORY
