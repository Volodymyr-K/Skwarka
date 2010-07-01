#ifndef VECTOR3D_SERIALIZATION_TEST_H
#define VECTOR3D_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Vector3D.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class Vector3DSerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    // Tests serialization with different extreme cases like infinity value, epsilon value and NaN.
    void test_Vector3D_Serialization()
      {
      Vector3D_d vd1(-0.2,DBL_INF, 123.0);
      Vector3D_f vf1(0.f, FLT_EPS, 999.f);
      vf1[0]=std::numeric_limits<float>::quiet_NaN();

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << vd1;
        output_archive << vf1;
        } // archive and stream closed when destructors are called

      Vector3D_d vd2;
      Vector3D_f vf2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> vd2;
        input_archive >> vf2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(vd1, vd2);
      TS_ASSERT_EQUALS(vf1[1],vf2[1]);
      TS_ASSERT_EQUALS(vf1[2],vf2[2]);
      if (IsNaN(vf2[0])==false) TS_FAIL("NaN value was not serialized properly.");
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // VECTOR3D_SERIALIZATION_TEST_H