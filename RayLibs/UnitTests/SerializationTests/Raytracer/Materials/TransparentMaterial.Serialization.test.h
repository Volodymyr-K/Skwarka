#ifndef TRANSPARENT_MATERIAL_SERIALIZATION_TEST_H
#define TRANSPARENT_MATERIAL_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/TransparentMaterial.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Math/ThreadSafeRandom.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class TransparentMaterialSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_TransparentMaterial_Serialization()
      {
      intrusive_ptr<Texture<SpectrumCoef_d>> p_reflectance_texture( new ConstantTexture<SpectrumCoef_d>(SpectrumCoef_d(0.9,0.8,0.7)) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_transmittance_texture( new ConstantTexture<SpectrumCoef_d>(SpectrumCoef_d(0.5,0.6,0.7)) );

      intrusive_ptr<Material> p_material1(new TransparentMaterial(p_reflectance_texture, p_transmittance_texture, 1.5));
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
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);

      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      Vector3D_d exitant1,exitant2;
      double pdf1,pdf2;
      BxDFType sampled_type1,sampled_type2;
      Point2D_d sample2D(RandomDouble(1.0), RandomDouble(1.0));
      double sample1D = RandomDouble(1.0);

      SpectrumCoef_d f1 = p_material1->GetBSDF(dg, 0, pool)->Sample(incident, exitant1, sample2D, sample1D, pdf1, sampled_type1, BSDF_ALL_TRANSMISSION);
      SpectrumCoef_d f2 = p_material2->GetBSDF(dg, 0, pool)->Sample(incident, exitant2, sample2D, sample1D, pdf2, sampled_type2, BSDF_ALL_TRANSMISSION);

      TS_ASSERT_EQUALS(f1,f2);
      TS_ASSERT_EQUALS(exitant1,exitant2);
      TS_ASSERT_EQUALS(pdf1,pdf2);
      TS_ASSERT_EQUALS(sampled_type1,sampled_type2);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // TRANSPARENT_MATERIAL_SERIALIZATION_TEST_H
