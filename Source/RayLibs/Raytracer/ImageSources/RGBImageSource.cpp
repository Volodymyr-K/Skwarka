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

#include "RGBImageSource.h"

#include <vector>
#include <string>
#include <exception>
#include <Raytracer/Core/Color.h>
#include <FreeImage.h>

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_IMPLEMENT(RGBImageSource_float);
BOOST_CLASS_EXPORT_IMPLEMENT(RGBImageSource_double);
BOOST_CLASS_EXPORT_IMPLEMENT(RGBImageSource_Spectrum_float);
BOOST_CLASS_EXPORT_IMPLEMENT(RGBImageSource_Spectrum_double);
BOOST_CLASS_EXPORT_IMPLEMENT(RGBImageSource_SpectrumCoef_float);
BOOST_CLASS_EXPORT_IMPLEMENT(RGBImageSource_SpectrumCoef_double);

std::vector<std::vector<RGBColor_f>> LoadImageFromFile(const std::string &i_filename, const ColorSystem &i_color_system)
  {
  // check the file signature and deduce its format (the second argument is currently not used by FreeImage)
  FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(i_filename.c_str(), 0);

  // if no signature was detected, try to guess the file format from the file extension
  if (fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename(i_filename.c_str());

  FIBITMAP *p_image = FreeImage_Load(fif, i_filename.c_str());
  if (p_image)
    {
    // Doing the guesswork on whether the gamma correction has been applied to the source image.
    bool gamma_applied = fif!=FIF_EXR && fif!=FIF_HDR;

    // Convert image to the normalized format with red, green and blue components in [0;1] range.
    FIBITMAP *p_converted_image = FreeImage_ConvertToRGBF(p_image);
    FreeImage_Unload(p_image);

    if (p_converted_image)
      {
      size_t width = FreeImage_GetWidth(p_converted_image);
      size_t height = FreeImage_GetHeight(p_converted_image);
      size_t pitch = FreeImage_GetPitch(p_converted_image);

      FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(p_converted_image);
      ASSERT(image_type == FIT_RGBF);

      BYTE *p_bits = (BYTE*)FreeImage_GetBits(p_converted_image);
      std::vector<std::vector<RGBColor_f>> values(height, std::vector<RGBColor_f>(width, RGBColor_f()));
      for (size_t i=0; i<height; ++i)
        {
        // FreeImage loads image upside down, so we need to flip it back.
        std::vector<RGBColor_f> &values_row = values[height-i-1];

        FIRGBF *pixel = (FIRGBF*)p_bits;
        if (gamma_applied)
          for (size_t j=0; j<width; ++j)
            values_row[j] = i_color_system.GammaDecode(RGBColor_f(pixel[j].red, pixel[j].green, pixel[j].blue));
        else
          for (size_t j=0; j<width; ++j)
            values_row[j] = RGBColor_f(pixel[j].red, pixel[j].green, pixel[j].blue);

        p_bits += pitch; // get next line
        }

      FreeImage_Unload(p_converted_image);
      return values;
      }
    else throw std::exception(std::string("Could not convert image to RGB: " + i_filename).c_str());
    }
  else throw std::exception(std::string("Could not load image file: " + i_filename).c_str());
  }