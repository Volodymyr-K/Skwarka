#ifndef SCENE_SERIALIZATION_TEST_H
#define SCENE_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/Scene.h>
#include <Raytracer/Core/Primitive.h>
#include <Raytracer/Core/TriangleAccelerator.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Raytracer/Core/Intersection.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Materials/Matte.h>
#include <Raytracer/PhaseFunctions/RayleighPhaseFunction.h>
#include <Raytracer/VolumeRegions/HomogeneousVolumeRegion.h>
#include <UnitTests/TestHelpers/TriangleMeshTestHelper.h>
#include <vector>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class SceneSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Scene_Serialization()
      {
      std::vector<intrusive_ptr<const Primitive> > primitives;
      for(size_t i=0;i<10;++i)
        primitives.push_back(_CreateDummyPrimitive(Point3D_f(0.f,0.f,i*1.f)));

      LightSources light_sources;
      intrusive_ptr<DeltaLightSource> delta_light( new PointLight(Point3D_d(1.0,2.0,3.0), Spectrum_d(1.5,2.5,3.5)) );
      light_sources.m_delta_light_sources.push_back(delta_light);

      for(size_t i=0;i<primitives.size();++i)
        if (primitives[i]->GetAreaLightSource()!=NULL)
          light_sources.m_area_light_sources.push_back( primitives[i]->GetAreaLightSource() );

      BBox3D_d bounds = Convert<double>(primitives[0]->GetTriangleMesh()->GetBounds());
      for(size_t i=1;i<primitives.size();++i)
        bounds=Union(bounds, Convert<double>(primitives[i]->GetTriangleMesh()->GetBounds()) );

      intrusive_ptr<PhaseFunction> p_phase_function( new RayleighPhaseFunction );
      intrusive_ptr<VolumeRegion> p_volume( new HomogeneousVolumeRegion(bounds, Spectrum_d(1.0), Spectrum_d(0.1), Spectrum_d(0.1), p_phase_function) );
      intrusive_ptr<Scene> p_scene1(new Scene(primitives, p_volume, light_sources));

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_scene1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Scene> p_scene2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_scene2;
        } // archive and stream closed when destructors are called

      // Check primitives.
      if (p_scene1->GetPrimitives().size() != p_scene2->GetPrimitives().size())
        {
        TS_FAIL("Scene serialization test failed.");
        return;
        }
      for(size_t i=0;i<p_scene1->GetPrimitives().size();++i)
        if ((p_scene1->GetPrimitives()[i]->GetTriangleMesh()->GetArea() != p_scene2->GetPrimitives()[i]->GetTriangleMesh()->GetArea()) ||
            (p_scene1->GetPrimitives()[i]->GetAreaLightSource()->Power() != p_scene2->GetPrimitives()[i]->GetAreaLightSource()->Power()))
          {
          TS_FAIL("Scene serialization test failed.");
          return;
          }

      TS_ASSERT_EQUALS(p_scene1->GetVolumeRegion()->GetBounds().m_min, p_scene2->GetVolumeRegion()->GetBounds().m_min);
      TS_ASSERT_EQUALS(p_scene1->GetVolumeRegion()->GetBounds().m_max, p_scene2->GetVolumeRegion()->GetBounds().m_max);

      TS_ASSERT_EQUALS(p_scene1->GetLightSources().m_area_light_sources.size(), p_scene2->GetLightSources().m_area_light_sources.size());
      TS_ASSERT_EQUALS(p_scene1->GetLightSources().m_delta_light_sources.size(), p_scene2->GetLightSources().m_delta_light_sources.size());
      TS_ASSERT_EQUALS(p_scene1->GetLightSources().m_infinite_light_sources.size(), p_scene2->GetLightSources().m_infinite_light_sources.size());

      TS_ASSERT_EQUALS(p_scene1->GetLightSources().m_delta_light_sources[0]->Power(), p_scene2->GetLightSources().m_delta_light_sources[0]->Power());
      }

  private:
    intrusive_ptr<Primitive> _CreateDummyPrimitive(const Point3D_f &i_origin)
      {
      intrusive_ptr<TriangleMesh> p_mesh( TriangleMeshHelper::ConstructTetrahedron(i_origin) );
      intrusive_ptr<Texture<Spectrum_d> > p_reflectance( new ConstantTexture<Spectrum_d>(Spectrum_d(1.0)) );
      intrusive_ptr<Texture<double> > p_sigma( new ConstantTexture<double>(0.1) );
      intrusive_ptr<Material> p_material( new Matte(p_reflectance, p_sigma) );
      intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(Spectrum_d(1.0), p_mesh) );

      return intrusive_ptr<Primitive>( new Primitive(p_mesh, p_material, p_area_light, NULL) );
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // SCENE_SERIALIZATION_TEST_H
