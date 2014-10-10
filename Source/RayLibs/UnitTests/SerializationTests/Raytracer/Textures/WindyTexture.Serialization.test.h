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

#ifndef WINDY_TEXTURE_SERIALIZATION_TEST_H
#define WINDY_TEXTURE_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Textures/WindyTexture.h>
#include <Raytracer/Mappings/TransformMapping3D.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class WindyTextureSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_WindyTexture_Serialization()
      {
      intrusive_ptr<const Mapping3D> p_mapping(new TransformMapping3D());
      intrusive_ptr<Texture<double>> p_texture1(new WindyTexture<double>(p_mapping));

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_texture1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Texture<double>> p_texture2;

        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_texture2;
        } // archive and stream closed when destructors are called

      DifferentialGeometry dg;
      dg.m_point = Point3D_d(RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0);
      double val1 = p_texture1->Evaluate(dg, 0);
      double val2 = p_texture2->Evaluate(dg, 0);
      TS_ASSERT_EQUALS(val1, val2);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // WINDY_TEXTURE_SERIALIZATION_TEST_H
