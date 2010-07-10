#ifndef OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_TEST_H
#define OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/ImageSources/OpenEXRRgbaSpectrumImageSource.h>
#include <Math/ThreadSafeRandom.h>
#include <ImfRgba.h>
#include <ImfArray.h>
#include <vector>

class OpenEXRRgbaSpectrumImageSourceTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_OpenEXRRgbaSpectrumImageSource_InitializedFromVector()
      {
      size_t width = 123, height=234;
      std::vector<std::vector<Imf::Rgba> > values(height, std::vector<Imf::Rgba>(width));
      double scale = 1.0/255.0;

      for(size_t i=0;i<height;++i)
        for(size_t j=0;j<width;++j)
          {
          values[i][j].r = (float)RandomDouble(1000);
          values[i][j].g = (float)RandomDouble(1000);
          values[i][j].b = (float)RandomDouble(1000);
          values[i][j].a = 0.f;
          }

      intrusive_ptr<ImageSource<Spectrum_f> > p_image_source( new OpenEXRRgbaSpectrumImageSource<float>(values, scale) );

      TS_ASSERT_EQUALS(p_image_source->GetHeight(), height);
      TS_ASSERT_EQUALS(p_image_source->GetWidth(), width);

      std::vector<std::vector<Spectrum_f> > image;
      p_image_source->GetImage(image);
      for(size_t i=0;i<height;++i)
        for(size_t j=0;j<width;++j)
          {
          Spectrum_f tmp(values[i][j].r,values[i][j].g,values[i][j].b);
          if (image[i][j] != tmp*scale)
            {
            TS_FAIL("OpenEXRRgbaSpectrumImageSource test failed.");
            return;
            }
          }
      }

    void test_OpenEXRRgbaSpectrumImageSource_InitializedFromOpenEXRArray2D()
      {
      size_t width = 123, height=234;

      Imf::Array2D<Imf::Rgba> values(height, width);
      double scale = 1.0/255.0;

      for(size_t i=0;i<height;++i)
        for(size_t j=0;j<width;++j)
          {
          values[i][j].r = (float)RandomDouble(1000);
          values[i][j].g = (float)RandomDouble(1000);
          values[i][j].b = (float)RandomDouble(1000);
          values[i][j].a = 0.f;
          }

        intrusive_ptr<ImageSource<Spectrum_f> > p_image_source( new OpenEXRRgbaSpectrumImageSource<float>(values, width, height, scale) );

        TS_ASSERT_EQUALS(p_image_source->GetHeight(), height);
        TS_ASSERT_EQUALS(p_image_source->GetWidth(), width);

        std::vector<std::vector<Spectrum_f> > image;
        p_image_source->GetImage(image);
        for(size_t i=0;i<height;++i)
          for(size_t j=0;j<width;++j)
            {
            Spectrum_f tmp(values[i][j].r,values[i][j].g,values[i][j].b);
            if (image[i][j] != tmp*scale)
              {
              TS_FAIL("OpenEXRRgbaSpectrumImageSource test failed.");
              return;
              }
            }
      }
  };

#endif // OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_TEST_H
