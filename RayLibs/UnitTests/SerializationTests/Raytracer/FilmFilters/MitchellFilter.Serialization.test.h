#ifndef MITCHELL_FILTER_SERIALIZATION_TEST_H
#define MITCHELL_FILTER_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/FilmFilter.h>
#include <Raytracer/FilmFilters/MitchellFilter.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class MitchellFilterSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_MitchellFilter_Serialization()
      {
      intrusive_ptr<FilmFilter> p_filter1( new MitchellFilter(1.0,2.0,0.1,0.2) );
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_filter1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<FilmFilter> p_filter2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_filter2;
        } // archive and stream closed when destructors are called

      TS_ASSERT(dynamic_cast<MitchellFilter*>(p_filter2.get()) != NULL); // Check run-time type.
      TS_ASSERT_EQUALS(p_filter2->GetXWidth(), 1.0);
      TS_ASSERT_EQUALS(p_filter2->GetYWidth(), 2.0);

      double b1,c1,b2,c2;
      dynamic_cast<MitchellFilter*>(p_filter1.get())->GetParameters(b1,c1);
      dynamic_cast<MitchellFilter*>(p_filter2.get())->GetParameters(b2,c2);
      TS_ASSERT_EQUALS(b1,b2);
      TS_ASSERT_EQUALS(c1,c2);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // MITCHELL_FILTER_SERIALIZATION_TEST_H
