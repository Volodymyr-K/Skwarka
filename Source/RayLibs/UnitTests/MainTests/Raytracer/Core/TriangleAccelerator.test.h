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

#ifndef TRIANGLE_ACCELERATOR_TEST_H
#define TRIANGLE_ACCELERATOR_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/TriangleAccelerator.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Raytracer/Core/Primitive.h>
#include <Raytracer/Core/Intersection.h>
#include <Raytracer/Core/CoreUtils.h>
#include "Mocks/MaterialMock.h"
#include <UnitTests/TestHelpers/TriangleMeshTestHelper.h>
#include <Math/RandomGenerator.h>
#include <vector>

class TriangleAcceleratorTestSuite : public CxxTest::TestSuite
  {
  public:
    TriangleAcceleratorTestSuite()
      {
      m_primitives.push_back( _CreatePrimitive(TriangleMeshHelper::ConstructSphere(Point3D_d(0,0,0), 10.0, 5)) );
      m_primitives.push_back( _CreatePrimitive(TriangleMeshHelper::ConstructSphere(Point3D_d(-20,0,0), 20.0, 5)) );
      m_primitives.push_back( _CreatePrimitive(TriangleMeshHelper::ConstructSphere(Point3D_d(0,-10,0), 0.5, 5)) );
      m_primitives.push_back( _CreatePrimitive(TriangleMeshHelper::ConstructSphere(Point3D_d(1000,0,0), 1000.0, 6)) );
      mp_triangle_accelerator.reset(new TriangleAccelerator(m_primitives));
      }

    void test_TriangleAccelerator_GetWorldBounds()
      {
      BBox3D_d bbox;

      for (size_t i=0;i<m_primitives.size();++i)
        {
        intrusive_ptr<const TriangleMesh> p_mesh = m_primitives[i]->GetTriangleMesh();
        for (size_t j=0;j<p_mesh->GetNumberOfTriangles();++j)
          {
          MeshTriangle triangle = p_mesh->GetTriangle(j);
          bbox.Unite(p_mesh->GetVertex(triangle.m_vertices[0]));
          bbox.Unite(p_mesh->GetVertex(triangle.m_vertices[1]));
          bbox.Unite(p_mesh->GetVertex(triangle.m_vertices[2]));
          }
        }

      BBox3D_d bbox2 = mp_triangle_accelerator->GetWorldBounds();
      TS_ASSERT_EQUALS(bbox.m_min, bbox2.m_min);
      TS_ASSERT_EQUALS(bbox.m_max, bbox2.m_max);
      }

    void test_TriangleAccelerator_Intersect()
      {
      size_t N = 1000;
      RandomGenerator<double> rg;

      BBox3D_d bbox = mp_triangle_accelerator->GetWorldBounds();

      for(size_t i=0;i<N;++i)
        {
        Point3D_d point(rg(bbox.m_min[0], bbox.m_max[0]), rg(bbox.m_min[1], bbox.m_max[1]), rg(bbox.m_min[2], bbox.m_max[2]));
        Vector3D_d dir(rg(1.0), rg(1.0), rg(1.0));
        Ray ray(point, dir.Normalized(), rg(-10,10), rg(100, 10000));

        Intersection isect;
        double t;
        bool hit = mp_triangle_accelerator->Intersect(RayDifferential(ray), isect, &t);

        double t2;
        size_t primitive_index, triangle_index;
        bool hit2 = _BruteForce(ray, primitive_index, triangle_index, t2);

        if (hit != hit2)
          {
          TS_FAIL("TriangleAccelerator::Intersect() test failed.");
          return;
          }

        if (hit && (isect.mp_primitive != m_primitives[primitive_index] || isect.m_triangle_index!=triangle_index || fabs(t-t2)>(1e-9) ))
          {
          TS_FAIL("TriangleAccelerator::Intersect() test failed.");
          return;
          }
        }
      }

    // Tests that outgoing rays do not intersect triangles at previous intersection position.
    void test_TriangleAccelerator_Epsilon()
      {
      size_t N1 = 1000, N2=1000;
      RandomGenerator<double> rg;

      BBox3D_d bbox = mp_triangle_accelerator->GetWorldBounds();

      for(size_t i=0;i<N1;++i)
        {
        Point3D_d point(rg(bbox.m_min[0], bbox.m_max[0]), rg(bbox.m_min[1], bbox.m_max[1]), rg(bbox.m_min[2], bbox.m_max[2]));
        Vector3D_d dir(rg(1.0), rg(1.0), rg(1.0));
        Ray ray(point, dir.Normalized(), rg(-10,10), rg(100, 10000));

        Intersection isect;
        double t;
        bool hit = mp_triangle_accelerator->Intersect(RayDifferential(ray), isect, &t);
        if (hit==false)
          continue;

        for(size_t j=0;j<N2;++j)
          {
          Ray ray2(isect.m_dg.m_point, Vector3D_d(rg(1.0), rg(1.0), rg(1.0)).Normalized());
          ray2.m_min_t = CoreUtils::GetNextMinT(isect, ray2.m_direction);

          Intersection isect2;
          hit = mp_triangle_accelerator->Intersect(RayDifferential(ray2), isect2, &t);
          if (hit && isect.mp_primitive==isect2.mp_primitive && isect.m_triangle_index==isect2.m_triangle_index)
            {
            TS_FAIL("Epsilon value is incorrect for outgoing ray.");
            return;
            }
          }

        }
      }

    void test_TriangleAccelerator_IntersectTest()
      {
      size_t N = 1000;
      RandomGenerator<double> rg;

      BBox3D_d bbox = mp_triangle_accelerator->GetWorldBounds();

      for(size_t i=0;i<N;++i)
        {
        Point3D_d point(rg(bbox.m_min[0], bbox.m_max[0]), rg(bbox.m_min[1], bbox.m_max[1]), rg(bbox.m_min[2], bbox.m_max[2]));
        Vector3D_d dir(rg(1.0), rg(1.0), rg(1.0));
        Ray ray(point, dir.Normalized(), rg(-10,10), rg(100, 10000));

        bool hit = mp_triangle_accelerator->IntersectTest(ray);

        double t2;
        size_t primitive_index, triangle_index;
        bool hit2 = _BruteForce(ray, primitive_index, triangle_index, t2);

        if (hit != hit2)
          {
          TS_FAIL("TriangleAccelerator::Intersect() test failed.");
          return;
          }
        }
      }

    void test_TriangleAccelerator_ObjectInstancing()
      {
      std::vector<intrusive_ptr<const Primitive>> primitives1, primitives2;
      RandomGenerator<double> rg;

      for(size_t i=0;i<10000;++i)
        {
        Transform transform;
        transform = MakeTranslation(Vector3D_d(rg(100), rg(100), rg(100)));
        transform = transform*MakeScale(rg(3.0)+0.01, rg(3.0)+0.01, rg(3.0)+0.01);
        transform = transform*MakeRotationY(rg(6.0))*MakeRotationZ(rg(6.0))*MakeRotationX(rg(6.0));

        primitives1.push_back( _CreatePrimitive(TriangleMeshHelper::ConstructTetrahedron(Point3D_f(0,0,0)), transform) );
        primitives2.push_back( _CreatePrimitive(TriangleMeshHelper::ConstructTetrahedron(Point3D_f(0,0,0), transform)) );
        }

      shared_ptr<TriangleAccelerator> p_triangle_accelerator1( new TriangleAccelerator(primitives1) );
      shared_ptr<TriangleAccelerator> p_triangle_accelerator2( new TriangleAccelerator(primitives2) );

      size_t N = 10000;
      for(size_t i=0;i<N;++i)
        {
        Point3D_d point(rg(0,100), rg(0,100), rg(0,100));
        Vector3D_d dir(rg(1.0), rg(1.0), rg(1.0));
        Ray ray(point, dir.Normalized());

        Intersection isect1, isect2;
        double t1,t2;
        bool hit1 = p_triangle_accelerator1->Intersect(RayDifferential(ray), isect1, &t1);
        bool hit2 = p_triangle_accelerator2->Intersect(RayDifferential(ray), isect2, &t2);

        TS_ASSERT_EQUALS(hit1, hit2);
        if (hit1 && hit2)
          {
          TS_ASSERT_DELTA(t1,t2,(1e-3));
          TS_ASSERT_EQUALS(isect1.m_triangle_index, isect2.m_triangle_index);
          CustomAssertDelta(isect1.m_dg.m_point, isect2.m_dg.m_point, (1e-3));
          }
        }
      }

  private:
    intrusive_ptr<Primitive> _CreatePrimitive(intrusive_ptr<TriangleMesh> ip_mesh, const Transform &i_transform = Transform()) const
      {
      intrusive_ptr<Material> p_material(new MaterialMock());
      intrusive_ptr<Primitive> p_primitive(new Primitive(ip_mesh, i_transform, p_material, NULL));
      return p_primitive;
      }

    bool _BruteForce(const Ray &i_ray, size_t &o_primitive_index, size_t &o_triangle_index, double &o_t) const
      {
      bool hit = false;
      o_t = DBL_MAX;
      
      for (size_t i=0;i<m_primitives.size();++i)
        {
        intrusive_ptr<const TriangleMesh> p_mesh = m_primitives[i]->GetTriangleMesh();
        for (size_t j=0;j<p_mesh->GetNumberOfTriangles();++j)
          {
          MeshTriangle triangle = p_mesh->GetTriangle(j);
          Triangle3D_f triangle3D(
            p_mesh->GetVertex(triangle.m_vertices[0]),
            p_mesh->GetVertex(triangle.m_vertices[1]),
            p_mesh->GetVertex(triangle.m_vertices[2]));
          
          double t = _IntersectTriangle(i_ray, triangle3D);
          if (t!=DBL_MAX && t>=i_ray.m_min_t && t<=i_ray.m_max_t && t<o_t)
            {
            o_t=t;
            o_primitive_index=i;
            o_triangle_index=j;
            hit=true;
            }
          }
        }

      return hit;
      }

    double _IntersectTriangle(const Ray &i_ray, const Triangle3D_f &i_triangle) const
      {
      Point3D_d v0 = Convert<double>(i_triangle[0]);
      Point3D_d v1 = Convert<double>(i_triangle[1]);
      Point3D_d v2 = Convert<double>(i_triangle[2]);

      Vector3D_d e1 = Vector3D_d(v1-v0);
      Vector3D_d e2 = Vector3D_d(v2-v0);
      Vector3D_d s1 = i_ray.m_direction^e2;
      double divisor = s1*e1;
      double inv_divisor = 1.0/divisor;

      // Compute first barycentric coordinate.
      Vector3D_d d = Vector3D_d(i_ray.m_origin - v0);
      double b1 = (d*s1) * inv_divisor;
      if(b1 < -DBL_EPS || b1 > (1.0+DBL_EPS) || divisor==0.0)
        return DBL_MAX;

      // Compute second barycentric coordinate.
      Vector3D_d s2 = d^e1;
      double b2 = (i_ray.m_direction*s2) * inv_divisor;
      if(b2 < -DBL_EPS || b1 + b2 > (1.0+DBL_EPS))
        return DBL_MAX;

      // Compute t to intersection point.
      return (e2*s2) * inv_divisor;
      }

  private:
    std::vector<intrusive_ptr<const Primitive>> m_primitives;
    shared_ptr<TriangleAccelerator> mp_triangle_accelerator;
  };

#endif // TRIANGLE_ACCELERATOR_TEST_H