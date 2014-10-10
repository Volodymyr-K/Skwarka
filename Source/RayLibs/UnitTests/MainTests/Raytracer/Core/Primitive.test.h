/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PRIMITIVE_TEST_H
#define PRIMITIVE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Primitive.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
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
      intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(Spectrum_d(1.0), p_mesh) );

      Transform transform = MakeScale(2,3,4)*MakeRotationZ(0.1);
      intrusive_ptr<Primitive> p_primitive( new Primitive(p_mesh, transform, p_material, p_area_light, NULL) );

      TS_ASSERT(p_primitive->GetMaterial()==p_material);
      TS_ASSERT(p_primitive->GetTriangleMesh()==p_mesh);
      TS_ASSERT(p_primitive->GetAreaLightSource()==p_area_light);

      Matrix4x4_d m1 = transform.GetMatrix();
      Matrix4x4_d m2 = p_primitive->GetMeshToWorldTransform().GetMatrix();
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          if (m1.m_values[i][j]!=m2.m_values[i][j])
            {
            TS_FAIL("Transform is incorrect.");
            return;
            }
      }

    // Tests BSDF with a constant bump map.
    void test_Primitive_BumpMap1()
      {
      intrusive_ptr<TriangleMesh> p_mesh( new TriangleMesh(std::vector<Point3D_f>(), std::vector<MeshTriangle>()) );
      intrusive_ptr<Material> p_material( new MaterialMock() );
      intrusive_ptr<Texture<double>> p_bump_map( new TextureMock<double>(1.0) );

      intrusive_ptr<Primitive> p_primitive( new Primitive(p_mesh, Transform(), p_material, NULL, p_bump_map) );

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      dg.m_point_dx=Point3D_d(1,0,0);
      dg.m_point_dy=Point3D_d(0,1,0);

      const BSDF *p_bsdf=p_primitive->GetBSDF(dg,0,pool);
      // Constant bump mapping should produce the same result as if no bump mapping was done at all.
      TS_ASSERT_EQUALS(p_bsdf->GetShadingNormal(),Vector3D_d(0.0,0.0,1.0));
      }

    void test_Primitive_Emittance()
      {
      intrusive_ptr<TriangleMesh> p_mesh( new TriangleMesh(std::vector<Point3D_f>(), std::vector<MeshTriangle>()) );
      intrusive_ptr<Material> p_material( new MaterialMock() );
      intrusive_ptr<AreaLightSource> p_area_light( new DiffuseAreaLightSource(Spectrum_d(1.0), p_mesh) );

      intrusive_ptr<Primitive> p_primitive( new Primitive(p_mesh, Transform(), p_material, p_area_light, NULL) );

      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);
      dg.m_point_dx=Point3D_d(1,0,0);
      dg.m_point_dy=Point3D_d(0,1,0);

      Spectrum_d em1 = p_primitive->SelfEmittance(dg, 0, Vector3D_d(0.5,0.5,0.5).Normalized());
      Spectrum_d em2 = p_primitive->SelfEmittance(dg, 0, Vector3D_d(0.5,0.5,-0.5).Normalized());

      TS_ASSERT_EQUALS(em1, Spectrum_d(1.0));
      TS_ASSERT_EQUALS(em2, Spectrum_d(0.0));
      }

  };

#endif // PRIMITIVE_TEST_H