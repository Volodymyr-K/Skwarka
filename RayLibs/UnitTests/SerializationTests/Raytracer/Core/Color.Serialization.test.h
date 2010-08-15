#ifndef COLOR_SERIALIZATION_TEST_H
#define COLOR_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/Color.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class ColorSerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    // Tests serialization with different extreme cases like infinity value, epsilon value and NaN.
    void test_RGBColor_Serialization()
      {
      RGBColor_d cd1(-0.2,DBL_INF, 123.0);
      RGBColor_f cf1(0.f, FLT_EPS, 999.f);
      cf1[0]=std::numeric_limits<float>::quiet_NaN();

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << cd1;
        output_archive << cf1;
        } // archive and stream closed when destructors are called

      RGBColor_d cd2;
      RGBColor_f cf2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> cd2;
        input_archive >> cf2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(cd1[0], cd2[0]);
      TS_ASSERT_EQUALS(cd1[1], cd2[1]);
      TS_ASSERT_EQUALS(cd1[2], cd2[2]);

      TS_ASSERT_EQUALS(cf1[1],cf2[1]);
      TS_ASSERT_EQUALS(cf1[2],cf2[2]);
      if (IsNaN(cf2[0])==false) TS_FAIL("NaN value serialization test failed.");
      }

    // Tests serialization with different extreme cases like infinity value, epsilon value and NaN.
    void test_XYZColor_Serialization()
      {
      XYZColor_d cd1(-0.2,DBL_INF, 123.0);
      XYZColor_f cf1(0.f, FLT_EPS, 999.f);
      cf1[0]=std::numeric_limits<float>::quiet_NaN();

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << cd1;
        output_archive << cf1;
        } // archive and stream closed when destructors are called

      XYZColor_d cd2;
      XYZColor_f cf2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> cd2;
        input_archive >> cf2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(cd1[0], cd2[0]);
      TS_ASSERT_EQUALS(cd1[1], cd2[1]);
      TS_ASSERT_EQUALS(cd1[2], cd2[2]);

      TS_ASSERT_EQUALS(cf1[1],cf2[1]);
      TS_ASSERT_EQUALS(cf1[2],cf2[2]);
      if (IsNaN(cf2[0])==false) TS_FAIL("NaN value serialization test failed.");
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // COLOR_SERIALIZATION_TEST_H