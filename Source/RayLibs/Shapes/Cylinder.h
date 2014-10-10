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

#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"
#include <Math/Geometry.h>
#include <Math/Transform.h>

/**
* %Cylinder implementation of the Shape interface.
* The class has no methods to set cylinder radius or height, instead it provides a method to set an arbitrary transformation.
* If no transformation is set, it will create a cylinder aligned with Z axis and with a unit radius with Z coordinate in [0;1] range.
* The number of subdivisions is configurable.
*/
class Cylinder: public Shape
  {
  public:
    Cylinder();

    /**
    * Sets number of cylinder subdivisions.
    * The final number of triangles in the mesh is equal to 2*i_subdivisions.
    * If the method is not called, the default number of subdivisions is 360.
    */
    void SetSubdivisions(size_t i_subdivisions);

    /**
    * Sets maximum value of the phi angle of the cylinder.
    * The triangles will be created only for the range of the phi angles equal to [0;i_max_phi].
    * If the method is not called, the default value is 2*M_PI.
    */
    void SetMaxPhi(double i_max_phi);

    /**
    * Sets transformation to be applied to the cylinder.
    * If the method is not called, the default transformation is the identity one which results
    * in a cylinder aligned with Z axis and with a unit radius with Z coordinate in [0;1] range.
    */
    void SetTransformation(const Transform &i_transform);

    /**
    * Sets whether to invert the normals of the resulting mesh. Default is false.
    */
    void SetInvertOrientation(bool i_invert_orientation);

    /**
    * Builds cylinder triangle mesh.
    * @return Smart pointer to the built mesh. Never NULL.
    */
    virtual intrusive_ptr<TriangleMesh> BuildMesh();

  private:
    size_t m_subdivisions;
    Transform m_transform;

    double m_max_phi;
    bool m_invert_orientation;
  };


#endif // CYLINDER_H