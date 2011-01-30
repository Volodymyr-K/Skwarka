#ifndef BBOX3D_SERIALIZATION_TEST_H
#define BBOX3D_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/BBox3D.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class BBox3DSerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    void test_BBox3D_Serialization()
      {
      BBox3D_d bbox1(Point3D_d(-0.2,14.9,1234.0), Point3D_d(10.2,999.0,1235.0));
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << bbox1;
        } // archive and stream closed when destructors are called

      BBox3D_d bbox2; 
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> bbox2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(bbox1.m_min, bbox2.m_min);
      TS_ASSERT_EQUALS(bbox1.m_max, bbox2.m_max);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // BBOX3D_SERIALIZATION_TEST_H