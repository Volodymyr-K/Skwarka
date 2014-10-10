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

#ifndef MATRIX_4X4_SERIALIZATION_TEST_H
#define MATRIX_4X4_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Matrix4x4.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class Matrix4x4SerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    void test_Matrix_Serialization()
      {
      double elements[4][4]={
        1.0,1.1,1.2,1.3,
        -1.0,2.1,2.2,2.3,
        1.0,2.0,-2.0,2.0,
        2.5,2.5,-2.5,-2.5};

      Matrix4x4_d matrix1(elements);
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << matrix1;
        } // archive and stream closed when destructors are called

      Matrix4x4_d matrix2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> matrix2;
        } // archive and stream closed when destructors are called

      for(size_t i=0;i<4;++i)
        for(size_t j=0;j<4;++j)
          if (matrix1.m_values[i][j] != matrix2.m_values[i][j])
            {
            TS_FAIL("Matrix4x4 serialization test failed.");
            return;
            }
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // MATRIX_4X4_SERIALIZATION_TEST_H