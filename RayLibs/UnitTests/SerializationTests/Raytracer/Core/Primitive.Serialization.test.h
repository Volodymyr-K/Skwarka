#ifndef PRIMITIVE_SERIALIZATION_TEST_H
#define PRIMITIVE_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Primitive.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Materials/MatteMaterial.h>
#include <UnitTests/TestHelpers/TriangleMeshTestHelper.h>
#include <vector>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class PrimitiveSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_Primitive_Serialization()
      {
      intrusive_ptr<Texture<SpectrumCoef_d> > p_reflectance( new ConstantTexture<SpectrumCoef_d>(SpectrumCoef_d(1.0)) );
      intrusive_ptr<Texture<double> > p_sigma( new ConstantTexture<double>(0.1) );

      intrusive_ptr<TriangleMesh> p_mesh = TriangleMeshHelper::ConstructSphere(Point3D_d(1,2,3), 3.14, 5);
      intrusive_ptr<Material> p_material( new MatteMaterial(p_reflectance, p_sigma) );
      intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(Spectrum_d(1.0), p_mesh) );
      intrusive_ptr<Texture<double> > p_bump_map( new ConstantTexture<double>(1.0) );

      intrusive_ptr<Primitive> p_primitive1( new Primitive(p_mesh, p_material, p_area_light, p_bump_map) );

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_primitive1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Primitive> p_primitive2;

        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_primitive2;
        } // archive and stream closed when destructors are called

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(1,0,0);
      dg.m_tangent=Vector3D_d(0.0,1.0,0.0);
      TS_ASSERT_EQUALS(p_primitive1->GetTriangleMesh()->GetNumberOfVertices(), p_primitive2->GetTriangleMesh()->GetNumberOfVertices());
      TS_ASSERT_EQUALS(p_primitive1->GetAreaLightSource()->Power(), p_primitive2->GetAreaLightSource()->Power());
      TS_ASSERT_EQUALS(p_primitive1->GetMaterial()->GetBSDF(dg,0,pool)->GetComponentsNum(), p_primitive2->GetMaterial()->GetBSDF(dg,0,pool)->GetComponentsNum());
      TS_ASSERT_EQUALS(p_primitive1->GetBumpMap()->Evaluate(dg,0), p_primitive2->GetBumpMap()->Evaluate(dg,0));
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // PRIMITIVE_SERIALIZATION_TEST_H