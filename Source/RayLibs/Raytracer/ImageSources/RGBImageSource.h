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

#ifndef RGB_SPECTRUM_IMAGE_SOURCE_H
#define RGB_SPECTRUM_IMAGE_SOURCE_H

#include <Common/Common.h>
#include <Raytracer/Core/ImageSource.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <Raytracer/Core/Color.h>
#include <vector>
#include <string>
#include <cmath>

/**
* ImageSource implementation that reads image from a file and converts it to a 2D array of the specified type (see template parameter of the class).
* The class is also responsible for gamma decoding of the image values and converting the color values to the specified RGB color space.
* The template parameter is the target type of the conversion. The following target types are supported: float, double, Spectrum_f, Spectrum_d, SpectrumCoef_f, SpectrumCoef_d.
*/
template<typename T>
class RGBImageSource: public ImageSource<T>
  {
  public:
    /**
    * Creates RGBImageSource with image from the specified file, using the specified RGB color space and the scale factor.
    * The resulting image values will be multiplied by the scale factor during conversion to the target type.
    * If image loading fails, an exception will be thrown.
    * @param i_filename Name of the image file. Should not be empty and should correspond to an existing file in the filesytem.
    * @param i_color_system Color system to convert colors to the target color space.
    * @param i_scale Scale factor for the resulting image values.
    * @throws exception if image loading fails
    */
    RGBImageSource(const std::string &i_filename, const ColorSystem &i_color_system, double i_scale = 1.0);

    /**
    * Gets 2D array of values (image) of the target type.
    */
    std::vector<std::vector<T>> GetImage() const;

    /**
    * Returns height of the image, i.e. the size of the outer vector which defines the image.
    */
    size_t GetHeight() const;

    /**
    * Returns width of the image, i.e. the size of the inner vector which defines the image.
    */
    size_t GetWidth() const;

  private:
    /**
    * Helper private method that converts XYZColor to the target type.
    * Specializations for float, double, Spectrum_f, Spectrum_d, SpectrumCoef_f and SpectrumCoef_d are provided.
    * The generic template implementation is not provided and thus the class won't compile with other target types.
    */
    T _XYZ_To_T(const XYZColor_d &i_color) const;

  private:
    std::vector<std::vector<RGBColor_f>> m_values;

    ColorSystem m_color_system;

    double m_scale;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* RGBImageSource uses FreeImage library, whose header conflicts with standard headers (e.g. windows.h) so it needs to be included in a separate cpp file.
* But because RGBImageSource is a template and has to be defined entirely in a header file, we had to separate the image loading routine to a separate function.
*/
std::vector<std::vector<RGBColor_f>> LoadImageFromFile(const std::string &i_filename, const ColorSystem &i_color_system);

template<typename T>
RGBImageSource<T>::RGBImageSource(const std::string &i_filename, const ColorSystem &i_color_system, double i_scale) :
m_color_system(i_color_system), m_scale(i_scale), m_values(LoadImageFromFile(i_filename, i_color_system))
  {
  }

template<typename T>
std::vector<std::vector<T>> RGBImageSource<T>::GetImage() const
  {
  std::vector<std::vector<T>> ret(GetHeight(), std::vector<T>(GetWidth()));

  for(size_t i=0;i<m_values.size();++i)
    {
    const std::vector<RGBColor_f> &source_row = m_values[i];

    std::vector<T> &dest_row = ret[i];
    dest_row.resize(m_values[i].size());

    // Just copy the values and multiply them by the scale factor.
    for(size_t j=0;j<m_values[i].size();++j)
      {
      RGBColor_d scaled
        {
        source_row[j].m_rgb[0] * m_scale,
        source_row[j].m_rgb[1] * m_scale,
        source_row[j].m_rgb[2] * m_scale
        };
      dest_row[j] = _XYZ_To_T(m_color_system.RGB_To_XYZ(scaled));
      }
    }

  return ret;
  }

template<typename T>
size_t RGBImageSource<T>::GetHeight() const
  {
  return m_values.size();
  }

template<typename T>
size_t RGBImageSource<T>::GetWidth() const
  {
  return m_values.empty() ? 0 : m_values[0].size();
  }

template<>
inline float RGBImageSource<float>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return (float)i_color[1];
  }

template<>
inline double RGBImageSource<double>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return i_color[1];
  }

template<>
inline Spectrum_f RGBImageSource<Spectrum_f>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return Convert<float>(SpectrumRoutines::XYZToSpectrum(i_color));
  }

template<>
inline Spectrum_d RGBImageSource<Spectrum_d>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return SpectrumRoutines::XYZToSpectrum(i_color);
  }

template<>
inline SpectrumCoef_f RGBImageSource<SpectrumCoef_f>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return Convert<float>(SpectrumRoutines::XYZToSpectrumCoef(i_color));
  }

template<>
inline SpectrumCoef_d RGBImageSource<SpectrumCoef_d>::_XYZ_To_T(const XYZColor_d &i_color) const
  {
  return SpectrumRoutines::XYZToSpectrumCoef(i_color);
  }

#endif // RGB_SPECTRUM_IMAGE_SOURCE_H