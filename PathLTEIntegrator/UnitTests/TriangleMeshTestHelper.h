#ifndef TRIANGLE_MESH_TEST_HELPER_H
#define TRIANGLE_MESH_TEST_HELPER_H

/*
This file contains various helper functions for tests that deal with triangle meshes.
These functions are called from different test suites so change them carefully.
*/

#include <Common/Common.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Shapes/Sphere.h>
#include <sstream>
#include <string>

namespace TriangleMeshHelper
  {
  /*
  This method creates a tetrahedron triangle mesh inscribed in a unit sphere centered at the specified point.
  */
  intrusive_ptr<TriangleMesh> ConstructTetrahedron(const Point3D_f &i_origin = Point3D_f());

  /*
  This method creates a sphere triangle mesh with the specified center and radius.
  */
  intrusive_ptr<TriangleMesh> ConstructSphere(const Point3D_d &i_center, double i_radius, size_t i_subdivisions = 6);
  }

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace TriangleMeshHelper
  {

  inline intrusive_ptr<TriangleMesh> ConstructTetrahedron(const Point3D_f &i_origin)
    {
    std::vector<Point3D_f> vertices(4);
    std::vector<MeshTriangle> triangles(4);

    double base_radius = sqrt(8.0)/3.0;
    vertices[0]=i_origin+Point3D_f(0.f, 0.f, 1.f);
    vertices[1]=i_origin+Point3D_f((float) base_radius,  0.f, -1.f/3.f);
    vertices[2]=i_origin+Point3D_f((float) (base_radius*cos(2.0*M_PI_3)), (float) ( base_radius*sin(2.0*M_PI_3)), -1.f/3.f);
    vertices[3]=i_origin+Point3D_f((float) (base_radius*cos(2.0*M_PI_3)), (float) (-base_radius*sin(2.0*M_PI_3)), -1.f/3.f);

    triangles[0]=MeshTriangle(1,3,2);
    triangles[1]=MeshTriangle(1,0,3);
    triangles[2]=MeshTriangle(1,2,0);
    triangles[3]=MeshTriangle(0,2,3);

    return intrusive_ptr<TriangleMesh>(new TriangleMesh(vertices,triangles));
    }

  inline intrusive_ptr<TriangleMesh> ConstructSphere(const Point3D_d &i_center, double i_radius, size_t i_subdivisions)
    {
    std::string center_str, radius_str, subdivisions_str;

      {
      std::stringstream sstream;
      sstream << i_center;

      char buf[256];
      sstream.getline(buf, 256);
      center_str = std::string(buf);
      }

      {
      std::stringstream sstream;
      sstream << i_radius;

      char buf[256];
      sstream.getline(buf, 256);
      radius_str = std::string(buf);
      }

      {
      std::stringstream sstream;
      sstream << i_subdivisions;

      char buf[256];
      sstream.getline(buf, 256);
      subdivisions_str = std::string(buf);
      }

      Sphere s;
      s.SetParameter("Center",center_str);
      s.SetParameter("Radius",radius_str);
      s.SetParameter("Subdivisions",subdivisions_str);

      return s.BuildMesh();
    }

  }

#endif // TRIANGLE_MESH_TEST_HELPER_H