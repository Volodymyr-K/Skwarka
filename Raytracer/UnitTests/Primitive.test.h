#ifndef PRIMITIVE_TEST_H
#define PRIMITIVE_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Primitive.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Raytracer/Core/BSDF.h>
#include "Mocks/TextureMock.h"
#include "Mocks/MaterialMock.h"
#include <vector>

class PrimitiveTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Primitive_Constr()
      {
      intrusive_ptr<TriangleMesh> p_mesh( new TriangleMesh(std::vector<Point3D_f>(), std::vector<MeshTriangle>()) );
      intrusive_ptr<Material> p_material( new MaterialMock() );

      intrusive_ptr<Primitive> p_primitive( new Primitive(p_mesh, p_material, NULL) );

      TS_ASSERT(p_primitive->GetMaterial()==p_material);
      TS_ASSERT(p_primitive->GetTriangleMesh()==p_mesh);
      }

    // Tests BSDF with a constant bump map.
    void test_Primitive_GetBSDF()
      {
      intrusive_ptr<TriangleMesh> p_mesh( new TriangleMesh(std::vector<Point3D_f>(), std::vector<MeshTriangle>()) );
      intrusive_ptr<Material> p_material( new MaterialMock() );
      intrusive_ptr<Texture<double> > p_bump_map( new TextureMock<double>(1.0) );

      intrusive_ptr<Primitive> p_primitive( new Primitive(p_mesh, p_material, p_bump_map) );

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_point_dx=Point3D_d(1,0,0);
      dg.m_point_dy=Point3D_d(0,1,0);

      BSDF *p_bsdf=p_primitive->GetBSDF(dg,0,pool);
      TS_ASSERT_EQUALS(p_bsdf->Evaluate(Vector3D_d(0.0,0.0,1.0),Vector3D_d(0.0,0.0,1.0)), Spectrum_d(1.0)*INV_PI);
      }

  };

#endif // PRIMITIVE_TEST_H