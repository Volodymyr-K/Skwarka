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

#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include <Math/Geometry.h>
#include <Math/Transform.h>

/**
* %Sphere implementation of the Shape interface.
* The triangulation method starts from a tetrahedron and then iteratively subdivides each face into four new triangles.
* The class has no methods to set sphere radius or sphere center, instead it provides a method to set an arbitrary transformation.
* If no transformation is set, it will create a sphere centered at the origin with a unit radius.
* The number of subdivision iterations is configurable.
*/
class Sphere: public Shape
  {
  public:
    Sphere();

    /**
    * Sets number of sphere subdivisions.
    * The final number of triangles in the mesh is equal to 4^(i_subdivisions+1).
    * If the method is not called, the default number of subdivisions is 4.
    */
    void SetSubdivisions(size_t i_subdivisions);

    /**
    * Sets transformation to be applied to the sphere.
    * If the method is not called, the default transformation is the identity one which results in a sphere centered at the origin with a unit radius.
    */
    void SetTransformation(const Transform &i_transform);

    /**
    * Sets whether to invert the normals of the resulting mesh. Default is false.
    */
    void SetInvertOrientation(bool i_invert_orientation);

    /**
    * Builds sphere triangle mesh.
    * @return Smart pointer to the built mesh. Never NULL.
    */
    virtual intrusive_ptr<TriangleMesh> BuildMesh();

  private:
    size_t m_subdivisions;
    Transform m_transform;
    bool m_invert_orientation;
  };


#endif // SPHERE_H