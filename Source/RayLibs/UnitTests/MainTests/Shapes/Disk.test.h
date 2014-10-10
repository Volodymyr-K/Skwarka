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

#ifndef DISK_TEST_H
#define DISK_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Shapes/Disk.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <string>

class DiskTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_Disk_Build()
      {
      Disk disk;

      intrusive_ptr<TriangleMesh> p_mesh=disk.BuildMesh();
      TS_ASSERT(p_mesh!=NULL);
      TS_ASSERT(p_mesh->GetUseShadingNormals()==false);
      }

    void test_Disk_MeshTopology()
      {
      Disk disk;

      intrusive_ptr<TriangleMesh> p_mesh=disk.BuildMesh();

      TopologyInfo info = p_mesh->GetTopologyInfo();
      TS_ASSERT(info.m_manifold);
      TS_ASSERT(info.m_solid==false);
      TS_ASSERT_EQUALS(info.m_number_of_patches,1);
      }

    void test_Disk_Area1()
      {
      Disk disk;

      intrusive_ptr<TriangleMesh> p_mesh=disk.BuildMesh();
      TS_ASSERT_DELTA(p_mesh->GetArea(), M_PI, 1e-3);
      }

    void test_Disk_Area2()
      {
      Disk disk;
      disk.SetInnerRadius(0.3);
      disk.SetMaxPhi(M_PI);

      intrusive_ptr<TriangleMesh> p_mesh=disk.BuildMesh();
      TS_ASSERT_DELTA(p_mesh->GetArea(), 0.5*M_PI*(1.0*1.0-0.3*0.3), 1e-3);
      }

    void test_Disk_Transformation()
      {
      Disk disk;
      disk.SetSubdivisions(1000);
      disk.SetTransformation(MakeTranslation(Vector3D_d(10,10,10))*MakeScale(2,3,1.0));

      intrusive_ptr<TriangleMesh> p_mesh=disk.BuildMesh();
      BBox3D_f bounds = p_mesh->GetBounds();
      CustomAssertDelta(bounds.m_min, Point3D_f(10.f,10.f,10.f)-Vector3D_f(2.f,3.f,0.0f), float(1e-5));
      CustomAssertDelta(bounds.m_max, Point3D_f(10.f,10.f,10.f)+Vector3D_f(2.f,3.f,0.0f), float(1e-5));
      }

    void test_Disk_MeshSize()
      {
      Disk disk;

      disk.SetSubdivisions(10);
      disk.SetInnerRadius(0.0);
      intrusive_ptr<TriangleMesh> p_mesh=disk.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetNumberOfTriangles(), 10);

      disk.SetSubdivisions(100);
      disk.SetInnerRadius(0.1);
      p_mesh=disk.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetNumberOfTriangles(), 200);
      }

    void test_Disk_InvertOrientation()
      {
      Disk disk;

      intrusive_ptr<TriangleMesh> p_mesh=disk.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetInvertNormals(), false);

      disk.SetInvertOrientation(true);
      p_mesh=disk.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetInvertNormals(), true);

      // Set transformation that inverts orientation.
      disk.SetTransformation(MakeScale(-1,1,1));
      p_mesh=disk.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetInvertNormals(), false);
      }

  };

#endif // DISK_TEST_H