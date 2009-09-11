#ifndef TRIANGLE_MESH_TEST_H
#define TRIANGLE_MESH_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/TriangleMesh.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <vector>

class TriangleMeshTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_TriangleMesh_Constr()
      {
      shared_ptr<TriangleMesh> p_mesh=_ConstructTetrahedron();

      TS_ASSERT_EQUALS(p_mesh->GetNumberOfVertices(),4);
      TS_ASSERT_EQUALS(p_mesh->GetNumberOfTriangles(),4);
      }

    void test_TriangleMesh_EmptyMesh()
      {
      shared_ptr<TriangleMesh> p_mesh(new TriangleMesh(std::vector<Point3D_f>(),std::vector<MeshTriangle>()));
      TopologyInfo info = p_mesh->GetTopologyInfo();

      TS_ASSERT_EQUALS(p_mesh->GetNumberOfVertices(),0);
      TS_ASSERT_EQUALS(p_mesh->GetNumberOfTriangles(),0);
      TS_ASSERT(info.m_manifold && info.m_number_of_patches==0 && info.m_solid);
      }

    // Tests a case when the mesh is solid.
    void test_TriangleMesh_SolidTopologyInfo()
      {
      shared_ptr<TriangleMesh> p_mesh=_ConstructTetrahedron();
      TopologyInfo info = p_mesh->GetTopologyInfo();

      TS_ASSERT(info.m_manifold && info.m_number_of_patches==1 && info.m_solid);
      }

    // Tests a case when the mesh is not solid.
    void test_TriangleMesh_NonSolidTopologyInfo()
      {
      std::vector<Point3D_f> vertices(3);
      std::vector<MeshTriangle> triangles(1);

      vertices[0]=Point3D_f(0.f, 0.f, 1.f);
      vertices[1]=Point3D_f(0.f, 1.f, 0.f);
      vertices[2]=Point3D_f(1.f, 0.f, 0.f);

      triangles[0]=MeshTriangle(0,1,2);

      shared_ptr<TriangleMesh> p_mesh(new TriangleMesh(vertices,triangles));
      TopologyInfo info = p_mesh->GetTopologyInfo();

      TS_ASSERT(info.m_manifold && info.m_number_of_patches==1 && info.m_solid==false);
      }

    // Tests a case when two triangles share an edge but they are not consistently oriented.
    void test_TriangleMesh_NonManifoldTopologyInfo()
      {
      std::vector<Point3D_f> vertices(4);
      std::vector<MeshTriangle> triangles(2);

      vertices[0]=Point3D_f(0.f, 0.f, 1.f);
      vertices[1]=Point3D_f(0.f, 1.f, 0.f);
      vertices[2]=Point3D_f(1.f, 0.f, 0.f);
      vertices[3]=Point3D_f(1.f, 1.f, 1.f);

      triangles[0]=MeshTriangle(0,1,2);
      triangles[1]=MeshTriangle(0,1,3);

      shared_ptr<TriangleMesh> p_mesh(new TriangleMesh(vertices,triangles));
      TopologyInfo info = p_mesh->GetTopologyInfo();

      TS_ASSERT(info.m_manifold==false && info.m_number_of_patches==2 && info.m_solid==false);
      }

    // Tests that geometric normals (not shaded ones) are correct.
    void test_TriangleMesh_TriangleNormals()
      {
      shared_ptr<TriangleMesh> p_mesh=_ConstructTetrahedron();
      bool normals_correct=true;

      for(size_t i=0;i<p_mesh->GetNumberOfTriangles();++i)
        {
        MeshTriangle triangle = p_mesh->GetTriangle(i);
        Point3D_f vertices[3] = {
          p_mesh->GetVertex(triangle.m_vertices[0]),
          p_mesh->GetVertex(triangle.m_vertices[1]),
          p_mesh->GetVertex(triangle.m_vertices[2])};

        Vector3D_f normal = (Vector3D_f(vertices[1]-vertices[0])^Vector3D_f(vertices[2]-vertices[0])).Normalized();
        Vector3D_f normal2 = p_mesh->GetTriangleNormal(i);
        if ((normal-normal2).Length()>(1e-6)) normals_correct=false;
        }
      TS_ASSERT(normals_correct);
      }

    // Tests DifferentialGeometry() method for a case when UV parameterization is not set, shading normals are not used and the ray has no differentials.
    void test_TriangleMesh_DifferentialGeometry1()
      {
      shared_ptr<TriangleMesh> p_mesh=_ConstructTetrahedron();
      p_mesh->SetUseShadingNormals(false);
      
      Point3D_d origin(0.5,-0.5,-10.0);
      Vector3D_d direction = Vector3D_d(Point3D_d(0.1,0.2,-1.0/3.0) - origin).Normalized();
      RayDifferential ray(Ray(origin,direction));

      DifferentialGeometry dg;
      p_mesh->ComputeDifferentialGeometry(0,ray,dg);

      // Test that the ray hit the correct point.
      CustomAssertDelta(dg.m_point, Point3D_d(0.1,0.2,-1.0/3.0), (1e-6));

      // Test that the geometric normal is correct.
      CustomAssertDelta(dg.m_geometric_normal, Convert<double>(p_mesh->GetTriangleNormal(0)), (1e-6));

      // Test that the shading normal is the same as the geometric one, since we turned shading normals off.
      CustomAssertDelta(dg.m_shading_normal, Convert<double>(p_mesh->GetTriangleNormal(0)), (1e-6));

      TS_ASSERT_EQUALS(dg.m_dn_du, Vector3D_d());
      TS_ASSERT_EQUALS(dg.m_dn_dv, Vector3D_d());
      TS_ASSERT_EQUALS(dg.m_dp_dx, Vector3D_d());
      TS_ASSERT_EQUALS(dg.m_dp_dy, Vector3D_d());
      TS_ASSERT_EQUALS(dg.m_duv_dx, Vector2D_d());
      TS_ASSERT_EQUALS(dg.m_duv_dy, Vector2D_d());
      }

    // Tests DifferentialGeometry() method for a case when UV parameterization is not set, shading normals are used and the ray has no differentials.
    void test_TriangleMesh_DifferentialGeometry2()
      {
      shared_ptr<TriangleMesh> p_mesh=_ConstructTetrahedron();
      p_mesh->SetUseShadingNormals(true);

      Point3D_d origin(0.5,-0.5,-10.0);
      Vector3D_d direction = Vector3D_d(Point3D_d(0.1,0.2,-1.0/3.0) - origin).Normalized();
      RayDifferential ray(Ray(origin,direction));

      DifferentialGeometry dg;
      p_mesh->ComputeDifferentialGeometry(0,ray,dg);

      // Test that the shading normal has unit length.
      TS_ASSERT_DELTA(dg.m_shading_normal.Length(),1.0,(1e-10));

      // Test that the shading normal lies in the same hemisphere as the geometric one.
      TS_ASSERT(dg.m_shading_normal*dg.m_geometric_normal > 0.0);

      // Test that the normal derivatives are not nulls.
      TS_ASSERT(dg.m_dn_du.Length() > 0.0);
      TS_ASSERT(dg.m_dn_dv.Length() > 0.0);

      TS_ASSERT_EQUALS(dg.m_dp_dx, Vector3D_d());
      TS_ASSERT_EQUALS(dg.m_dp_dy, Vector3D_d());
      TS_ASSERT_EQUALS(dg.m_duv_dx, Vector2D_d());
      TS_ASSERT_EQUALS(dg.m_duv_dy, Vector2D_d());
      }

    // Tests DifferentialGeometry() method for a case when UV parameterization is not set, shading normals are used and the ray has differentials.
    void test_TriangleMesh_DifferentialGeometry3()
      {
      shared_ptr<TriangleMesh> p_mesh=_ConstructTetrahedron();
      p_mesh->SetUseShadingNormals(true);

      Point3D_d origin(0.5,-0.5,-10.0);
      Vector3D_d direction = Vector3D_d(Point3D_d(0.1,0.2,-1.0/3.0) - origin).Normalized();
      RayDifferential ray(Ray(origin,direction));
      ray.m_has_differentials=true;

      // Set dx ray differentials.
      ray.m_origin_dx=origin+Point3D_d(0.01,0.02,0.03);
      ray.m_direction_dx=Vector3D_d(Point3D_d(0.15,0.25,-1.0/3.0) - ray.m_origin_dx).Normalized();

      // Set dy ray differentials.
      ray.m_origin_dy=origin+Point3D_d(0.03,0.02,0.01);
      ray.m_direction_dy=Vector3D_d(Point3D_d(0.05,0.15,-1.0/3.0) - ray.m_origin_dy).Normalized();

      DifferentialGeometry dg;
      p_mesh->ComputeDifferentialGeometry(0,ray,dg);

      CustomAssertDelta(dg.m_dp_dx, Vector3D_d(0.05,0.05,0.0), (1e-10));
      CustomAssertDelta(dg.m_dp_dy, Vector3D_d(-0.05,-0.05,0.0), (1e-10));

      // Test that the UV derivatives are not nulls.
      TS_ASSERT(dg.m_duv_dx.Length() > 0.0);
      TS_ASSERT(dg.m_duv_dy.Length() > 0.0);
      }

    // Tests shading normal by evaluating DifferentialGeometry at a triangle's vertex.
    void test_TriangleMesh_ShadingNormal()
      {
      shared_ptr<TriangleMesh> p_mesh=_ConstructTetrahedron();
      p_mesh->SetUseShadingNormals(true);

      Point3D_d origin(0.5,-0.5,-10.0);
      Vector3D_d direction = Vector3D_d(Convert<double>(p_mesh->GetVertex(1)) - origin).Normalized();
      RayDifferential ray(Ray(origin,direction));

      DifferentialGeometry dg;
      p_mesh->ComputeDifferentialGeometry(0,ray,dg);

      // Since we hit exactly the triangle's vertex and all the triangles are isosceles, the shaded normal should be a normalized sum of the incident triangle's normals.
      Vector3D_d theoretical_shading_normal = Convert<double>(p_mesh->GetTriangleNormal(0)+p_mesh->GetTriangleNormal(1)+p_mesh->GetTriangleNormal(2));
      theoretical_shading_normal.Normalize();
      
      CustomAssertDelta(dg.m_shading_normal, theoretical_shading_normal, (1e-6));
      }

  private:
    shared_ptr<TriangleMesh> _ConstructTetrahedron()
      {
      std::vector<Point3D_f> vertices(4);
      std::vector<MeshTriangle> triangles(4);

      double base_radius = sqrt(8.0)/3.0;
      vertices[0]=Point3D_f(0.f, 0.f, 1.f);
      vertices[1]=Point3D_f((float) base_radius,  0.f, -1.f/3.f);
      vertices[2]=Point3D_f((float) (base_radius*cos(2.0*M_PI_3)), (float) ( base_radius*sin(2.0*M_PI_3)), -1.f/3.f);
      vertices[3]=Point3D_f((float) (base_radius*cos(2.0*M_PI_3)), (float) (-base_radius*sin(2.0*M_PI_3)), -1.f/3.f);

      triangles[0]=MeshTriangle(1,3,2);
      triangles[1]=MeshTriangle(1,0,3);
      triangles[2]=MeshTriangle(1,2,0);
      triangles[3]=MeshTriangle(0,2,3);

      return shared_ptr<TriangleMesh>(new TriangleMesh(vertices,triangles));
      }
  };

#endif // TRIANGLE_MESH_TEST_H