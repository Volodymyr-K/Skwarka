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

#ifndef SHAPE_MOCK_H
#define SHAPE_MOCK_H

#include <Common/Common.h>
#include <Shapes/Shape.h>
#include <Raytracer/Core/TriangleMesh.h>

/*
BaseShape mock implementation.
Always generates an empty mesh.

The implementation overrides the protected methods from the BaseShape to make them public for testing purposes.
*/
class ShapeMock: public BaseShape
  {
  public:
    ShapeMock(): BaseShape()
      {
      }

    intrusive_ptr<TriangleMesh> BuildMesh()
      {
      return intrusive_ptr<TriangleMesh>(new TriangleMesh(std::vector<Point3D_f>(),std::vector<MeshTriangle>()));
      }
    
    void AddError(const std::string &i_error)
      {
      _AddError(i_error);
      }

    bool ErrorsExist() const
      {
      return _ErrorsExist();
      }

    void ClearErrors()
      {
      _ClearErrors();
      }

    template<typename ParameterType>
    bool GetParameter(const std::string &i_parameter_name, ParameterType &o_parameter_value, bool i_required)
      {
      return _GetParameter(i_parameter_name, o_parameter_value, i_required);
      }
  };

#endif // SHAPE_MOCK_H