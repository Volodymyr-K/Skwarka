#ifndef BOX_FILTER_SERIALIZATION_TEST_H
#define BOX_FILTER_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/FilmFilter.h>
#include <Raytracer/FilmFilters/BoxFilter.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class BoxFilterSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_BoxFilter_Serialization()
      {
      intrusive_ptr<FilmFilter> p_filter1( new BoxFilter(1.0,2.0) );
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

      TS_ASSERT(dynamic_cast<BoxFilter*>(p_filter2.get()) != NULL); // Check run-time type.
      TS_ASSERT_EQUALS(p_filter2->GetXWidth(), 1.0);
      TS_ASSERT_EQUALS(p_filter2->GetYWidth(), 2.0);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // BOX_FILTER_SERIALIZATION_TEST_H
