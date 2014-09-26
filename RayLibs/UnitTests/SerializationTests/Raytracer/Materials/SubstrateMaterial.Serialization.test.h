#ifndef SUBSTRATE_MATERIAL_SERIALIZATION_TEST_H
#define SUBSTRATE_MATERIAL_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/SubstrateMaterial.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class SubstrateMaterialSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_SubstrateMaterial_Serialization_Isotropic()
      {
      SpectrumCoef_d diffuse_reflectance(0.4,0.5,0.6), specular_reflectance(0.5);
      intrusive_ptr<Texture<SpectrumCoef_d>> p_diffuse_reflectance( new ConstantTexture<SpectrumCoef_d>(diffuse_reflectance) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_specular_reflectance( new ConstantTexture<SpectrumCoef_d>(specular_reflectance) );
      intrusive_ptr<Texture<double>> p_roughness( new ConstantTexture<double>(0.025) );

      intrusive_ptr<Material> p_material1(new SubstrateMaterial(p_diffuse_reflectance, p_specular_reflectance, p_roughness));
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

      SpectrumCoef_d val1 = p_material1->GetBSDF(dg, 0, pool)->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());
      SpectrumCoef_d val2 = p_material2->GetBSDF(dg, 0, pool)->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());
      TS_ASSERT_EQUALS(val1,val2);
      }

    void test_SubstrateMaterial_Serialization_Anisotropic()
      {
      SpectrumCoef_d diffuse_reflectance(0.4,0.5,0.6), specular_reflectance(0.5);
      intrusive_ptr<Texture<SpectrumCoef_d>> p_diffuse_reflectance( new ConstantTexture<SpectrumCoef_d>(diffuse_reflectance) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_specular_reflectance( new ConstantTexture<SpectrumCoef_d>(specular_reflectance) );
      intrusive_ptr<Texture<double>> p_u_roughness( new ConstantTexture<double>(0.025) );
      intrusive_ptr<Texture<double>> p_v_roughness( new ConstantTexture<double>(0.125) );

      intrusive_ptr<Material> p_material1(new SubstrateMaterial(p_diffuse_reflectance, p_specular_reflectance, p_u_roughness, p_v_roughness));
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

      SpectrumCoef_d val1 = p_material1->GetBSDF(dg, 0, pool)->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());
      SpectrumCoef_d val2 = p_material2->GetBSDF(dg, 0, pool)->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());
      TS_ASSERT_EQUALS(val1,val2);
      }


  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // SUBSTRATE_MATERIAL_SERIALIZATION_TEST_H
