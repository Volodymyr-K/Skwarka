#ifndef SPECTRUM_COEF_SERIALIZATION_TEST_H
#define SPECTRUM_COEF_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/SpectrumCoef.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class SpectrumCoefSerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    // Tests serialization with different extreme cases like infinity value, epsilon value and NaN.
    void test_SpectrumCoef_Serialization()
      {
      SpectrumCoef_d sd1(-0.2,DBL_INF, 123.0);
      SpectrumCoef_f sf1(0.f, FLT_EPS, 999.f);
      sf1[0]=std::numeric_limits<float>::quiet_NaN();

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << sd1;
        output_archive << sf1;
        } // archive and stream closed when destructors are called

      SpectrumCoef_d sd2;
      SpectrumCoef_f sf2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> sd2;
        input_archive >> sf2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(sd1, sd2);
      TS_ASSERT_EQUALS(sf1[1],sf2[1]);
      TS_ASSERT_EQUALS(sf1[2],sf2[2]);
      if (IsNaN(sf2[0])==false) TS_FAIL("NaN value serialization test failed.");
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // SPECTRUM_COEF_SERIALIZATION_TEST_H