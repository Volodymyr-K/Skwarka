#ifndef MERL_MEASURED_DATA_SERIALIZATION_TEST_H
#define MERL_MEASURED_DATA_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/BxDFs/MERLMeasured.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <fstream>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class MERLMeasuredDataSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_MERLMeasuredData_Serialization()
      {
      intrusive_ptr<MERLMeasuredData> p_tungsten_carbide_data1 = _LoadMERLDataFromFile("TestData/MERLMeasured/tungsten-carbide.binary");
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_tungsten_carbide_data1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<MERLMeasuredData> p_tungsten_carbide_data2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_tungsten_carbide_data2;
        } // archive and stream closed when destructors are called

      Vector3D_d exitant1, exitant2;
      double pdf1,pdf2;
      SpectrumCoef_d sp1=p_tungsten_carbide_data1->Sample(Vector3D_d(1,2,3).Normalized(), exitant1, Point2D_d(0.345,0.876), pdf1);
      SpectrumCoef_d sp2=p_tungsten_carbide_data2->Sample(Vector3D_d(1,2,3).Normalized(), exitant2, Point2D_d(0.345,0.876), pdf2);
      TS_ASSERT_EQUALS(sp1,sp2);
      TS_ASSERT_EQUALS(exitant1,exitant2);
      TS_ASSERT_EQUALS(pdf1,pdf2);
      }

  private:
    intrusive_ptr<MERLMeasuredData> _LoadMERLDataFromFile(std::string i_filename) const
      {
      std::ifstream file(i_filename.c_str(), std::ios::in | std::ios::binary);
      return intrusive_ptr<MERLMeasuredData>( new MERLMeasuredData(file) );
      }

  private:
    const static size_t m_buffer_size=32*1024*1024; // 32Mb should be more than enough
    char m_data[m_buffer_size];
  };

#endif // MERL_MEASURED_DATA_SERIALIZATION_TEST_H
