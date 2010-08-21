#ifndef RGB24_SPECTRUM_IMAGE_SOURCE_TEST_H
#define RGB24_SPECTRUM_IMAGE_SOURCE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/Color.h>
#include <Raytracer/Core/SpectrumCoef.h>
#include <Raytracer/ImageSources/RGB24ImageSource.h>
#include <Math/ThreadSafeRandom.h>
#include <vector>

class RGB24ImageSourceTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_RGB24ImageSource()
      {
      size_t width = 123, height=234;
      std::vector<std::vector<RGB24> > values(height, std::vector<RGB24>(width));
      double scale = 1.0/255.0;

      for(size_t i=0;i<height;++i)
        for(size_t j=0;j<width;++j)
          {
          values[i][j].m_rgb[0]=(unsigned char)RandomInt(256);
          values[i][j].m_rgb[1]=(unsigned char)RandomInt(256);
          values[i][j].m_rgb[2]=(unsigned char)RandomInt(256);
          }

      intrusive_ptr<ImageSource<SpectrumCoef_f> > p_image_source( new RGB24ImageSource<SpectrumCoef_f>(values, global_sRGB_E_ColorSystem, scale) );

      TS_ASSERT_EQUALS(p_image_source->GetHeight(), height);
      TS_ASSERT_EQUALS(p_image_source->GetWidth(), width);

      std::vector<std::vector<SpectrumCoef_f> > image;
      p_image_source->GetImage(image);
      for(size_t i=0;i<height;++i)
        for(size_t j=0;j<width;++j)
          {
          RGBColor_f rgb(values[i][j].m_rgb[0]/255.f, values[i][j].m_rgb[1]/255.f, values[i][j].m_rgb[2]/255.f);
          XYZColor_f xyz = global_sRGB_E_ColorSystem.RGB_To_XYZ(global_sRGB_E_ColorSystem.GammaDecode(rgb));
          SpectrumCoef_f tmp = SpectrumRoutines::XYZToSpectrumCoef(xyz);

          if (InRange(image[i][j] - tmp*scale, -float(1e-5), float(1e-5))==false)
            {
            TS_FAIL("RGB24ImageSource test failed.");
            return;
            }
          }
      }
  };

#endif // RGB24_SPECTRUM_IMAGE_SOURCE_TEST_H
