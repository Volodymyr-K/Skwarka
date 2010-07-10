#ifndef RGB24_SPECTRUM_IMAGE_SOURCE_TEST_H
#define RGB24_SPECTRUM_IMAGE_SOURCE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/ImageSources/RGB24SpectrumImageSource.h>
#include <Math/ThreadSafeRandom.h>
#include <vector>

class RGB24SpectrumImageSourceTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_RGB24SpectrumImageSource()
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

      intrusive_ptr<ImageSource<Spectrum_f> > p_image_source( new RGB24SpectrumImageSource<float>(values, scale) );

      TS_ASSERT_EQUALS(p_image_source->GetHeight(), height);
      TS_ASSERT_EQUALS(p_image_source->GetWidth(), width);

      std::vector<std::vector<Spectrum_f> > image;
      p_image_source->GetImage(image);
      for(size_t i=0;i<height;++i)
        for(size_t j=0;j<width;++j)
          {
          Spectrum_f tmp(values[i][j].m_rgb[0],values[i][j].m_rgb[1],values[i][j].m_rgb[2]);
          if (image[i][j] != tmp*scale)
            {
            TS_FAIL("RGB24SpectrumImageSource test failed.");
            return;
            }
          }
      }
  };

#endif // RGB24_SPECTRUM_IMAGE_SOURCE_TEST_H
