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

#ifndef POINT3D_SERIALIZATION_TEST_H
#define POINT3D_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Point3D.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class Point3DSerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    // Tests serialization with different extreme cases like infinity value, epsilon value and NaN.
    void test_Point3D_Serialization()
      {
      Point3D_d pd1(-0.2,DBL_INF, 123.0);
      Point3D_f pf1(0.f, FLT_EPS, 999.f);
      pf1[0]=std::numeric_limits<float>::quiet_NaN();

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << pd1;
        output_archive << pf1;
        } // archive and stream closed when destructors are called

      Point3D_d pd2;
      Point3D_f pf2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> pd2;
        input_archive >> pf2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(pd1, pd2);
      TS_ASSERT_EQUALS(pf1[1],pf2[1]);
      TS_ASSERT_EQUALS(pf1[2],pf2[2]);
      if (IsNaN(pf2[0])==false) TS_FAIL("NaN value serialization test failed.");
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // POINT3D_SERIALIZATION_TEST_H