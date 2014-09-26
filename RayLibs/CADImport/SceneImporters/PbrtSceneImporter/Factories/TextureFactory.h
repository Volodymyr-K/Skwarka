#ifndef PBRT_TEXTURE_FACTORY
#define PBRT_TEXTURE_FACTORY

#include <Common/Common.h>
#include "../PbrtParamSet.h"
#include <Raytracer/Mappings/SphericalMapping2D.h>
#include <Raytracer/Mappings/TransformMapping3D.h>
#include <Raytracer/Mappings/UVMapping2D.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Textures/ImageTexture.h>
#include <Raytracer/Textures/ScaleTexture.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <Raytracer/Core/MIPMap.h>
#include "../PbrtUtils.h"
#include <string>
#include <map>
#include <algorithm>

namespace PbrtImport
  {

  class TextureFactory
    {
    public:
      TextureFactory(intrusive_ptr<Log> ip_log): mp_log(ip_log) {}

      intrusive_ptr<const Texture<double> > CreateFloatTexture(const std::string &i_name, const Transform &i_tex_to_world, const PbrtImport::TextureParams &i_params) const
        {
        if (i_name == "constant")
          return _CreateConstantFloatTexture(i_tex_to_world, i_params);
        else if (i_name == "scale")
          return _CreateScaleFloatTexture(i_tex_to_world, i_params);
        else if (i_name == "mix")
          return _CreateMixFloatTexture(i_tex_to_world, i_params);
        else if (i_name == "bilerp")
          return _CreateBilerpFloatTexture(i_tex_to_world, i_params);
        else if (i_name == "imagemap")
          return _CreateImageFloatTexture(i_tex_to_world, i_params);
        else if (i_name == "uv")
          return _CreateUVFloatTexture(i_tex_to_world, i_params);
        else if (i_name == "checkerboard")
          return _CreateCheckerboardFloatTexture(i_tex_to_world, i_params);
        else if (i_name == "dots")
          return _CreateDotsFloatTexture(i_tex_to_world, i_params);
        else if (i_name == "fbm")
          return _CreateFBmFloatTexture(i_tex_to_world, i_params);
        else if (i_name == "wrinkled")
          return _CreateWrinkledFloatTexture(i_tex_to_world, i_params);
        else if (i_name == "marble")
          return _CreateMarbleFloatTexture(i_tex_to_world, i_params);
        else if (i_name == "windy")
          return _CreateWindyFloatTexture(i_tex_to_world, i_params);
        else
          PbrtImport::Utils::LogError(mp_log, std::string("Float texture \"") + i_name + std::string("\" unknown."));

        return NULL;
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > CreateSpectrumCoefTexture(const std::string &i_name, const Transform &i_tex_to_world, const PbrtImport::TextureParams &i_params) const
        {
        if (i_name == "constant")
          return _CreateConstantSpectrumCoefTexture(i_tex_to_world, i_params);
        else if (i_name == "scale")
          return _CreateScaleSpectrumCoefTexture(i_tex_to_world, i_params);
        else if (i_name == "mix")
          return _CreateMixSpectrumCoefTexture(i_tex_to_world, i_params);
        else if (i_name == "bilerp")
          return _CreateBilerpSpectrumCoefTexture(i_tex_to_world, i_params);
        else if (i_name == "imagemap")
          return _CreateImageSpectrumCoefTexture(i_tex_to_world, i_params);
        else if (i_name == "uv")
          return _CreateUVSpectrumCoefTexture(i_tex_to_world, i_params);
        else if (i_name == "checkerboard")
          return _CreateCheckerboardSpectrumCoefTexture(i_tex_to_world, i_params);
        else if (i_name == "dots")
          return _CreateDotsSpectrumCoefTexture(i_tex_to_world, i_params);
        else if (i_name == "fbm")
          return _CreateFBmSpectrumCoefTexture(i_tex_to_world, i_params);
        else if (i_name == "wrinkled")
          return _CreateWrinkledSpectrumCoefTexture(i_tex_to_world, i_params);
        else if (i_name == "marble")
          return _CreateMarbleSpectrumCoefTexture(i_tex_to_world, i_params);
        else if (i_name == "windy")
          return _CreateWindySpectrumCoefTexture(i_tex_to_world, i_params);
        else
          PbrtImport::Utils::LogError(mp_log, std::string("Spectrum texture \"") + i_name + std::string("\" unknown."));

        return NULL;
        }

    private:
      /////////////////////////////////////////// Float Textures ////////////////////////////////////////////////

      intrusive_ptr<const Texture<double> > _CreateConstantFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        return new ConstantTexture<double>(tp.FindFloat("value", 1.f));
        }

      intrusive_ptr<const Texture<double> > _CreateImageFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        intrusive_ptr<const Mapping2D> p_map;
        std::string type = tp.FindString("mapping", "uv");
        if (type == "uv")
          {
          float su = tp.FindFloat("uscale", 1.);
          float sv = tp.FindFloat("vscale", 1.);
          float du = tp.FindFloat("udelta", 0.);
          float dv = tp.FindFloat("vdelta", 0.);
          p_map.reset( new UVMapping2D(su, sv, Vector2D_d(du, dv)) );
          }
        else if (type == "spherical") p_map.reset( new SphericalMapping2D(i_tex_to_world.Inverted()) );
        else
          {
          PbrtImport::Utils::LogError(mp_log, std::string("2D texture mapping \"") + type + std::string("\" unknown or not supported. Using UV mapping texture."));
          p_map.reset(new UVMapping2D);
          }

        std::string filename = tp.FindFilename("filename");
        float maxAniso = tp.FindFloat("maxanisotropy", 8.f);
        bool trilerp = tp.FindBool("trilinear", false);
        std::string wrap = tp.FindString("wrap", "repeat");
        bool repeat = true;
        if (wrap == "black") repeat = false;
        else if (wrap == "clamp") repeat = false;
        float scale = tp.FindFloat("scale", 1.f);
        float gamma = tp.FindFloat("gamma", 1.f);

        intrusive_ptr<const MIPMap<float> > p_mip_map;
        std::string cache_key = filename+","+std::to_string(maxAniso)+","+wrap+","+std::to_string(scale)+","+std::to_string(gamma);
        if (m_float_mip_map_cache.find(cache_key) != m_float_mip_map_cache.end())
          p_mip_map = m_float_mip_map_cache[cache_key];
        else
          {
          // PBRT scales image values prior to applying the gamma correction so we need to account for that.
          scale = pow(scale, gamma);

          intrusive_ptr<const ImageSource<float> > p_image_source =
            PbrtImport::Utils::CreateImageSourceFromFile<float>(filename, true, scale, mp_log);

          if (p_image_source==NULL)
            {
            PbrtImport::Utils::LogError(mp_log, "Cannot create float image texture.");
            return NULL;
            }

          p_mip_map.reset(new MIPMap<float>(p_image_source, repeat, maxAniso));
          m_float_mip_map_cache[cache_key] = p_mip_map;
          }

        ASSERT(p_mip_map);
        return intrusive_ptr<const Texture<double> >(new ImageTexture<float, double>(p_mip_map, p_map));
        }

      intrusive_ptr<const Texture<double> > _CreateScaleFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        return new ScaleTexture<double, double>(tp.GetFloatTexture("tex1", 1.0, mp_log), tp.GetFloatTexture("tex2", 1.0, mp_log));
        }

      intrusive_ptr<const Texture<double> > _CreateMixFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Mix texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<double> > _CreateBilerpFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Bilerp texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<double> > _CreateUVFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "UV texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<double> > _CreateCheckerboardFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Checkerboard texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<double> > _CreateDotsFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Dots texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<double> > _CreateFBmFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "FBm texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<double> > _CreateWrinkledFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Wrinkled texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<double> > _CreateMarbleFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Marble texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<double> > _CreateWindyFloatTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Windy texture is not supported");
        return NULL;
        }

      ///////////////////////////////////////// Spectrum Textures //////////////////////////////////////////////

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateConstantSpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        SpectrumCoef_d spectrum = tp.FindSpectrumCoef("value", SpectrumCoef_d(1.0));
        return new ConstantTexture<SpectrumCoef_d>(spectrum);
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateImageSpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        intrusive_ptr<const Mapping2D> p_map;
        std::string type = tp.FindString("mapping", "uv");
        if (type == "uv")
          {
          float su = tp.FindFloat("uscale", 1.);
          float sv = tp.FindFloat("vscale", 1.);
          float du = tp.FindFloat("udelta", 0.);
          float dv = tp.FindFloat("vdelta", 0.);
          p_map.reset( new UVMapping2D(su, sv, Vector2D_d(du, dv)) );
          }
        else if (type == "spherical") p_map.reset( new SphericalMapping2D(i_tex_to_world.Inverted()) );
        else
          {
          PbrtImport::Utils::LogError(mp_log, std::string("2D texture mapping \"") + type + std::string("\" unknown or not supported. Using UV mapping texture."));
          p_map.reset(new UVMapping2D);
          }

        std::string filename = tp.FindFilename("filename");
        float maxAniso = tp.FindFloat("maxanisotropy", 8.f);
        bool trilerp = tp.FindBool("trilinear", false);
        std::string wrap = tp.FindString("wrap", "repeat");
        bool repeat = true;
        if (wrap == "black") repeat = false;
        else if (wrap == "clamp") repeat = false;
        float scale = tp.FindFloat("scale", 1.f);
        float gamma = tp.FindFloat("gamma", 1.f);

        intrusive_ptr<const MIPMap<SpectrumCoef_f> > p_mip_map;
        std::string cache_key = filename+","+std::to_string(maxAniso)+","+wrap+","+std::to_string(scale)+","+std::to_string(gamma);
        if (m_spectrum_mip_map_cache.find(cache_key) != m_spectrum_mip_map_cache.end())
          p_mip_map = m_spectrum_mip_map_cache[cache_key];
        else
          {
          // PBRT scales image values prior to applying the gamma correction so we need to account for that.
          scale = pow(scale, gamma);

          intrusive_ptr<const ImageSource<SpectrumCoef_f> > p_image_source =
            PbrtImport::Utils::CreateImageSourceFromFile<SpectrumCoef_f>(filename, true, scale, mp_log);

          if (p_image_source==NULL)
            {
            PbrtImport::Utils::LogError(mp_log, "Cannot create spectrum image texture.");
            return NULL;
            }

          p_mip_map.reset(new MIPMap<SpectrumCoef_f>(p_image_source, repeat, maxAniso));
          m_spectrum_mip_map_cache[cache_key] = p_mip_map;
          }

        ASSERT(p_mip_map);
        return intrusive_ptr<const Texture<SpectrumCoef_d> >(new ImageTexture<SpectrumCoef_f, SpectrumCoef_d>(p_mip_map, p_map));
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateScaleSpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        return new ScaleTexture<SpectrumCoef_d, SpectrumCoef_d>(tp.GetSpectrumCoefTexture("tex1", SpectrumCoef_d(1.0), mp_log), tp.GetSpectrumCoefTexture("tex2", SpectrumCoef_d(1.0), mp_log));
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateMixSpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Mix texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateBilerpSpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Bilerp texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateUVSpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "UV texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateCheckerboardSpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Checkerboard texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateDotsSpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Dots texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateFBmSpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "FBm texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateWrinkledSpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Wrinkled texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateMarbleSpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Marble texture is not supported");
        return NULL;
        }

      intrusive_ptr<const Texture<SpectrumCoef_d> > _CreateWindySpectrumCoefTexture(const Transform &i_tex_to_world, const TextureParams &tp) const
        {
        PbrtImport::Utils::LogError(mp_log, "Windy texture is not supported");
        return NULL;
        }

    private:
      intrusive_ptr<Log> mp_log;

      mutable std::map<std::string, intrusive_ptr<const MIPMap<SpectrumCoef_f> > > m_spectrum_mip_map_cache;
      mutable std::map<std::string, intrusive_ptr<const MIPMap<float> > > m_float_mip_map_cache;
    };

  };

#endif // PBRT_TEXTURE_FACTORY