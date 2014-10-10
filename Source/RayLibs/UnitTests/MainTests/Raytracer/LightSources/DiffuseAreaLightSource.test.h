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

#ifndef DIFFUSE_AREA_LIGHT_TEST_H
#define DIFFUSE_AREA_LIGHT_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/LightSources/DiffuseAreaLightSource.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Math/SamplingRoutines.h>
#include <UnitTests/TestHelpers/TriangleMeshTestHelper.h>
#include <vector>

class DiffuseAreaLightSourceTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      mp_mesh=TriangleMeshHelper::ConstructTetrahedron();

      mp_area_light.reset(new DiffuseAreaLightSource(Spectrum_d(1.0), mp_mesh));
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_DiffuseAreaLightSource_Radiance()
      {
      DifferentialGeometry dg;
      dg.m_geometric_normal=Vector3D_d(0,0,1);
      TS_ASSERT_EQUALS(mp_area_light->Radiance(dg,0,Vector3D_d(0,0,1)), Spectrum_d(1.0));

      dg.m_geometric_normal=Vector3D_d(0,0,-1);
      TS_ASSERT_EQUALS(mp_area_light->Radiance(dg,0,Vector3D_d(0,0,1)), Spectrum_d(0.0));
      }

    void test_DiffuseAreaLightSource_Power()
      {
      // This is the tetrahedron's area.
      double area=4*2.0/sqrt(3.0);
      Spectrum_d power = area*M_PI*Spectrum_d(1.0);

      CustomAssertDelta(mp_area_light->Power(), power, 1e-6);
      }

    // Tests that SampleLighting() method samples points on the mesh surface, that the returned PDF is not negative and that returned radiance is correct.
    void test_DiffuseAreaLightSource_SampleLighting()
      {
      size_t num_samples=1000;
      Point3D_d point(3.0,4.0,5.0);

      for(size_t i=0;i<num_samples;++i)
        {
        Ray lighting_ray;
        double pdf;
        Spectrum_d sampled_radiance = mp_area_light->SampleLighting(point, RandomDouble(1.0), Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), lighting_ray, pdf);
        
        if (lighting_ray.m_direction.IsNormalized()==false)
          {
          TS_FAIL("Ray direction is not normalized.");
          break;
          }

        if (pdf<0.0)
          {
          TS_FAIL("PDF value is negative.");
          break;
          }

        Point3D_d sampled_point = lighting_ray(lighting_ray.m_max_t);
        size_t triangle_index;
        if (_IsPointOnMeshSurface(sampled_point, triangle_index) == false)
          {
          TS_FAIL("Sampled point is not on the mesh surface.");
          break;
          }

        Vector3D_d normal = Convert<double>(mp_mesh->GetTriangleNormal(triangle_index));
        Spectrum_d correct_radiance = normal*lighting_ray.m_direction<0.0 ? Spectrum_d(1.0) : Spectrum_d(0.0);

        if (sampled_radiance != correct_radiance)
          {
          TS_FAIL("Sampled light is incorrect.");
          break;
          }
        }

      }

    // Tests that LightingPDF() method returns the same PDF as SampleLighting() method does.
    void test_DiffuseAreaLightSource_LightingPDF()
      {
      size_t num_samples=1000;
      Point3D_d point(3.0,4.0,5.0);

      for(size_t i=0;i<num_samples;++i)
        {
        Ray lighting_ray;
        double pdf;
        mp_area_light->SampleLighting(point, RandomDouble(1.0), Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), lighting_ray, pdf);

        Point3D_d sampled_point = lighting_ray(lighting_ray.m_max_t);
        size_t triangle_index;
        _IsPointOnMeshSurface(sampled_point, triangle_index);

        double pdf2 = mp_area_light->LightingPDF(lighting_ray, triangle_index);

        if (fabs(pdf-pdf2)>(1e-10))
          {
          TS_FAIL("PDF is incorrect.");
          break;
          }
        }
      }

    // Tests that PDF values integrate to one.
    void test_DiffuseAreaLightSource_LightingPDFSum()
      {
      size_t num_samples_x=300;
      size_t num_samples_y=300;
      Point3D_d point(1.5,2.0,1.2);

      std::vector<Point2D_d> samples(num_samples_x*num_samples_y);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),num_samples_x,num_samples_y,true);

      double sum=0.0;
      for(size_t i=0;i<num_samples_x*num_samples_y;++i)
        {
        Vector3D_d direction = SamplingRoutines::UniformSphereSampling(samples[i]);

        // Here we integrate not by all sphere directions but by all mesh triangles also.
        for(size_t triangle_index=0;triangle_index<mp_mesh->GetNumberOfTriangles();++triangle_index)
          {
          Point3D_d intersection;
          if (_ComputeIntersectionPoint(point, direction, triangle_index, intersection)==false)
            continue;

          Ray lighting_ray = Ray(point, Vector3D_d(intersection-point).Normalized(), 0.0, Vector3D_d(intersection-point).Length());
          double pdf = mp_area_light->LightingPDF(lighting_ray, triangle_index);
          sum += pdf * (4.0*M_PI)/(num_samples_x*num_samples_y);
          }
        }

      TS_ASSERT_DELTA(sum, 1.0, 0.02);
      }

    void test_DiffuseAreaLightSource_SamplePhoton()
      {
      size_t num_samples_sqrt=100, num_samples=num_samples_sqrt*num_samples_sqrt;
      std::vector<Point2D_d> samples(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),num_samples_sqrt,num_samples_sqrt,true);

      Spectrum_d sum;
      for(size_t i=0;i<num_samples;++i)
        {
        Ray photon_ray;
        double pdf;
        Spectrum_d sampled_irradiance = mp_area_light->SamplePhoton(RandomDouble(1.0), Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), photon_ray, pdf);

        if (pdf<0.0)
          {
          TS_FAIL("PDF value is negative.");
          return;
          }

        size_t triangle_index;
        if (_IsPointOnMeshSurface(photon_ray.m_origin, triangle_index) == false)
          {
          TS_FAIL("Sampled point is not on the mesh surface.");
          return;
          }

        Vector3D_d normal = Convert<double>(mp_mesh->GetTriangleNormal(triangle_index));
        if (normal*photon_ray.m_direction<0.0)
          {
          TS_FAIL("Photon ray direction is invalid.");
          return;
          }

        double cos_value = normal*photon_ray.m_direction;
        if (sampled_irradiance != Spectrum_d(1.0)*cos_value)
          {
          TS_FAIL("Sampled irradiance is incorrect.");
          return;
          }

        // Integrate the irradiance value to check it later.
        sum += sampled_irradiance / pdf;
        }

      // Check that the integrated irradiance is equal to the light's power.
      sum /= num_samples;
      Spectrum_d power = mp_area_light->Power();
      CustomAssertDelta(sum, power, 1e-5);
      }

  private:

    // This is a helper method that checks if the specified point is on the mesh surface.
    bool _IsPointOnMeshSurface(const Point3D_d &i_point, size_t &o_triangle_index)
      {
      for(size_t j=0;j<mp_mesh->GetNumberOfTriangles();++j)
        {
        MeshTriangle mesh_triangle = mp_mesh->GetTriangle(j);
        Point3D_d vertices[3] = {
          Convert<double>( mp_mesh->GetVertex(mesh_triangle.m_vertices[0]) ), 
          Convert<double>( mp_mesh->GetVertex(mesh_triangle.m_vertices[1]) ),
          Convert<double>( mp_mesh->GetVertex(mesh_triangle.m_vertices[2]) )
          };

        double area1=0.5*(Vector3D_d(i_point-vertices[0])^Vector3D_d(i_point-vertices[1])).Length();
        double area2=0.5*(Vector3D_d(i_point-vertices[1])^Vector3D_d(i_point-vertices[2])).Length();
        double area3=0.5*(Vector3D_d(i_point-vertices[2])^Vector3D_d(i_point-vertices[0])).Length();

        double triangle_area = 0.5*(Vector3D_d(vertices[0]-vertices[1])^Vector3D_d(vertices[0]-vertices[2])).Length();

        if (fabs(area1+area2+area3-triangle_area)<(1e-10))
          {
          o_triangle_index=j;
          return true;
          }
        }

      return false;
      }

    // This is a helper method that checks if the specified ray intersects the specified triangle.
    bool _ComputeIntersectionPoint(const Point3D_d &i_origin, const Vector3D_d &i_direction, size_t i_triangle_index, Point3D_d &o_intersection) const
      {
      MeshTriangle mesh_triangle = mp_mesh->GetTriangle(i_triangle_index);
      Point3D_d vertices[3] = {
        Convert<double>( mp_mesh->GetVertex(mesh_triangle.m_vertices[0]) ), 
        Convert<double>( mp_mesh->GetVertex(mesh_triangle.m_vertices[1]) ),
        Convert<double>( mp_mesh->GetVertex(mesh_triangle.m_vertices[2]) )
        };

      Vector3D_d e1 = Vector3D_d(vertices[1] - vertices[0]);
      Vector3D_d e2 = Vector3D_d(vertices[2] - vertices[0]);

      Vector3D_d s1 = i_direction^e2;
      double divisor = s1*e1;
      if(divisor == 0.0)
        return false;

      double invDivisor = 1.0 / divisor;

      // Compute first barycentric coordinate.
      Vector3D_d d = Vector3D_d(i_origin - vertices[0]);
      double b1 = (d*s1) * invDivisor;
      if (b1<0.0)
        return false;

      // Compute second barycentric coordinate.
      Vector3D_d s2 = d^e1;
      double b2 = (i_direction*s2) * invDivisor;
      if (b2<0.0 || b1+b2>1.0)
        return false;

      double t=(e2*s2) * invDivisor;
      if (t<0.0)
        return false;

      o_intersection = i_origin + i_direction*t;
      return true;
      }

  private:
    intrusive_ptr<AreaLightSource> mp_area_light;
    intrusive_ptr<TriangleMesh> mp_mesh;
  };

#endif // DIFFUSE_AREA_LIGHT_TEST_H
