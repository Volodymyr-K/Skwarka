#ifndef SCENE_TEST_H
#define SCENE_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/Scene.h>
#include <Raytracer/Core/Primitive.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Raytracer/Core/Intersection.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include "Mocks/MaterialMock.h"
#include "TriangleMeshTestHelper.h"
#include <vector>

class SceneTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      m_primitives.clear();
      for(size_t i=0;i<10;++i)
        m_primitives.push_back(_CreateDummyPrimitive(Point3D_f(0.f,0.f,i*1.f)));

      m_light_sources.m_delta_light_sources.clear();
      m_light_sources.m_infinitiy_light_sources.clear();
      m_light_sources.m_area_light_sources.clear();

      intrusive_ptr<DeltaLightSource> delta_light( new PointLight(Point3D_d(1.0,2.0,3.0), Spectrum_d(1.5,2.5,3.5)) );
      m_light_sources.m_delta_light_sources.push_back(delta_light);

      for(size_t i=0;i<m_primitives.size();++i)
        if (m_primitives[i]->GetAreaLightSource()!=NULL)
          m_light_sources.m_area_light_sources.push_back( m_primitives[i]->GetAreaLightSource() );

      mp_scene.reset(new Scene(m_primitives, m_light_sources));
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_Scene_GetPrimitives()
      {
      TS_ASSERT(mp_scene->GetPrimitives() == m_primitives);
      }

    void test_Scene_GetWorldBounds()
      {
      BBox3D_d bounds = Convert<double>(m_primitives[0]->GetTriangleMesh()->GetBounds());
      for(size_t i=1;i<m_primitives.size();++i)
        bounds=Union(bounds, Convert<double>(m_primitives[i]->GetTriangleMesh()->GetBounds()) );

      TS_ASSERT_EQUALS(mp_scene->GetWorldBounds().m_min, bounds.m_min);
      TS_ASSERT_EQUALS(mp_scene->GetWorldBounds().m_max, bounds.m_max);
      }

    void test_Scene_Intersect()
      {
      RayDifferential rd( Ray(Point3D_d(0.0,0.0,-1.0), Vector3D_d(0.1,0.1,1.0)) );
      TriangleTree tree(m_primitives);

      Intersection isect1,isect2;
      bool t1 = mp_scene->Intersect(rd, isect1);     
      bool t2 = tree.Intersect(rd,isect2);

      TS_ASSERT(t1 == t2);
      TS_ASSERT(isect1.mp_primitive == isect2.mp_primitive);
      TS_ASSERT(isect1.m_triangle_index == isect2.m_triangle_index);
      TS_ASSERT(isect1.m_dg.m_point == isect2.m_dg.m_point);
      }

    void test_Scene_IntersectTest1()
      {
      Ray r = Ray(Point3D_d(0.0,0.0,-1.0), Vector3D_d(0.1,0.1,1.0));

      bool intersected = mp_scene->IntersectTest(r);     
      TS_ASSERT(intersected);
      }

    void test_Scene_IntersectTest2()
      {
      Ray r = Ray(Point3D_d(0.0,0.0,-1.0), Vector3D_d(2.1,2.1,1.0));

      bool intersected = mp_scene->IntersectTest(r);     
      TS_ASSERT(intersected == false);
      }

  private:
    intrusive_ptr<Primitive> _CreateDummyPrimitive(const Point3D_f &i_origin)
      {
      intrusive_ptr<TriangleMesh> p_mesh( TriangleMeshHelper::ConstructTetrahedron(i_origin) );
      intrusive_ptr<Material> p_material( new MaterialMock() );
      intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(Spectrum_d(1.0), p_mesh) );

      return intrusive_ptr<Primitive>( new Primitive(p_mesh, p_material, p_area_light, NULL) );
      }

  private:
    std::vector<intrusive_ptr<Primitive> > m_primitives;
    LightSources m_light_sources;

    intrusive_ptr<Scene> mp_scene;
  };

#endif // SCENE_TEST_H
