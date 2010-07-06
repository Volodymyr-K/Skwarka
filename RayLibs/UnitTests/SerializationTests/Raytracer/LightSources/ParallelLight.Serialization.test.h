#ifndef PARALLEL_LIGHT_SERIALIZATION_TEST_H
#define PARALLEL_LIGHT_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Numerics.h>
#include <Raytracer/LightSources/ParallelLight.h>
#include <Math/ThreadSafeRandom.h>
#include <vector>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class ParallelLightSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_ParallelLight_Serialization()
      {
      Vector3D_d direction = Vector3D_d(1,1,1).Normalized();
      Spectrum_d radiance(1.0,2.0,3.0);

      BBox3D_d bbox(Point3D_d(0,0,0), Point3D_d(10,20,30));
      intrusive_ptr<DeltaLightSource> p_light1(new ParallelLight(direction, radiance, bbox));

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

#endif // PARALLEL_LIGHT_SERIALIZATION_TEST_H
