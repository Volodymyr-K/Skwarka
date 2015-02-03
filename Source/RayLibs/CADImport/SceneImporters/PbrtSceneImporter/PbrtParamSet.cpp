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

#include "PbrtParamSet.h"
#include "PbrtUtils.h"
#include <Raytracer/Core/Color.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <Raytracer/Textures/ConstantTexture.h>

namespace PbrtImport
  {

  // ParamSet Macros
  #define ERASE_PARAM(vec) \
      for (size_t i = 0; i < vec.size(); ++i) \
      if (vec[i]->name == n) \
        { \
        vec.erase(vec.begin() + i); \
        return true; \
        } \
    return false

  #define LOOKUP_PTR(vec) \
      for (size_t i = 0; i < (vec).size(); ++i) \
          if ((vec)[i]->name == name) { \
              *nItems = (vec)[i]->nItems; \
              return (vec)[i]->data; \
          } \
      return NULL

  #define LOOKUP_ONE(vec) \
      for (size_t i = 0; i < (vec).size(); ++i) { \
          if ((vec)[i]->name == name && \
              (vec)[i]->nItems == 1) { \
              return *((vec)[i]->data); \
  }        } \
      return d

  // ParamSet Methods
  void ParamSet::AddFloat(const std::string &name, const float *data, size_t nItems)
    {
    EraseFloat(name);
    floats.push_back(new ParamSetItem<float>(name, data, nItems));
    }

  void ParamSet::AddInt(const std::string &name, const int *data, size_t nItems)
    {
    EraseInt(name);
    ints.push_back(new ParamSetItem<int>(name, data, nItems));
    }

  void ParamSet::AddBool(const std::string &name, const bool *data, size_t nItems)
    {
    EraseBool(name);
    bools.push_back(new ParamSetItem<bool>(name, data, nItems));
    }

  void ParamSet::AddPoint(const std::string &name, const Point3D_d *data, size_t nItems)
    {
    ErasePoint(name);
    points.push_back(new ParamSetItem<Point3D_d>(name, data, nItems));
    }

  void ParamSet::AddVector(const std::string &name, const Vector3D_d *data, size_t nItems)
    {
    EraseVector(name);
    vectors.push_back(new ParamSetItem<Vector3D_d>(name, data, nItems));
    }

  void ParamSet::AddNormal(const std::string &name, const Vector3D_d *data, size_t nItems)
    {
    EraseNormal(name);
    normals.push_back(new ParamSetItem<Vector3D_d>(name, data, nItems));
    }

  void ParamSet::AddString(const std::string &name, const std::string *data, size_t nItems)
    {
    EraseString(name);
    strings.push_back(new ParamSetItem<std::string>(name, data, nItems));
    }

  void ParamSet::AddTexture(const std::string &name, const std::string &value)
    {
    EraseTexture(name);
    textures.push_back(new ParamSetItem<std::string>(name, (std::string *)&value, 1));
    }

  void ParamSet::AddRGBSpectrum(const std::string &name, const float *data, size_t nItems)
    {
    EraseSpectrum(name);
    ASSERT(nItems % 3 == 0);
    nItems /= 3;
    Spectrum_d *s = new Spectrum_d[nItems];
    for (int i = 0; i < nItems; ++i)
      s[i] = SpectrumRoutines::XYZToSpectrum(global_sRGB_D65_ColorSystem.RGB_To_XYZ(RGBColor_d(data[3*i], data[3*i+1], data[3*i+2])));
    spectrums.push_back(new ParamSetItem<Spectrum_d>(name, s, nItems));
    delete[] s;
    }

  void ParamSet::AddRGBSpectrumCoef(const std::string &name, const float *data, size_t nItems)
    {
    EraseSpectrumCoef(name);
    ASSERT(nItems % 3 == 0);
    nItems /= 3;
    SpectrumCoef_d *s = new SpectrumCoef_d[nItems];
    for (int i = 0; i < nItems; ++i)
      {
      float r = data[3*i], g = data[3*i+1], b = data[3*i+2];

      // If all RGB components are the same we treat this as a special case to avoid round-off errors.
      if (r==g && r==b)
        s[i] = SpectrumCoef_d(r);
      else
        s[i] = SpectrumRoutines::XYZToSpectrumCoef(global_sRGB_E_ColorSystem.RGB_To_XYZ(RGBColor_d(r,g,b)));
      }
    spectrum_coefs.push_back(new ParamSetItem<SpectrumCoef_d>(name, s, nItems));
    delete[] s;
    }

  void ParamSet::AddXYZSpectrum(const std::string &name, const float *data, size_t nItems)
    {
    EraseSpectrum(name);
    ASSERT(nItems % 3 == 0);
    nItems /= 3;
    Spectrum_d *s = new Spectrum_d[nItems];
    for (int i = 0; i < nItems; ++i)
      s[i] = SpectrumRoutines::XYZToSpectrum(XYZColor_d(data[3*i], data[3*i+1], data[3*i+2]));
    spectrums.push_back(new ParamSetItem<Spectrum_d>(name, s, nItems));
    delete[] s;
    }

  void ParamSet::AddXYZSpectrumCoef(const std::string &name, const float *data, size_t nItems)
    {
    EraseSpectrumCoef(name);
    ASSERT(nItems % 3 == 0);
    nItems /= 3;
    SpectrumCoef_d *s = new SpectrumCoef_d[nItems];
    for (int i = 0; i < nItems; ++i)
      s[i] = SpectrumRoutines::XYZToSpectrumCoef(XYZColor_d(data[3*i], data[3*i+1], data[3*i+2]));
    spectrum_coefs.push_back(new ParamSetItem<SpectrumCoef_d>(name, s, nItems));
    delete[] s;
    }

  void ParamSet::AddSampledSpectrum(const std::string &name, const float *data, size_t nItems)
    {
    EraseSpectrum(name);
    ASSERT(nItems % 2 == 0);
    nItems /= 2;
    float *wl = new float[nItems], *v = new float[nItems];
    std::vector<std::pair<double, double>> samples(nItems);
    for (int i = 0; i < nItems; ++i)
      {
      samples[i].first  = data[2*i];
      samples[i].second = data[2*i+1]; 
      }
    Spectrum_d s = SpectrumRoutines::SampledToSpectrum(samples);
    spectrums.push_back(new ParamSetItem<Spectrum_d>(name, &s, 1));
    }

  void ParamSet::AddSampledSpectrumCoef(const std::string &name, const float *data, size_t nItems)
    {
    EraseSpectrumCoef(name);
    ASSERT(nItems % 2 == 0);
    nItems /= 2;
    float *wl = new float[nItems], *v = new float[nItems];
    std::vector<std::pair<double, double>> samples(nItems);
    for (int i = 0; i < nItems; ++i)
      {
      samples[i].first  = data[2*i];
      samples[i].second = data[2*i+1]; 
      }
    SpectrumCoef_d s = SpectrumRoutines::SampledToSpectrumCoef(samples);
    spectrum_coefs.push_back(new ParamSetItem<SpectrumCoef_d>(name, &s, 1));
    }

  void ParamSet::AddSampledSpectrumFiles(const std::string &name, const std::string *names, size_t nItems, intrusive_ptr<Log> ip_log)
    {
    EraseSpectrum(name);
    Spectrum_d *s = new Spectrum_d[nItems];
    for (int i = 0; i < nItems; ++i)
      {
      std::string full_name = _GetFullPath(names[i]);

      if (cachedSpectrums.find(full_name) != cachedSpectrums.end())
        {
        s[i] = cachedSpectrums[full_name];
        continue;
        }

      std::vector<float> vals;
      if (!Utils::ReadFloatFile(full_name, &vals, ip_log))
        {
        if (ip_log)
          ip_log->LogMessage(Log::WARNING_LEVEL, std::string("Unable to read SPD file \"") + full_name + std::string("\".  Using black distribution."));
        s[i] = Spectrum_d(0.);
        }
      else
        {
        if ((vals.size() % 2) && ip_log)
          ip_log->LogMessage(Log::WARNING_LEVEL, std::string("Extra value found in spectrum file \"") + full_name + std::string("\".  Ignoring it."));

        std::vector<std::pair<double, double>> samples(vals.size()/2);
        for (size_t j = 0; j < vals.size()/2; ++j)
          {
          samples[j].first  = vals[2*j];
          samples[j].second = vals[2*j+1]; 
          }
        s[i] = SpectrumRoutines::SampledToSpectrum(samples);
        }
      cachedSpectrums[full_name] = s[i];
      }

    spectrums.push_back(new ParamSetItem<Spectrum_d>(name, s, nItems));
    delete[] s;
    }

  void ParamSet::AddSampledSpectrumCoefFiles(const std::string &name, const std::string *names, size_t nItems, intrusive_ptr<Log> ip_log)
    {
    EraseSpectrumCoef(name);
    SpectrumCoef_d *s = new SpectrumCoef_d[nItems];
    for (int i = 0; i < nItems; ++i)
      {
      std::string full_name = _GetFullPath(names[i]);

      if (cachedSpectrumCoefs.find(full_name) != cachedSpectrumCoefs.end())
        {
        s[i] = cachedSpectrumCoefs[full_name];
        continue;
        }

      std::vector<float> vals;
      if (!Utils::ReadFloatFile(full_name, &vals, ip_log))
        {
        if (ip_log)
          ip_log->LogMessage(Log::WARNING_LEVEL, std::string("Unable to read SPD file \"") + full_name + std::string("\".  Using black distribution."));
        s[i] = SpectrumCoef_d(0.);
        }
      else
        {
        if ((vals.size() % 2) && ip_log)
          ip_log->LogMessage(Log::WARNING_LEVEL, std::string("Extra value found in spectrum file \"") + full_name + std::string("\".  Ignoring it."));

        std::vector<std::pair<double, double>> samples(vals.size()/2);
        for (size_t j = 0; j < vals.size()/2; ++j)
          {
          samples[j].first  = vals[2*j];
          samples[j].second = vals[2*j+1]; 
          }
        s[i] = SpectrumRoutines::SampledToSpectrumCoef(samples);
        }
      cachedSpectrumCoefs[full_name] = s[i];
      }

    spectrum_coefs.push_back(new ParamSetItem<SpectrumCoef_d>(name, s, nItems));
    delete[] s;
    }

  std::map<std::string , Spectrum_d> ParamSet::cachedSpectrums;
  std::map<std::string , SpectrumCoef_d> ParamSet::cachedSpectrumCoefs;

  bool ParamSet::EraseInt(const std::string &n)
    {
    ERASE_PARAM(ints);
    }

  bool ParamSet::EraseBool(const std::string &n)
    {
    ERASE_PARAM(bools);
    }

  bool ParamSet::EraseFloat(const std::string &n)
    {
    ERASE_PARAM(floats);
    }

  bool ParamSet::ErasePoint(const std::string &n)
    {
    ERASE_PARAM(points);
    }

  bool ParamSet::EraseVector(const std::string &n)
    {
    ERASE_PARAM(vectors);
    }

  bool ParamSet::EraseNormal(const std::string &n)
    {
    ERASE_PARAM(normals);
    }

  bool ParamSet::EraseSpectrum(const std::string &n)
    {
    ERASE_PARAM(spectrums);
    }

  bool ParamSet::EraseSpectrumCoef(const std::string &n)
    {
    ERASE_PARAM(spectrum_coefs);
    }

  bool ParamSet::EraseString(const std::string &n)
    {
    ERASE_PARAM(strings);
    }

  bool ParamSet::EraseTexture(const std::string &n)
    {
    ERASE_PARAM(textures);
    }

  float ParamSet::FindOneFloat(const std::string &name, float d) const
    {
    LOOKUP_ONE(floats);
    }

  const float *ParamSet::FindFloat(const std::string &name, size_t *nItems) const
    {
    LOOKUP_PTR(floats);
    }

  const int *ParamSet::FindInt(const std::string &name, size_t *nItems) const
    {
    LOOKUP_PTR(ints);
    }

  const bool *ParamSet::FindBool(const std::string &name, size_t *nItems) const
    {
    LOOKUP_PTR(bools);
    }

  int ParamSet::FindOneInt(const std::string &name, int d) const
    {
    LOOKUP_ONE(ints);
    }

  bool ParamSet::FindOneBool(const std::string &name, bool d) const
    {
    LOOKUP_ONE(bools);
    }

  const Point3D_d *ParamSet::FindPoint(const std::string &name, size_t *nItems) const
    {
    LOOKUP_PTR(points);
    }

  Point3D_d ParamSet::FindOnePoint(const std::string &name, const Point3D_d &d) const
    {
    LOOKUP_ONE(points);
    }

  const Vector3D_d *ParamSet::FindVector(const std::string &name, size_t *nItems) const
    {
    LOOKUP_PTR(vectors);
    }

  Vector3D_d ParamSet::FindOneVector(const std::string &name, const Vector3D_d &d) const
    {
    LOOKUP_ONE(vectors);
    }

  const Vector3D_d *ParamSet::FindNormal(const std::string &name, size_t *nItems) const
    {
    LOOKUP_PTR(normals);
    }

  Vector3D_d ParamSet::FindOneNormal(const std::string &name, const Vector3D_d &d) const
    {
    LOOKUP_ONE(normals);
    }

  const Spectrum_d *ParamSet::FindSpectrum(const std::string &name, size_t *nItems) const
    {
    LOOKUP_PTR(spectrums);
    }

  Spectrum_d ParamSet::FindOneSpectrum(const std::string &name, const Spectrum_d &d) const
    {
    LOOKUP_ONE(spectrums);
    }

  const SpectrumCoef_d *ParamSet::FindSpectrumCoef(const std::string &name, size_t *nItems) const
    {
    LOOKUP_PTR(spectrum_coefs);
    }

  SpectrumCoef_d ParamSet::FindOneSpectrumCoef(const std::string &name, const SpectrumCoef_d &d) const
    {
    LOOKUP_ONE(spectrum_coefs);
    }

  const std::string *ParamSet::FindString(const std::string &name, size_t *nItems) const
    {
    LOOKUP_PTR(strings);
    }

  std::string ParamSet::FindOneString(const std::string &name, const std::string &d) const
    {
    LOOKUP_ONE(strings);
    }

  std::string ParamSet::FindOneFilename(const std::string &name, const std::string &d) const
    {
    std::string filename = FindOneString(name, "");
    if (filename == "")
      return d;

    return _GetFullPath(filename);
    }

  std::string ParamSet::_GetFullPath(const std::string &i_path) const
    {
    if (Utils::IsAbsolutePath(i_path))
      return i_path;

    return m_base_path + "/" + i_path;
    }

  std::string ParamSet::FindTexture(const std::string &name) const
    {
    std::string d = "";
    LOOKUP_ONE(textures);
    }

  void ParamSet::Clear()
    {
  #define DEL_PARAMS(name) \
    (name).erase((name).begin(), (name).end())
    DEL_PARAMS(ints);    DEL_PARAMS(bools);
    DEL_PARAMS(floats);  DEL_PARAMS(points);
    DEL_PARAMS(vectors); DEL_PARAMS(normals);
    DEL_PARAMS(spectrums); DEL_PARAMS(spectrum_coefs);
    DEL_PARAMS(strings); DEL_PARAMS(textures);
  #undef DEL_PARAMS
    }

  // TextureParams Method Definitions
  intrusive_ptr<const Texture<SpectrumCoef_d>> TextureParams::GetSpectrumCoefTexture(const std::string &n, const SpectrumCoef_d &def, intrusive_ptr<Log> ip_log) const
    {
    std::string name = geomParams.FindTexture(n);
    if (name == "") name = materialParams.FindTexture(n);
    if (name != "")
      {
      if (spectrumTextures.find(name) != spectrumTextures.end())
        return spectrumTextures.find(name)->second;
      else
        {
        std::string message("Couldn't find spectrum texture named \"");
        message += name;message += std::string("\" for parameter \""); message += n; message += std::string("\"");
        PbrtImport::Utils::LogError(ip_log, message);
        }
      }

    SpectrumCoef_d val = geomParams.FindOneSpectrumCoef(n, materialParams.FindOneSpectrumCoef(n, def));
    return new ConstantTexture<SpectrumCoef_d>(val);
    }

  intrusive_ptr<const Texture<double>> TextureParams::GetFloatTexture(const std::string &n, float def, intrusive_ptr<Log> ip_log) const
    {
    std::string name = geomParams.FindTexture(n);
    if (name == "") name = materialParams.FindTexture(n);
    if (name != "")
      {
      if (floatTextures.find(name) != floatTextures.end())
        return floatTextures.find(name)->second;
      else
        {
        std::string message("Couldn't find float texture named \"");
        message += name;message += std::string("\" for parameter \""); message += n; message += std::string("\"");
        PbrtImport::Utils::LogError(ip_log, message);
        }
      }

    float val = geomParams.FindOneFloat(n, materialParams.FindOneFloat(n, def));
    return new ConstantTexture<double>(val);
    }

  };