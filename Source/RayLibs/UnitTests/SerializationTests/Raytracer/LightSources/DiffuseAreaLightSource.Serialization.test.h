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

#ifndef DIFFUSE_AREA_LIGHT_SERIALIZATION_TEST_H
#define DIFFUSE_AREA_LIGHT_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Math/SamplingRoutines.h>
#include <UnitTests/TestHelpers/TriangleMeshTestHelper.h>
#include <vector>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class DiffuseAreaLightSourceSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_DiffuseAreaLightSource_Serialization()
      {
      intrusive_ptr<TriangleMesh> p_mesh( TriangleMeshHelper::ConstructTetrahedron() );
      intrusive_ptr<AreaLightSource> p_light1( new DiffuseAreaLightSource(Spectrum_d(1.0), p_mesh));

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_light1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<AreaLightSource> p_light2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_light2;
        } // archive and stream closed when destructors are called

      Point3D_d point(3.0,4.0,5.0);
      Ray lighting_ray1, lighting_ray2;
      double pdf1, pdf2;
      Point2D_d sample2D(RandomDouble(1.0), RandomDouble(1.0));
      double sample1D = RandomDouble(1.0);
      Spectrum_d sampled_radiance1 = p_light1->SampleLighting(point, sample1D, sample2D, lighting_ray1, pdf1);
      Spectrum_d sampled_radiance2 = p_light2->SampleLighting(point, sample1D, sample2D, lighting_ray2, pdf2);

      TS_ASSERT_EQUALS(sampled_radiance1, sampled_radiance2);
      TS_ASSERT_EQUALS(lighting_ray1.m_origin, lighting_ray2.m_origin);
      TS_ASSERT_EQUALS(lighting_ray1.m_direction, lighting_ray2.m_direction);
      TS_ASSERT_EQUALS(lighting_ray1.m_min_t, lighting_ray2.m_min_t);
      TS_ASSERT_EQUALS(lighting_ray1.m_max_t, lighting_ray2.m_max_t);
      TS_ASSERT_EQUALS(pdf1, pdf2);
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // DIFFUSE_AREA_LIGHT_SERIALIZATION_TEST_H
