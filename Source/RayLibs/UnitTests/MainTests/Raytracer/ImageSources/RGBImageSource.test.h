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

#ifndef RGB_SPECTRUM_IMAGE_SOURCE_TEST_H
#define RGB_SPECTRUM_IMAGE_SOURCE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/Color.h>
#include <Raytracer/Core/SpectrumCoef.h>
#include <Raytracer/ImageSources/RGBImageSource.h>
#include <Math/ThreadSafeRandom.h>
#include <vector>

class RGBImageSourceTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_RGBImageSource()
      {
      double scale = 12;
      size_t width=200, height=100;
      intrusive_ptr<ImageSource<SpectrumCoef_f>> p_image_source(new RGBImageSource<SpectrumCoef_f>("TestData/red_200x100.tif", global_sRGB_E_ColorSystem, scale) );

      TS_ASSERT_EQUALS(p_image_source->GetHeight(), height);
      TS_ASSERT_EQUALS(p_image_source->GetWidth(), width);

      std::vector<std::vector<SpectrumCoef_f>> image{ p_image_source->GetImage() };
      for(size_t i=0;i<height;++i)
        for(size_t j=0;j<width;++j)
          {
          SpectrumCoef_f value = image[i][j];

          // There isn't much we can verify, but we at least know that the image is supposed to be red
          TS_ASSERT(value[2]>value[1] && value[2]>value[0]);
          }
      }
  };

#endif // RGB_SPECTRUM_IMAGE_SOURCE_TEST_H
