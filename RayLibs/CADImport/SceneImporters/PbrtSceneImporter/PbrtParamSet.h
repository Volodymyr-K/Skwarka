#ifndef PBRT_PARAM_SET_H
#define PBRT_PARAM_SET_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/Texture.h>
#include <map>
#include <vector>
#include <string>

namespace PbrtImport
  {

  ////////////////////////////////////////// ParamSetItem ///////////////////////////////////////////////

  template <typename T>
  struct ParamSetItem : public ReferenceCounted
    {
    ParamSetItem(const std::string &name, const T *val, int nItems = 1);

    ~ParamSetItem()
      {
      delete[] data;
      }

    std::string name;
    int nItems;
    T *data;
    };

  template <typename T>
  ParamSetItem<T>::ParamSetItem(const std::string &n, const T *v, int ni)
    {
    name = n;
    nItems = ni;
    data = new T[nItems];
    for (int i = 0; i < nItems; ++i) data[i] = v[i];
    }

  //////////////////////////////////////////// ParamSet /////////////////////////////////////////////////

  class ParamSet
    {
    public:
      ParamSet(const std::string &i_base_path = ""): m_base_path(i_base_path) { }

      void AddFloat(const std::string &, const float *, int nItems = 1);
      void AddInt(const std::string &, const int *, int nItems);
      void AddBool(const std::string &, const bool *, int nItems);
      void AddPoint(const std::string &, const Point3D_d *, int nItems);
      void AddVector(const std::string &, const Vector3D_d *, int nItems);
      void AddNormal(const std::string &, const Vector3D_d *, int nItems);
      void AddString(const std::string &, const std::string *, int nItems);
      void AddTexture(const std::string &, const std::string &);
      void AddRGBSpectrum(const std::string &, const float *, int nItems);
      void AddRGBSpectrumCoef(const std::string &, const float *, int nItems);
      void AddXYZSpectrum(const std::string &, const float *, int nItems);
      void AddXYZSpectrumCoef(const std::string &, const float *, int nItems);
      void AddSampledSpectrumFiles(const std::string &, const std::string *, int nItems, intrusive_ptr<Log> ip_log = NULL);
      void AddSampledSpectrumCoefFiles(const std::string &, const std::string *, int nItems, intrusive_ptr<Log> ip_log = NULL);
      void AddSampledSpectrum(const std::string &, const float *, int nItems);
      void AddSampledSpectrumCoef(const std::string &, const float *, int nItems);
      bool EraseInt(const std::string &);
      bool EraseBool(const std::string &);
      bool EraseFloat(const std::string &);
      bool ErasePoint(const std::string &);
      bool EraseVector(const std::string &);
      bool EraseNormal(const std::string &);
      bool EraseSpectrum(const std::string &);
      bool EraseSpectrumCoef(const std::string &);
      bool EraseString(const std::string &);
      bool EraseTexture(const std::string &);
      float FindOneFloat(const std::string &, float d) const;
      int FindOneInt(const std::string &, int d) const;
      bool FindOneBool(const std::string &, bool d) const;
      Point3D_d FindOnePoint(const std::string &, const Point3D_d &d) const;
      Vector3D_d FindOneVector(const std::string &, const Vector3D_d &d) const;
      Vector3D_d FindOneNormal(const std::string &, const Vector3D_d &d) const;
      Spectrum_d FindOneSpectrum(const std::string &, const Spectrum_d &d) const;
      SpectrumCoef_d FindOneSpectrumCoef(const std::string &, const SpectrumCoef_d &d) const;
      std::string FindOneString(const std::string &, const std::string &d) const;
      std::string FindOneFilename(const std::string &, const std::string &d) const;
      std::string FindTexture(const std::string &) const;
      const float *FindFloat(const std::string &, int *nItems) const;
      const int *FindInt(const std::string &, int *nItems) const;
      const bool *FindBool(const std::string &, int *nItems) const;
      const Point3D_d *FindPoint(const std::string &, int *nItems) const;
      const Vector3D_d *FindVector(const std::string &, int *nItems) const;
      const Vector3D_d *FindNormal(const std::string &, int *nItems) const;
      const Spectrum_d *FindSpectrum(const std::string &, int *nItems) const;
      const SpectrumCoef_d *FindSpectrumCoef(const std::string &, int *nItems) const;
      const std::string *FindString(const std::string &, int *nItems) const;
      void Clear();

    private:
        std::string _GetFullPath(const std::string &i_path) const;

    private:
      std::string m_base_path;

      std::vector<intrusive_ptr<ParamSetItem<bool>>> bools;
      std::vector<intrusive_ptr<ParamSetItem<int>>> ints;
      std::vector<intrusive_ptr<ParamSetItem<float>>> floats;
      std::vector<intrusive_ptr<ParamSetItem<Point3D_d>>> points;
      std::vector<intrusive_ptr<ParamSetItem<Vector3D_d>>> vectors;
      std::vector<intrusive_ptr<ParamSetItem<Vector3D_d>>> normals;
      std::vector<intrusive_ptr<ParamSetItem<SpectrumCoef_d>>> spectrum_coefs;
      std::vector<intrusive_ptr<ParamSetItem<Spectrum_d>>> spectrums;
      std::vector<intrusive_ptr<ParamSetItem<std::string>>> strings;
      std::vector<intrusive_ptr<ParamSetItem<std::string>>> textures;
      static std::map<std::string, Spectrum_d> cachedSpectrums;
      static std::map<std::string, SpectrumCoef_d> cachedSpectrumCoefs;
    };

  /////////////////////////////////////////// TextureParams ///////////////////////////////////////////////

  class TextureParams
    {
    public:

      TextureParams(const ParamSet &geomp, const ParamSet &matp,
        const std::map<std::string, intrusive_ptr<const Texture<double>>> &ft,
        const std::map<std::string, intrusive_ptr<const Texture<SpectrumCoef_d>>> &st):
      floatTextures(ft), spectrumTextures(st), geomParams(geomp), materialParams(matp)
        {
        }

      intrusive_ptr<const Texture<SpectrumCoef_d>> GetSpectrumCoefTexture(const std::string &name, const SpectrumCoef_d &def, intrusive_ptr<Log> ip_log) const;

      intrusive_ptr<const Texture<double>> GetFloatTexture(const std::string &name, float def, intrusive_ptr<Log> ip_log) const;

      float FindFloat(const std::string &n, float d) const
        {
        return geomParams.FindOneFloat(n, materialParams.FindOneFloat(n, d));
        }

      std::string FindString(const std::string &n, const std::string &d = "") const
        {
        return geomParams.FindOneString(n, materialParams.FindOneString(n, d));
        }

      std::string FindFilename(const std::string &n, const std::string &d = "") const
        {
        return geomParams.FindOneFilename(n, materialParams.FindOneFilename(n, d));
        }

      int FindInt(const std::string &n, int d) const
        {
        return geomParams.FindOneInt(n, materialParams.FindOneInt(n, d));
        }

      bool FindBool(const std::string &n, bool d) const
        {
        return geomParams.FindOneBool(n, materialParams.FindOneBool(n, d));
        }

      Point3D_d FindPoint(const std::string &n, const Point3D_d &d) const
        {
        return geomParams.FindOnePoint(n, materialParams.FindOnePoint(n, d));
        }

      Vector3D_d FindVector(const std::string &n, const Vector3D_d &d) const
        {
        return geomParams.FindOneVector(n, materialParams.FindOneVector(n, d));
        }

      Vector3D_d FindNormal(const std::string &n, const Vector3D_d &d) const
        {
        return geomParams.FindOneNormal(n, materialParams.FindOneNormal(n, d));
        }

      Spectrum_d FindSpectrum(const std::string &n, const Spectrum_d &d) const
        {
        return geomParams.FindOneSpectrum(n, materialParams.FindOneSpectrum(n, d));
        }

      SpectrumCoef_d FindSpectrumCoef(const std::string &n, const SpectrumCoef_d &d) const
        {
        return geomParams.FindOneSpectrumCoef(n, materialParams.FindOneSpectrumCoef(n, d));
        }

      const ParamSet &GetGeomParams() const { return geomParams; }

      const ParamSet &GetMaterialParams() const { return materialParams; }

    private:
      const std::map<std::string, intrusive_ptr<const Texture<double>>> &floatTextures;
      const std::map<std::string, intrusive_ptr<const Texture<SpectrumCoef_d>>> &spectrumTextures;
      const ParamSet &geomParams, &materialParams;
    };

  };

#endif // PBRT_PARAM_SET_H
