#ifndef UV_MAPPING_2D_SERIALIZATION_TEST_H
#define UV_MAPPING_2D_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Raytracer/Mappings/UVMapping2D.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class UVMapping2DSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_UVMapping2D_Serialization()
      {
      double u_scale=0.5, v_scale=2.0;
      Vector2D_d translation(1,-1);
      intrusive_ptr<Mapping2D> p_mapping1( new UVMapping2D(u_scale,v_scale,translation) );

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_mapping1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Mapping2D> p_mapping2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_mapping2;
        } // archive and stream closed when destructors are called

      DifferentialGeometry dg;
      dg.m_uv = Point2D_d(1,2);
      dg.m_duv_dx = Vector2D_d(3,4);
      dg.m_duv_dy = Vector2D_d(5,6);

      Point2D_d mapped1, mapped2;
      Vector2D_d dp_dx1, dp_dy1;
      Vector2D_d dp_dx2, dp_dy2;
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

#endif // UV_MAPPING_2D_SERIALIZATION_TEST_H