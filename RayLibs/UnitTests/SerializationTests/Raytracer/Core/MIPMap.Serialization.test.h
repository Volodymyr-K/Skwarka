#ifndef MIP_MAP_SERIALIZATION_TEST_H
#define MIP_MAP_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/MIPMap.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/ImageSources/RGB24ImageSource.h>
#include <vector>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class MIPMapSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    // Tests serialization for MIPMap created from std::vector<std::vector<Spectrum_d> >
    void test_MIPMap_Serialization_FromVector()
      {
      intrusive_ptr<MIPMap<Spectrum_d> > p_map1 ( _CreateBlueRedMIPMapFromVector(123,234,true,8.0) );

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_map1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<MIPMap<Spectrum_d> > p_map2;

        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_map2;
        } // archive and stream closed when destructors are called

      if (_CompareMaps(p_map1, p_map2)==false)
        TS_FAIL("MIPMap serialization test failed.");
      }

    // Tests serialization for MIPMap created from ImageSource
    void test_MIPMap_Serialization_FromImageSource()
      {
      intrusive_ptr<MIPMap<Spectrum_d> > p_map1 ( _CreateBlueRedMIPMapFromImageSource(123,234,true,8.0) );

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_map1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<MIPMap<Spectrum_d> > p_map2;

        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_map2;
        } // archive and stream closed when destructors are called

      if (_CompareMaps(p_map1, p_map2)==false)
        TS_FAIL("MIPMap serialization test failed.");
      }

  private:
    intrusive_ptr<MIPMap<Spectrum_d> > _CreateBlueRedMIPMapFromVector(size_t i_width, size_t i_height, bool i_repeat, double i_max_anisotropy)
      {
      std::vector<std::vector<Spectrum_d> > image(i_height,std::vector<Spectrum_d>(i_width));

      for(size_t i=0;i<i_width;++i)
        for(size_t j=0;j<i_height;++j)
          if (i<i_width/2)
            image[j][i]=Spectrum_d(1.0,0.0,0.0);
          else
            image[j][i]=Spectrum_d(0.0,0.0,1.0);

      return intrusive_ptr<MIPMap<Spectrum_d> > (new MIPMap<Spectrum_d>(image,i_repeat,i_max_anisotropy));
      }

    intrusive_ptr<MIPMap<Spectrum_d> > _CreateBlueRedMIPMapFromImageSource(size_t i_width, size_t i_height, bool i_repeat, double i_max_anisotropy)
      {
      std::vector<std::vector<RGB24> > image(i_height,std::vector<RGB24>(i_width));

      for(size_t i=0;i<i_width;++i)
        for(size_t j=0;j<i_height;++j)
          if (i<i_width/2)
            {
            image[j][i].m_rgb[0]=255;
            image[j][i].m_rgb[1]=0;
            image[j][i].m_rgb[2]=0;
            }
          else
            {
            image[j][i].m_rgb[0]=0;
            image[j][i].m_rgb[1]=0;
            image[j][i].m_rgb[2]=255;
            }

      intrusive_ptr<ImageSource<Spectrum_d> > p_image_source( new RGB24ImageSource<Spectrum_d>(image, global_sRGB_E_ColorSystem) );
      return intrusive_ptr<MIPMap<Spectrum_d> > (new MIPMap<Spectrum_d>(p_image_source,i_repeat,i_max_anisotropy));
      }

    bool _CompareMaps(intrusive_ptr<MIPMap<Spectrum_d> > ip_map1, intrusive_ptr<MIPMap<Spectrum_d> >ip_map2)
      {
      for(double u=0;u<=1.0;u+=0.1)
        for(double v=0;v<=1.0;v+=0.1)
          {
          Spectrum_d t1 = ip_map1->Evaluate(Point2D_d(u,v), Vector2D_d(0.02,0.0), Vector2D_d(0.0,0.2));
          Spectrum_d t2 = ip_map2->Evaluate(Point2D_d(u,v), Vector2D_d(0.02,0.0), Vector2D_d(0.0,0.2));
          if (t1!=t2)
            return false;
          }

      return true;
      }

  private:
    const static size_t m_buffer_size=10*1024*1024;
    char m_data[m_buffer_size];
  };

#endif // MIP_MAP_SERIALIZATION_TEST_H
