#ifndef GRID_DENSITY_VOLUME_REGION_SERIALIZATION_TEST_H
#define GRID_DENSITY_VOLUME_REGION_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/VolumeRegions/GridDensityVolumeRegion.h>
#include <Raytracer/PhaseFunctions/RayleighPhaseFunction.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class GridDensityVolumeRegionSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_GridDensityVolumeRegion_Serialization()
      {
      BBox3D_d bounds(Point3D_d(1,2,3), Point3D_d(10,20,30));
      Spectrum_d emission(1,1.5,2);
      SpectrumCoef_d absorption(5,7,9), scattering(0.1,0.0,0.9);
      intrusive_ptr<PhaseFunction> p_phase_function( new RayleighPhaseFunction );

      size_t size_x=20, size_y=30, size_z=40;
      std::vector<std::vector<std::vector<float>>> densities(size_x, std::vector<std::vector<float>>(size_y, std::vector<float>(size_z, 0.0)));
      for(size_t i=0;i<size_x;++i)
        for(size_t j=0;j<size_y;++j)
          for(size_t k=0;k<size_z;++k)
            densities[i][j][k]=(float)(i+j+k);

      intrusive_ptr<VolumeRegion> p_volume1( new GridDensityVolumeRegion(bounds, emission, absorption, scattering, p_phase_function, densities) );

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

      Point3D_d point(5,6,7);
      Vector3D_d d1=Vector3D_d(1,2,3).Normalized(), d2=Vector3D_d(-2,1,-3).Normalized();
      TS_ASSERT_EQUALS(p_volume1->GetBounds().m_min, p_volume2->GetBounds().m_min);
      TS_ASSERT_EQUALS(p_volume1->GetBounds().m_max, p_volume2->GetBounds().m_max);
      TS_ASSERT_EQUALS(p_volume1->Emission(point), p_volume2->Emission(point));
      TS_ASSERT_EQUALS(p_volume1->Absorption(point), p_volume2->Absorption(point));
      TS_ASSERT_EQUALS(p_volume1->Scattering(point), p_volume2->Scattering(point));
      TS_ASSERT_EQUALS(p_volume1->Phase(point,d1,d2), p_volume2->Phase(point,d1,d2));
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // GRID_DENSITY_VOLUME_REGION_SERIALIZATION_TEST_H
