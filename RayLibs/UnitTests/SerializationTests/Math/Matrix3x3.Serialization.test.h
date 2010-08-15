#ifndef MATRIX_3X3_SERIALIZATION_TEST_H
#define MATRIX_3X3_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Matrix3x3.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class Matrix3x3SerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    void test_Matrix3x3_Serialization()
      {
      double elements[3][3]={
        1.0,1.1,1.2,
        -1.0,2.1,2.2,
        1.0,2.0,-2.0};

      Matrix3x3_d matrix1(elements);
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << matrix1;
        } // archive and stream closed when destructors are called

      Matrix3x3_d matrix2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> matrix2;
        } // archive and stream closed when destructors are called

      for(size_t i=0;i<3;++i)
        for(size_t j=0;j<3;++j)
          if (matrix1.m_values[i][j] != matrix2.m_values[i][j])
            {
            TS_FAIL("Matrix3x3 serialization test failed.");
            return;
            }
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // MATRIX_3X3_SERIALIZATION_TEST_H