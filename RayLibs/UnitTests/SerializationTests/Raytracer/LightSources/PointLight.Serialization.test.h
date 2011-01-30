#ifndef POINT_LIGHT_SERIALIZATION_TEST_H
#define POINT_LIGHT_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Math/ThreadSafeRandom.h>
#include <vector>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class PointLightSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_PointLight_Serialization()
      {
      Point3D_d light_pos(0,0,0);
      Spectrum_d intensity(1.0,2.0,3.0);
      intrusive_ptr<DeltaLightSource> p_light1(new PointLight(light_pos, intensity));

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_light1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<DeltaLightSource> p_light2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_light2;
        } // archive and stream closed when destructors are called

      Point3D_d point(2.0,3.0,4.0);
      Ray lighting_ray1, lighting_ray2;
      Spectrum_d lighting1 = p_light1->Lighting(point, lighting_ray1);
      Spectrum_d lighting2 = p_light2->Lighting(point, lighting_ray2);

      TS_ASSERT_EQUALS(lighting1, lighting2);
      TS_ASSERT_EQUALS(lighting_ray1.m_origin, lighting_ray2.m_origin);
      TS_ASSERT_EQUALS(lighting_ray1.m_direction, lighting_ray2.m_direction);
      TS_ASSERT_EQUALS(lighting_ray1.m_min_t, lighting_ray2.m_min_t);
      TS_ASSERT_EQUALS(lighting_ray1.m_max_t, lighting_ray2.m_max_t);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // POINT_LIGHT_SERIALIZATION_TEST_H
