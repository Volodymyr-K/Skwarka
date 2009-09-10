#ifndef SPHERE_TEST_H
#define SPHERE_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Shapes/Sphere.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <string>

class SphereTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_Sphere_BuildMesh()
      {
      Sphere sphere;
      sphere.SetParameter("center","0.0 0.0 0.0");
      sphere.SetParameter("radius","1.0");

      shared_ptr<TriangleMesh> p_mesh=sphere.BuildMesh();
      TS_ASSERT(p_mesh!=NULL);
      TS_ASSERT(sphere.GetErrors().empty());
      }

    void test_Sphere_MeshTopology()
      {
      Sphere sphere;
      sphere.SetParameter("center","0.0 0.0 0.0");
      sphere.SetParameter("radius","1.0");

      shared_ptr<TriangleMesh> p_mesh=sphere.BuildMesh();
      TopologyInfo info = p_mesh->GetTopologyInfo();
      TS_ASSERT(info.m_manifold);
      TS_ASSERT(info.m_solid);
      TS_ASSERT_EQUALS(info.m_number_of_patches,1);
      }

    void test_Sphere_MeshSize1()
      {
      Sphere sphere;
      sphere.SetParameter("center","0.0 0.0 0.0");
      sphere.SetParameter("radius","1.0");
      sphere.SetParameter("subdivisions","0");

      shared_ptr<TriangleMesh> p_mesh=sphere.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetNumberOfTriangles(), 4);
      }

    void test_Sphere_MeshSize2()
      {
      Sphere sphere;
      sphere.SetParameter("center","0.0 0.0 0.0");
      sphere.SetParameter("radius","1.0");
      sphere.SetParameter("subdivisions","4");

      shared_ptr<TriangleMesh> p_mesh=sphere.BuildMesh();
      TS_ASSERT_EQUALS(p_mesh->GetNumberOfTriangles(), 4*4*4*4*4);
      }

    void test_Sphere_NegativeRadius()
      {
      Sphere sphere;
      sphere.SetParameter("center","0.0 0.0 0.0");
      sphere.SetParameter("radius","-1.0");

      shared_ptr<TriangleMesh> p_mesh=sphere.BuildMesh();
      TS_ASSERT(p_mesh==NULL);
      TS_ASSERT(sphere.GetErrors().empty()==false);
      }

    void test_Sphere_NegativeSubdivisions()
      {
      Sphere sphere;
      sphere.SetParameter("center","0.0 0.0 0.0");
      sphere.SetParameter("radius","1.0");
      sphere.SetParameter("subdivisions","-1");

      shared_ptr<TriangleMesh> p_mesh=sphere.BuildMesh();
      TS_ASSERT(p_mesh==NULL);
      TS_ASSERT(sphere.GetErrors().empty()==false);
      }

  };

#endif // SPHERE_TEST_H
