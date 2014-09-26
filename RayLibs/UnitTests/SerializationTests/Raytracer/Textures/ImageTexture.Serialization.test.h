#ifndef IMAGE_TEXTURE_SERIALIZATION_TEST_H
#define IMAGE_TEXTURE_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Textures/ImageTexture.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Mappings/UVMapping2D.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class ImageTextureSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_ImageTexture_Serialization()
      {
      std::vector<std::vector<Spectrum_f>> image;
      _CreateRedBueCheckerboardImage(100,90,image);

      intrusive_ptr<Mapping2D> p_mapping( new UVMapping2D() );
      intrusive_ptr<Texture<Spectrum_d>> p_texture1(new ImageTexture<Spectrum_f,Spectrum_d>(image, p_mapping) );

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_texture1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Texture<Spectrum_d>> p_texture2;

        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_texture2;
        } // archive and stream closed when destructors are called

      DifferentialGeometry dg;
      dg.m_duv_dx=Vector2D_d(0.02,0.0);
      dg.m_duv_dy=Vector2D_d(0.0,0.02);
      
      for(double u=0;u<=1.0;u+=0.1)
        for(double v=0;v<=1.0;v+=0.1)
          {
          dg.m_uv=Point2D_d(u,v);
          Spectrum_d spectrum1 = p_texture1->Evaluate(dg, 0);
          Spectrum_d spectrum2 = p_texture2->Evaluate(dg, 0);
          if (spectrum1!=spectrum2)
            {
            TS_FAIL("ImageTexture serialization test failed.");
            return;
            }
          }
      }

  private:
    void _CreateRedBueCheckerboardImage(size_t i_width, size_t i_height, std::vector<std::vector<Spectrum_f>> &o_image)
      {
      o_image.assign(i_height,std::vector<Spectrum_f>(i_width,Spectrum_f()));

      for(size_t i=0;i<i_width;++i)
        for(size_t j=0;j<i_height;++j)
          if ((i+j)%2)
            o_image[j][i]=Spectrum_f(1,0,0);
          else
            o_image[j][i]=Spectrum_f(0,0,1);
      }

  private:
    const static size_t m_buffer_size=10*1024*1024;
    char m_data[m_buffer_size];
  };

#endif // IMAGE_TEXTURE_SERIALIZATION_TEST_H
