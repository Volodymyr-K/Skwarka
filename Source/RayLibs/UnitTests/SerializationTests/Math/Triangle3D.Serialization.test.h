#ifndef TRIANGLE3D_SERIALIZATION_TEST_H
#define TRIANGLE3D_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Triangle3D.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class Triangle3DSerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    void test_Triangle3D_Serialization()
      {
      Triangle3D_d tr1(Point3D_d(-0.2,14.9,1234.0), Point3D_d(10.2,999.0,1235.0), Point3D_d(1.0,2.0,3.0));
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << tr1;
        } // archive and stream closed when destructors are called

      Triangle3D_d tr2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> tr2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(tr1[0], tr2[0]);
      TS_ASSERT_EQUALS(tr1[1], tr2[1]);
      TS_ASSERT_EQUALS(tr1[2], tr2[2]);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // TRIANGLE3D_SERIALIZATION_TEST_H