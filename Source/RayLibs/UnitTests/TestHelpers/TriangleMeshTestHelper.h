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

#ifndef TRIANGLE_MESH_TEST_HELPER_H
#define TRIANGLE_MESH_TEST_HELPER_H

/*
This file contains various helper functions for tests that deal with triangle meshes.
These functions are called from different test suites so change them carefully.
*/

#include <Common/Common.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Shapes/Sphere.h>

namespace TriangleMeshHelper
  {
  /*
  This method creates a tetrahedron triangle mesh inscribed in a unit sphere centered at the specified point.
  */
  intrusive_ptr<TriangleMesh> ConstructTetrahedron(const Point3D_f &i_origin = Point3D_f(), const Transform &i_transform = Transform());

  /*
  This method creates a sphere triangle mesh with the specified center and radius.
  */
  intrusive_ptr<TriangleMesh> ConstructSphere(const Point3D_d &i_center, double i_radius, size_t i_subdivisions = 6);
  }

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace TriangleMeshHelper
  {

  inline intrusive_ptr<TriangleMesh> ConstructTetrahedron(const Point3D_f &i_origin, const Transform &i_transform)
    {
    std::vector<Point3D_f> vertices(4);
    std::vector<MeshTriangle> triangles(4);

    double base_radius = sqrt(8.0)/3.0;
    vertices[0]=i_origin+Point3D_f(0.f, 0.f, 1.f);
    vertices[1]=i_origin+Point3D_f((float) base_radius,  0.f, -1.f/3.f);
    vertices[2]=i_origin+Point3D_f((float) (base_radius*cos(2.0*M_PI_3)), (float) ( base_radius*sin(2.0*M_PI_3)), -1.f/3.f);
    vertices[3]=i_origin+Point3D_f((float) (base_radius*cos(2.0*M_PI_3)), (float) (-base_radius*sin(2.0*M_PI_3)), -1.f/3.f);

    for(size_t i=0;i<4;++i)
      i_transform(vertices[i], vertices[i]);

    triangles[0]=MeshTriangle(1,3,2);
    triangles[1]=MeshTriangle(1,0,3);
    triangles[2]=MeshTriangle(1,2,0);
    triangles[3]=MeshTriangle(0,2,3);

    return intrusive_ptr<TriangleMesh>(new TriangleMesh(vertices,triangles));
    }

  inline intrusive_ptr<TriangleMesh> ConstructSphere(const Point3D_d &i_center, double i_radius, size_t i_subdivisions)
    {
    Sphere s;
    s.SetTransformation(MakeTranslation(Vector3D_d(i_center))*MakeScale(i_radius));
    s.SetSubdivisions(i_subdivisions);
    return s.BuildMesh();
    }

  }

#endif // TRIANGLE_MESH_TEST_HELPER_H