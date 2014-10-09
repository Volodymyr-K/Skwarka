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
