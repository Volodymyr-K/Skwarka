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

    shared_ptr<TriangleMesh> BuildMesh()
      {
      return shared_ptr<TriangleMesh>(new TriangleMesh(std::vector<Point3D_f>(),std::vector<MeshTriangle>()));
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