#ifndef OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H
#define OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/ImageSources/OpenEXRRgbaImageSource.h>
#include <Math/ThreadSafeRandom.h>
#include <ImfRgba.h>
#include <ImfArray.h>
#include <vector>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class OpenEXRRgbaImageSourceSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_OpenEXRRgbaImageSource_Serialization()
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

      intrusive_ptr<ImageSource<Spectrum_f> > p_image_source1( new OpenEXRRgbaImageSource<Spectrum_f>(values, scale) );
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_image_source1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<ImageSource<Spectrum_f> > p_image_source2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_image_source2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(p_image_source1->GetHeight(), p_image_source2->GetHeight());
      TS_ASSERT_EQUALS(p_image_source1->GetWidth(), p_image_source2->GetWidth());

      std::vector<std::vector<Spectrum_f> > image1, image2;
      p_image_source1->GetImage(image1);
      p_image_source2->GetImage(image2);
      if (image1 != image2)
        TS_FAIL("OpenEXRRgbaImageSource serialization test failed.");
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H
