#ifndef IMAGE_ENVIRONMENTAL_SERIALIZATION_LIGHT_TEST_H
#define IMAGE_ENVIRONMENTAL_SERIALIZATION_LIGHT_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Numerics.h>
#include <Raytracer/LightSources/ImageEnvironmentalLight.h>
#include <Math/ThreadSafeRandom.h>
#include <Math/SamplingRoutines.h>
#include <Math/MathRoutines.h>
#include <vector>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class ImageEnvironmentalLightSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_ImageEnvironmentalLight_Serialization()
      {
      size_t width = 45, height = 56;
      std::vector<std::vector<Spectrum_f> > image(height, std::vector<Spectrum_f>(width));
      for(size_t y=0;y<height;++y)
        for(size_t x=0;x<width;++x)
          image[y][x]=Spectrum_f((float)x,(float)y,(float)(x+y));

      BBox3D_d bbox = BBox3D_d(Point3D_d(0,0,0), Point3D_d(10,20,30));
      Transform light_to_world = MakeRotation(1.2, Vector3D_d(-1,2,-3).Normalized()); 
      intrusive_ptr<InfiniteLightSource> p_light1 (new ImageEnvironmentalLight(bbox, light_to_world, image, SpectrumCoef_d(1.5,2.0,2.5)));

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_light1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<InfiniteLightSource> p_light2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_light2;
        } // archive and stream closed when destructors are called

      // Test radiance;
      Vector3D_d dir = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();
      Spectrum_d radiance1 = p_light1->Radiance(RayDifferential(Ray(Point3D_d(), dir)));
      Spectrum_d radiance2 = p_light2->Radiance(RayDifferential(Ray(Point3D_d(), dir)));
      TS_ASSERT_EQUALS(radiance1, radiance2);

      // Test sampling with normal.
      Vector3D_d normal = Vector3D_d(RandomDouble(2.0)-1.0,RandomDouble(2.0)-1.0,RandomDouble(2.0)-1.0).Normalized();
      Vector3D_d dir1,dir2;
      double pdf1,pdf2;
      Point2D_d sample2D(RandomDouble(1.0), RandomDouble(1.0));
      radiance1 = p_light1->SampleLighting(normal, sample2D, dir1, pdf1);
      radiance2 = p_light2->SampleLighting(normal, sample2D, dir2, pdf2);
      TS_ASSERT_EQUALS(radiance1, radiance2);
      TS_ASSERT_EQUALS(dir1, dir2);
      TS_ASSERT_EQUALS(pdf1, pdf2);
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // IMAGE_ENVIRONMENTAL_SERIALIZATION_LIGHT_TEST_H
