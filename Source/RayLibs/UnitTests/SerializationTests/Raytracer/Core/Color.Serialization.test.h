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

#ifndef COLOR_SERIALIZATION_TEST_H
#define COLOR_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/Color.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class ColorSerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    // Tests serialization with different extreme cases like infinity value, epsilon value and NaN.
    void test_RGBColor_Serialization()
      {
      RGBColor_d cd1(-0.2,DBL_INF, 123.0);
      RGBColor_f cf1(0.f, FLT_EPS, 999.f);
      cf1[0]=std::numeric_limits<float>::quiet_NaN();

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << cd1;
        output_archive << cf1;
        } // archive and stream closed when destructors are called

      RGBColor_d cd2;
      RGBColor_f cf2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> cd2;
        input_archive >> cf2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(cd1[0], cd2[0]);
      TS_ASSERT_EQUALS(cd1[1], cd2[1]);
      TS_ASSERT_EQUALS(cd1[2], cd2[2]);

      TS_ASSERT_EQUALS(cf1[1],cf2[1]);
      TS_ASSERT_EQUALS(cf1[2],cf2[2]);
      if (IsNaN(cf2[0])==false) TS_FAIL("NaN value serialization test failed.");
      }

    // Tests serialization with different extreme cases like infinity value, epsilon value and NaN.
    void test_XYZColor_Serialization()
      {
      XYZColor_d cd1(-0.2,DBL_INF, 123.0);
      XYZColor_f cf1(0.f, FLT_EPS, 999.f);
      cf1[0]=std::numeric_limits<float>::quiet_NaN();

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << cd1;
        output_archive << cf1;
        } // archive and stream closed when destructors are called

      XYZColor_d cd2;
      XYZColor_f cf2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> cd2;
        input_archive >> cf2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(cd1[0], cd2[0]);
      TS_ASSERT_EQUALS(cd1[1], cd2[1]);
      TS_ASSERT_EQUALS(cd1[2], cd2[2]);

      TS_ASSERT_EQUALS(cf1[1],cf2[1]);
      TS_ASSERT_EQUALS(cf1[2],cf2[2]);
      if (IsNaN(cf2[0])==false) TS_FAIL("NaN value serialization test failed.");
      }

    void test_ColorSystem_Serialization()
      {
      ColorSystem cs1(Point2D_d(0.64, 0.33), Point2D_d(0.3, 0.6), Point2D_d(0.15, 0.06), Point2D_d(0.3127, 0.3290), 2.2);
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << cs1;
        } // archive and stream closed when destructors are called

      ColorSystem cs2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> cs2;
        } // archive and stream closed when destructors are called

      RGBColor_d rgb(0.1,0.2,0.3);
      XYZColor_d xyz1 = cs1.RGB_To_XYZ(rgb);
      XYZColor_d xyz2 = cs2.RGB_To_XYZ(rgb);

      TS_ASSERT_EQUALS(xyz1[0], xyz2[0]);
      TS_ASSERT_EQUALS(xyz1[1], xyz2[1]);
      TS_ASSERT_EQUALS(xyz1[2], xyz2[2]);

      RGBColor_d rgb1 = cs1.GammaEncode(rgb);
      RGBColor_d rgb2 = cs1.GammaEncode(rgb);

      TS_ASSERT_EQUALS(rgb1[0], rgb2[0]);
      TS_ASSERT_EQUALS(rgb1[1], rgb2[1]);
      TS_ASSERT_EQUALS(rgb1[2], rgb2[2]);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // COLOR_SERIALIZATION_TEST_H