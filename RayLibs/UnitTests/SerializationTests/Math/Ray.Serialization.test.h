#ifndef RAY_SERIALIZATION_TEST_H
#define RAY_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Ray.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class RaySerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    void test_Ray_Serialization()
      {
      Ray r1(Point3D_d(-0.2,14.9,1234.0), Vector3D_d(10.2,-99.0,5.0), -DBL_INF, DBL_INF);
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << r1;
        } // archive and stream closed when destructors are called

      Ray r2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> r2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(r1.m_origin, r2.m_origin);
      TS_ASSERT_EQUALS(r1.m_direction, r2.m_direction);
      TS_ASSERT_EQUALS(r1.m_min_t, r2.m_min_t);
      TS_ASSERT_EQUALS(r1.m_max_t, r2.m_max_t);
      }

    void test_RayDifferential_Serialization()
      {
      Ray r1(Point3D_d(-0.2,14.9,1234.0), Vector3D_d(10.2,-99.0,5.0), -DBL_INF, DBL_INF);
      RayDifferential rd1(r1);
      rd1.m_direction_dx=Vector3D_d(-1,-2,-3);
      rd1.m_direction_dy=Vector3D_d(-11,-21,-31);
      rd1.m_has_differentials=true;
      rd1.m_origin_dx=Point3D_d(2,3,4);
      rd1.m_origin_dy=Point3D_d(5,6,7);
      rd1.m_specular_depth=100;
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << rd1;
        } // archive and stream closed when destructors are called

      RayDifferential rd2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> rd2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(rd1.m_base_ray.m_origin, rd2.m_base_ray.m_origin);
      TS_ASSERT_EQUALS(rd1.m_base_ray.m_direction, rd2.m_base_ray.m_direction);
      TS_ASSERT_EQUALS(rd1.m_base_ray.m_min_t, rd2.m_base_ray.m_min_t);
      TS_ASSERT_EQUALS(rd1.m_base_ray.m_max_t, rd2.m_base_ray.m_max_t);

      TS_ASSERT_EQUALS(rd1.m_direction_dx, rd2.m_direction_dx);
      TS_ASSERT_EQUALS(rd1.m_direction_dy, rd2.m_direction_dy);
      TS_ASSERT_EQUALS(rd1.m_origin_dx, rd2.m_origin_dx);
      TS_ASSERT_EQUALS(rd1.m_origin_dy, rd2.m_origin_dy);
      TS_ASSERT_EQUALS(rd1.m_has_differentials, rd2.m_has_differentials);
      TS_ASSERT_EQUALS(rd1.m_specular_depth, rd2.m_specular_depth);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // RAY_SERIALIZATION_TEST_H