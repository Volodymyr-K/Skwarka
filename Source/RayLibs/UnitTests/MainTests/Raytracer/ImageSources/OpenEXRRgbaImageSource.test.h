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

#ifndef OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_TEST_H
#define OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/ImageSources/OpenEXRRgbaImageSource.h>
#include <Math/ThreadSafeRandom.h>
#include <ImfRgba.h>
#include <ImfArray.h>
#include <vector>

class OpenEXRRgbaImageSourceTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_OpenEXRRgbaImageSource()
      {
      double scale = 2.0;

      size_t width = 1024, height=512; // these are dimensions of TestData/EXR/grace_latlong.exr
      intrusive_ptr<ImageSource<Spectrum_d>> p_image_source(new OpenEXRRgbaImageSource<Spectrum_d>("TestData/EXR/grace_latlong.exr", false, scale));

      TS_ASSERT_EQUALS(p_image_source->GetHeight(), height);
      TS_ASSERT_EQUALS(p_image_source->GetWidth(), width);

      std::vector<std::vector<Spectrum_d>> image{ p_image_source->GetImage() };
      for(size_t i=0;i<height;++i)
        for(size_t j=0;j<width;++j)
          {
          // Not really much we can verify here.
          TS_ASSERT(InRange(image[i][j], -DBL_INF, DBL_INF));
          }
      }

  };

#endif // OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_TEST_H
