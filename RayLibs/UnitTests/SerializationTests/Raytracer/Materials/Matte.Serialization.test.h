#ifndef MATTE_SERIALIZATION_TEST_H
#define MATTE_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/Matte.h>
#include <Raytracer/BxDFs/OrenNayar.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class MatteSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_Matte_Serialization()
      {
      intrusive_ptr<Texture<Spectrum_d> > p_reflectance( new ConstantTexture<Spectrum_d>(Spectrum_d(1.0,0.5,0.1)) );
      intrusive_ptr<ConstantTexture<double> > p_sigma( new ConstantTexture<double>(0.2) );

      intrusive_ptr<Material> p_material1(new Matte(p_reflectance, p_sigma));

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_material1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Material> p_material2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_material2;
        } // archive and stream closed when destructors are called

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);

      Spectrum_d val1 = p_material1->GetBSDF(dg, 0, pool)->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));
      Spectrum_d val2 = p_material2->GetBSDF(dg, 0, pool)->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));
      TS_ASSERT_EQUALS(val1,val2);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // MATTE_SERIALIZATION_TEST_H