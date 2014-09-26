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
