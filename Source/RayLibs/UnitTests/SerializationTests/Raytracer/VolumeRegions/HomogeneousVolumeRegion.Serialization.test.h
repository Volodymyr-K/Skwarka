/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HOMOGENEOUS_VOLUME_REGION_SERIALIZATION_TEST_H
#define HOMOGENEOUS_VOLUME_REGION_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/VolumeRegions/HomogeneousVolumeRegion.h>
#include <Raytracer/PhaseFunctions/RayleighPhaseFunction.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class HomogeneousVolumeRegionSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
  
    void test_HomogeneousVolumeRegion_Serialization()
      {
      BBox3D_d bounds(Point3D_d(1,2,3), Point3D_d(10,20,30));
      SpectrumCoef_d absorption(5,7,9), scattering(0.1,0.0,0.9);
      intrusive_ptr<PhaseFunction> p_phase_function( new RayleighPhaseFunction );

      intrusive_ptr<VolumeRegion> p_volume1(new HomogeneousVolumeRegion(bounds, absorption, scattering, p_phase_function));

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
      TS_ASSERT_EQUALS(p_volume1->Absorption(point), p_volume2->Absorption(point));
      TS_ASSERT_EQUALS(p_volume1->Scattering(point), p_volume2->Scattering(point));
      TS_ASSERT_EQUALS(p_volume1->Phase(point,d1,d2), p_volume2->Phase(point,d1,d2));
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // HOMOGENEOUS_VOLUME_REGION_SERIALIZATION_TEST_H
