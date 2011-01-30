#ifndef TRANSFORM_MAPPING_3D_SERIALIZATION_TEST_H
#define TRANSFORM_MAPPING_3D_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Raytracer/Mappings/TransformMapping3D.h>
#include <Math/Transform.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class TransformMapping3DSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_TransformMapping3D_Serialization()
      {
      Transform t = MakeRotationX(0.5);
      intrusive_ptr<Mapping3D> p_mapping1( new TransformMapping3D(t) );

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_mapping1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Mapping3D> p_mapping2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_mapping2;
        } // archive and stream closed when destructors are called

      DifferentialGeometry dg;
      dg.m_point = Point3D_d(10,1,1);
      dg.m_point_dx = dg.m_point+Point3D_d(0,1,0);
      dg.m_point_dy = dg.m_point+Point3D_d(0,1,-1);

      Point3D_d mapped1, mapped2;
      Vector3D_d dp_dx1, dp_dy1;
      Vector3D_d dp_dx2, dp_dy2;
      p_mapping1->Map(dg, 0, mapped1, dp_dx1, dp_dy1);
      p_mapping2->Map(dg, 0, mapped2, dp_dx2, dp_dy2);
      TS_ASSERT_EQUALS(mapped1, mapped2);
      TS_ASSERT_EQUALS(dp_dx1, dp_dx2);
      TS_ASSERT_EQUALS(dp_dy1, dp_dy2);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // TRANSFORM_MAPPING_3D_SERIALIZATION_TEST_H