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

#ifndef SPHERICAL_MAPPING_2D_SERIALIZATION_TEST_H
#define SPHERICAL_MAPPING_2D_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Raytracer/Mappings/SphericalMapping2D.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class SphericalMapping2DSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_SphericalMapping2D_Serialization()
      {
      Point3D_d center(1,2,3);
      Vector3D_d z_axis(1,0,0), x_axis(0,1,0);
      intrusive_ptr<Mapping2D> p_mapping1( new SphericalMapping2D(center, z_axis, x_axis) );

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
      dg.m_point = center+Point3D_d(10,1,1);
      dg.m_point_dx = center+Point3D_d(0,1,0);
      dg.m_point_dy = center+Point3D_d(0,1,-1);

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

#endif // SPHERICAL_MAPPING_2D_SERIALIZATION_TEST_H