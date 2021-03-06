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

#ifndef CYLINDER_TEST_H
#define CYLINDER_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Shapes/Cylinder.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <string>

class CylinderTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_Cylinder_Build()
      {
      Cylinder cylinder;

      intrusive_ptr<TriangleMesh> p_mesh=cylinder.BuildMesh();
      TS_ASSERT(p_mesh!=NULL);
      TS_ASSERT(p_mesh->GetUseShadingNormals());
      }

    void test_Cylinder_MeshTopology()
      {
      Cylinder cylinder;

      intrusive_ptr<TriangleMesh> p_mesh=cylinder.BuildMesh();

      TopologyInfo info = p_mesh->GetTopologyInfo();
      TS_ASSERT(info.m_manifold);
      TS_ASSERT(info.m_solid==false);
      TS_ASSERT_EQUALS(info.m_number_of_patches,1);
      }

    void test_Cylinder_Area1()
      {
      Cylinder cylinder;

      intrusive_ptr<TriangleMesh> p_mesh=cylinder.BuildMesh();
      TS_ASSERT_DELTA(p_mesh->GetArea(), 2.0*M_PI, 1e-3);
      }

    void test_Cylinder_Area2()
      {
      Cylinder cylinder;
      cylinder.SetMaxPhi(M_PI);

      intrusive_ptr<TriangleMesh> p_mesh=cylinder.BuildMesh();
      TS_ASSERT_DELTA(p_mesh->GetArea(), 1.0*M_PI, 1e-3);
      }

    void test_Cylinder_Transformation()
      {
      Cylinder cylinder;
      cylinder.SetSubdivisions(1000);
      cylinder.SetTransformation(MakeTranslation(Vector3D_d(10,10,10))*MakeScale(1,1,0.1));

      intrusive_ptr<TriangleMesh> p_mesh=cylinder.BuildMesh();
      BBox3D_f bounds = p_mesh->GetBounds();
      CustomAssertDelta(bounds.m_min, Point3D_f(10.f,10.f,10.f)-Vector3D_f(1.f,1.f,0.0f), float(1e-5));
      CustomAssertDelta(bounds.m_max, Point3D_f(10.f,10.f,10.f)+Vector3D_f(1.f,1.f,0.1f), float(1e-5));
      }

    void test_Cylinder_MeshSize()
      {
      Cylinder cylinder;

      cylinder.SetSubdivisions(10);
      intrusive_ptr<TriangleMesh> p_mesh=cylinder.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetNumberOfTriangles(), 20);

      cylinder.SetSubdivisions(100);
      p_mesh=cylinder.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetNumberOfTriangles(), 200);
      }

    void test_Cylinder_InvertOrientation()
      {
      Cylinder cylinder;

      intrusive_ptr<TriangleMesh> p_mesh=cylinder.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetInvertNormals(), false);

      cylinder.SetInvertOrientation(true);
      p_mesh=cylinder.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetInvertNormals(), true);

      // Set transformation that inverts orientation.
      cylinder.SetTransformation(MakeScale(-1,1,1));
      p_mesh=cylinder.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetInvertNormals(), false);
      }
  };

#endif // CYLINDER_TEST_H