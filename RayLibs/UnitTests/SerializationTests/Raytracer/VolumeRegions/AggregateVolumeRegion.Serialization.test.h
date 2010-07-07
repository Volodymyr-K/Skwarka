#ifndef AGGREGATE_VOLUME_REGION_SERIALIZATION_TEST_H
#define AGGREGATE_VOLUME_REGION_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/VolumeRegions/AggregateVolumeRegion.h>
#include <Raytracer/VolumeRegions/HomogeneousVolumeRegion.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/PhaseFunctions/RayleighPhaseFunction.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class AggregateVolumeRegionSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_AggregateVolumeRegion_Serialization()
      {
      BBox3D_d bounds1(Point3D_d(0,0,0), Point3D_d(2,2,2));
      Spectrum_d emission1(1,1.5,2);
      Spectrum_d absorption1(5,7,9);
      Spectrum_d scattering1(0.1,0.0,0.9);

      BBox3D_d bounds2(Point3D_d(1,1,1), Point3D_d(3,3,3));
      Spectrum_d emission2(2,0.5,1);
      Spectrum_d absorption2(2,1,4);
      Spectrum_d scattering2(5.1,0.3,3.9);

      intrusive_ptr<PhaseFunction> p_phase_function( new RayleighPhaseFunction );
      intrusive_ptr<VolumeRegion> p_region1(new HomogeneousVolumeRegion(bounds1, emission1, absorption1, scattering1, p_phase_function));
      intrusive_ptr<VolumeRegion> p_region2(new HomogeneousVolumeRegion(bounds2, emission2, absorption2, scattering2, p_phase_function));

      std::vector<intrusive_ptr<const VolumeRegion> > regions;
      regions.push_back(p_region1);
      regions.push_back(p_region2);

      intrusive_ptr<VolumeRegion> p_volume1(new AggregateVolumeRegion(regions));

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_volume1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<VolumeRegion> p_volume2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_volume2;
        } // archive and stream closed when destructors are called

      const size_t N = 100;
      Vector3D_d d1=Vector3D_d(1,2,3).Normalized(), d2=Vector3D_d(-2,1,-3).Normalized();
      for(int i=0;i<N;++i)
        {
        Point3D_d point(RandomDouble(3.0), RandomDouble(3.0), RandomDouble(3.0));

        TS_ASSERT_EQUALS(p_volume1->GetBounds().m_min, p_volume2->GetBounds().m_min);
        TS_ASSERT_EQUALS(p_volume1->GetBounds().m_max, p_volume2->GetBounds().m_max);
        TS_ASSERT_EQUALS(p_volume1->Emission(point), p_volume2->Emission(point));
        TS_ASSERT_EQUALS(p_volume1->Absorption(point), p_volume2->Absorption(point));
        TS_ASSERT_EQUALS(p_volume1->Scattering(point), p_volume2->Scattering(point));
        TS_ASSERT_EQUALS(p_volume1->Phase(point,d1,d2), p_volume2->Phase(point,d1,d2));
        }
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // AGGREGATE_VOLUME_REGION_SERIALIZATION_TEST_H
