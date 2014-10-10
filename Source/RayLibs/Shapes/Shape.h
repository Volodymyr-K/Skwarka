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

#ifndef SHAPE_H
#define SHAPE_H

#include <Common/Common.h>
#include <Raytracer/Core/TriangleMesh.h>

/**
* Interface class for all shapes that can be triangulated into a TriangleMesh.
* This is a base class with the only pure virtual method to build the triangle mesh.
* @sa TriangleMesh
*/
class Shape
  {
  public:

    /**
    * Builds triangle mesh.
    * @return Smart pointer to the built mesh or a NULL pointer if the mesh can not be built (e.g. not all required parameters are set yet).
    */
    virtual intrusive_ptr<TriangleMesh> BuildMesh() = 0;

    virtual ~Shape();

  protected:
    Shape();

  private:
    // Not implemented, Shape is not a value type.
    Shape(const Shape&);
    Shape &operator=(const Shape&);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Shape::Shape()
  {
  }

inline Shape::~Shape()
  {
  }

#endif // SHAPE_H