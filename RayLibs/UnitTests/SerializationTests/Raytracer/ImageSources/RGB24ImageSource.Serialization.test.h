#ifndef RGB24_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H
#define RGB24_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/ImageSources/RGB24ImageSource.h>
#include <Math/ThreadSafeRandom.h>
#include <vector>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class RGB24ImageSourceSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_RGB24ImageSource_Serialization()
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

      intrusive_ptr<ImageSource<Spectrum_f> > p_image_source1( new RGB24ImageSource<Spectrum_f>(values, scale) );
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

#endif // RGB24_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H
