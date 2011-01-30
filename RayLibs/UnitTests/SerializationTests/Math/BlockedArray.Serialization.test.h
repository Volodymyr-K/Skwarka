#ifndef BLOCKED_ARRAY_SERIALIZATION_TEST_H
#define BLOCKED_ARRAY_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/BlockedArray.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class BlockedArraySerializationTestSuite : public CxxTest::TestSuite
  { 
  public:

    void test_BlockedArray_Serialization()
      {
      size_t n=123,m=234;
      BlockedArray<double,3> *p_ba1 = new BlockedArray<double,3>(n,m);
      for(size_t i=0;i<n;++i)
        for(size_t j=0;j<m;++j)
          p_ba1->Get(i,j)=3*i+j;

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_ba1;
        } // archive and stream closed when destructors are called

      BlockedArray<double,3> *p_ba2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_ba2;
        } // archive and stream closed when destructors are called

      for(size_t i=0;i<n;++i)
        for(size_t j=0;j<m;++j)
          if (p_ba1->Get(i,j)!=p_ba2->Get(i,j))
            {
            TS_FAIL("BlockedArray serialization test failed.");
            return;
            }
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // BLOCKED_ARRAY_SERIALIZATION_TEST_H