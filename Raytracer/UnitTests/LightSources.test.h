#ifndef LIGHT_SOURCES_TEST_H
#define LIGHT_SOURCES_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/LightSources.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Math/SamplingRoutines.h>
#include <vector>

class LightSourcesTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      // Construct tetrahedron mesh.
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

      mp_mesh.reset(new TriangleMesh(vertices,triangles));

      mp_area_light.reset(new AreaLightSource(Spectrum_d(1.0), mp_mesh));
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_AreaLightSource_Radiance()
      {
      TS_ASSERT_EQUALS(mp_area_light->Radiance(Vector3D_d(0,0,1),Vector3D_d(0,0,1)), Spectrum_d(1.0));
      TS_ASSERT_EQUALS(mp_area_light->Radiance(Vector3D_d(0,0,1),Vector3D_d(0,0,-1)), Spectrum_d(0.0));
      }

    void test_AreaLightSource_Power()
      {
      // This is the tetrahedron's area.
      double area=4*2.0/sqrt(3.0);
      Spectrum_d power = area*M_PI*Spectrum_d(1.0);

      CustomAssertDelta(mp_area_light->Power(), power, 1e-6);
      }

    // Tests that SampleLighting() method samples points on the mesh surface, that the returned PDF is not negative and that returned radiance is correct.
    void test_AreaLightSource_SampleLighting()
      {
      size_t num_samples=1000;
      Point3D_d point(3.0,4.0,5.0);

      for(size_t i=0;i<num_samples;++i)
        {
        Vector3D_d lighting_vector;
        double pdf;
        Spectrum_d sampled_radiance = mp_area_light->SampleLighting(point, RandomDouble(1.0), Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), lighting_vector, pdf);

        if (pdf<0.0)
          {
          TS_FAIL("PDF value is negative.");
          break;
          }

        Point3D_d sampled_point = point+lighting_vector;
        size_t triangle_index;
        if (_IsPointOnMeshSurface(sampled_point, triangle_index) == false)
          {
          TS_FAIL("Sampled point is not on the mesh surface.");
          break;
          }

        Vector3D_d normal = Convert<double>(mp_mesh->GetTriangleNormal(triangle_index));
        Spectrum_d correct_radiance = normal*lighting_vector<0.0 ? Spectrum_d(1.0) : Spectrum_d(0.0);

        if (sampled_radiance != correct_radiance)
          {
          TS_FAIL("Sampled light is incorrect.");
          break;
          }
        }

      }

    // Tests that LightingPDF() method returns the same PDF as SampleLighting() method does.
    void test_AreaLightSource_LightingPDF()
      {
      size_t num_samples=1000;
      Point3D_d point(3.0,4.0,5.0);

      for(size_t i=0;i<num_samples;++i)
        {
        Vector3D_d lighting_vector;
        double pdf;
        mp_area_light->SampleLighting(point, RandomDouble(1.0), Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), lighting_vector, pdf);

        Point3D_d sampled_point = point+lighting_vector;
        size_t triangle_index;
        _IsPointOnMeshSurface(sampled_point, triangle_index);

        double pdf2 = mp_area_light->LightingPDF(lighting_vector, triangle_index);

        if (fabs(pdf-pdf2)>(1e-10))
          {
          TS_FAIL("PDF is incorrect.");
          break;
          }
        }
      }

    // Tests that PDF values integrate to one.
    void test_AreaLightSource_LightingPDFSum()
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

          double pdf = mp_area_light->LightingPDF(Vector3D_d(intersection-point), triangle_index);
          sum += pdf * (4.0*M_PI)/(num_samples_x*num_samples_y);
          }
        }

      TS_ASSERT_DELTA(sum, 1.0, 0.02);
      }

    void test_AreaLightSource_SamplePhoton()
      {
      size_t num_samples=10000;

      for(size_t i=0;i<num_samples;++i)
        {
        Ray photon_ray;
        double pdf;
        Spectrum_d sampled_radiance = mp_area_light->SamplePhoton(RandomDouble(1.0), Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), Point2D_d(RandomDouble(1.0), RandomDouble(1.0)), photon_ray, pdf);

        if (pdf<0.0)
          {
          TS_FAIL("PDF value is negative.");
          break;
          }

        size_t triangle_index;
        if (_IsPointOnMeshSurface(photon_ray.m_origin, triangle_index) == false)
          {
          TS_FAIL("Sampled point is not on the mesh surface.");
          break;
          }

        Vector3D_d normal = Convert<double>(mp_mesh->GetTriangleNormal(triangle_index));
        if (normal*photon_ray.m_direction<0.0)
          {
          TS_FAIL("Photon ray direction is invalid.");
          break;
          }

        if (sampled_radiance != Spectrum_d(1.0))
          {
          TS_FAIL("Sampled light is incorrect.");
          break;
          }

        }

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

#endif // LIGHT_SOURCES_TEST_H
