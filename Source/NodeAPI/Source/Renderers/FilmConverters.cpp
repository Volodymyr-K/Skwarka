/*
* Copyright (C) 2015 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
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

#include "FilmConverters.h"

#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/Color.h>
#include <Raytracer/Core/SpectrumRoutines.h>

#include <FreeImage.h>

namespace NodeAPI
  {
  namespace FilmConverters
    {

    std::vector<unsigned char> FilmToPNG(intrusive_ptr<const Film> ip_film)
      {
      std::vector<unsigned char> ret;
      const Film *p_film = ip_film.get();
      int height = (int)p_film->GetYResolution(), width = (int)p_film->GetXResolution();

      double inv_256 = 1.0 / 256.0;
      FIBITMAP *dib = FreeImage_Allocate(width, height, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
      if (dib)
        {
        // Calculate the number of bytes per pixel (3 for 24-bit or 4 for 32-bit)
        int bytespp = FreeImage_GetLine(dib) / FreeImage_GetWidth(dib);
        for (int y = 0; y < height; y++)
          {
          BYTE *bits = FreeImage_GetScanLine(dib, y);
          for (int x = 0; x < width; x++)
            {
            Spectrum_d sp;
            p_film->GetPixel(Point2D_i(x, height - y - 1), sp);
            sp *= 500;//250;

            RGBColor_d color = global_sRGB_D65_ColorSystem.XYZ_To_RGB(SpectrumRoutines::SpectrumToXYZ(sp), true);
            color.Clamp(0.0, 256.0);

            color[0] *= inv_256; color[1] *= inv_256; color[2] *= inv_256;
            color = global_sRGB_D65_ColorSystem.GammaEncode(color);
            int r = std::min(255, (int)(color[0] * 256.0));
            int g = std::min(255, (int)(color[1] * 256.0));
            int b = std::min(255, (int)(color[2] * 256.0));

            // Set pixel color to green with a transparency of 128
            bits[FI_RGBA_RED] = BYTE(r);
            bits[FI_RGBA_GREEN] = BYTE(g);
            bits[FI_RGBA_BLUE] = BYTE(b);
            bits[FI_RGBA_ALPHA] = 255;
            // jump to next pixel
            bits += bytespp;
            }
          }

        // open a memory stream
        FIMEMORY *hmem = FreeImage_OpenMemory();

        // encode and save the image to the memory
        FreeImage_SaveToMemory(FIF_PNG, dib, hmem, 0);
        FreeImage_Unload(dib);

        // get the buffer from the memory stream
        BYTE *mem_buffer = NULL;
        DWORD size_in_bytes = 0;
        FreeImage_AcquireMemory(hmem, &mem_buffer, &size_in_bytes);
        const unsigned char *p_buf = reinterpret_cast<const unsigned char *>(mem_buffer);
        ret.assign(p_buf, p_buf + size_in_bytes);

        // make sure to close the stream since FreeImage_SaveToMemory
        // will cause internal memory allocations and this is the only
        // way to free this allocated memory
        FreeImage_CloseMemory(hmem);
        }

      return ret;
      }

    }
  }