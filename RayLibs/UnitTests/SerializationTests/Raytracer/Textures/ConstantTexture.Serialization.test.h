#ifndef CONSTANT_TEXTURE_SERIALIZATION_TEST_H
#define CONSTANT_TEXTURE_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class ConstantTextureSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_ConstantTexture_Serialization()
      {
      intrusive_ptr<Texture<Spectrum_d> > p_texture1(new ConstantTexture<Spectrum_d>(Spectrum_d(1.0,2.0,3.0)) );

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_texture1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Texture<Spectrum_d> > p_texture2;

        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_texture2;
        } // archive and stream closed when destructors are called

      DifferentialGeometry dg; // No need to fill with any data for the constant texture.
      Spectrum_d spectrum1 = p_texture1->Evaluate(dg, 0);
      Spectrum_d spectrum2 = p_texture2->Evaluate(dg, 0);
      TS_ASSERT_EQUALS(spectrum1, spectrum2);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // CONSTANT_TEXTURE_SERIALIZATION_TEST_H
