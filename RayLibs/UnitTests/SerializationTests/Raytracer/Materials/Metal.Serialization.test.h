#ifndef METAL_SERIALIZATION_TEST_H
#define METAL_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/Metal.h>
#include <Raytracer/BxDFs/Microfacet.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/Core/Fresnel.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class MetalSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_Metal_Serialization()
      {
      Spectrum_d refreactive_index(0.41,1.15,1.18), absorption(4.2,2.66,2.5);
      intrusive_ptr<Texture<Spectrum_d> > p_refreactive_index( new ConstantTexture<Spectrum_d>(refreactive_index) );
      intrusive_ptr<Texture<Spectrum_d> > p_absorption( new ConstantTexture<Spectrum_d>(absorption) );
      intrusive_ptr<Texture<double> > p_roughness( new ConstantTexture<double>(1.0) );

      intrusive_ptr<Material> p_material1(new Metal(p_refreactive_index, p_absorption, p_roughness));
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

      Spectrum_d val1 = p_material1->GetBSDF(dg, 0, pool)->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());
      Spectrum_d val2 = p_material2->GetBSDF(dg, 0, pool)->Evaluate(Vector3D_d(0.5,0.0,1.0).Normalized(), Vector3D_d(-0.5,0.0,1.0).Normalized());
      TS_ASSERT_EQUALS(val1,val2);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // METAL_SERIALIZATION_TEST_H
