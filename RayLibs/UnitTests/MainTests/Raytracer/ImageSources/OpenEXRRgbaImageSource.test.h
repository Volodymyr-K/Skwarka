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
      size_t width = 123, height=234;
      std::vector<Imf::Rgba> values(width*height);
      double scale = 2.0;

      for(size_t i=0;i<height;++i)
        for(size_t j=0;j<width;++j)
          {
          values[i*width+j].r = (float)RandomDouble(1000);
          values[i*width+j].g = (float)RandomDouble(1000);
          values[i*width+j].b = (float)RandomDouble(1000);
          values[i*width+j].a = 0.f;
          }

      intrusive_ptr<ImageSource<Spectrum_d> > p_image_source( new OpenEXRRgbaImageSource<Spectrum_d>(values, width, height, global_sRGB_E_ColorSystem, scale) );

      TS_ASSERT_EQUALS(p_image_source->GetHeight(), height);
      TS_ASSERT_EQUALS(p_image_source->GetWidth(), width);

      std::vector<std::vector<Spectrum_d> > image;
      p_image_source->GetImage(image);
      for(size_t i=0;i<height;++i)
        for(size_t j=0;j<width;++j)
          {
          RGBColor_d rgb(values[i*width+j].r,values[i*width+j].g,values[i*width+j].b);
          XYZColor_d xyz = global_sRGB_E_ColorSystem.RGB_To_XYZ(rgb);
          Spectrum_d tmp = SpectrumRoutines::XYZToSpectrum(xyz);

          if (InRange(image[i][j] - tmp*scale, -(1e-10), (1e-10))==false)
            {
            TS_FAIL("OpenEXRRgbaImageSource test failed.");
            return;
            }
          }
      }

  };

#endif // OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_TEST_H
