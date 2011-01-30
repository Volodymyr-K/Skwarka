#ifndef COMPRESSED_DIRECTION_SERIALIZATION_TEST_H
#define COMPRESSED_DIRECTION_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Vector3D.h>
#include <Math/CompressedDirection.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class CompressedDirectionSerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    void test_CompressedDirection_Serialization()
      {
      Vector3D_d v = Vector3D_d(1,2,3).Normalized();
      CompressedDirection cd1(v);

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << cd1;
        } // archive and stream closed when destructors are called

      CompressedDirection cd2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> cd2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(cd1.ToVector3D<double>(), cd2.ToVector3D<double>());

      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // COMPRESSED_DIRECTION_SERIALIZATION_TEST_H