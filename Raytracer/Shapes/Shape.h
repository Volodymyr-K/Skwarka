#ifndef SHAPE_H
#define SHAPE_H

#include <Common/Common.h>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <Math/Geometry.h>
#include <Raytracer/Core/TriangleMesh.h>

/**
* Interface class for all shapes that can be triangulated into a TriangleMesh.
* This is a base class with only pure virtual methods. The interface defines a contract for all shapes that being configured from text parameters
* can produce TriangleMesh. The produced mesh is an approximated triangulation of the original shape defined by the Shape implementation.
* @sa BaseShape, TriangleMesh
*/
class Shape
  {
  public:
    Shape();

    /**
    * Sets (name, value) pair for a configuration parameter for the shape.
    * The parameter name is case-insensitive. If a parameter with the same name has already been set the old value is overwritten.
    */
    virtual void SetParameter(const std::string &i_parameter_name, const std::string &i_parameter_value) = 0;

    /**
    * Build mesh after all the parameters are set.
    * @return Smart pointer to the built mesh or a point to NULL if the mesh was not built yet.
    */
    virtual shared_ptr<TriangleMesh> BuildMesh() = 0;

    /**
    * Returns descriptions of the errors meet when building the mesh.
    * This method should be called after BuildMesh() method returned NULL pointer.
    * @return Vector of strings explaining the problems (e.g. missing or invalid parameters etc.).
    */
    virtual std::vector<std::string> GetErrors() const = 0;

    virtual ~Shape();

  private:
    // Not implemented, Shape is not a value type.
    Shape(const Shape&);
    Shape &operator=(const Shape&);
  };

/**
* A convenient base class for Shape implementations.
* It implements parameters setting and retrieving and error handling.
* @sa Shape, TriangleMesh
*/
class BaseShape: public Shape
  {
  public:
    /**
    * Sets (name, value) pair for a configuration parameter for the shape.
    * The parameter name is case-insensitive. If a parameter with the same name has already been set the old value is overwritten.
    */
    void SetParameter(const std::string &i_parameter_name, const std::string &i_parameter_value);

    /**
    * Returns descriptions of the errors meet when building the mesh.
    * This method should be called after BuildMesh() method returned NULL pointer.
    * @return Vector of strings explaining the problems (e.g. missing or invalid parameters etc.).
    */
    std::vector<std::string> GetErrors() const;

    virtual ~BaseShape();

  protected:
    BaseShape();
    
    /**
    * Adds new error description.
    */
    void _AddError(const std::string &i_error);

    /**
    * Returns true if the internal list of errors is not empty.
    */
    bool _ErrorsExist() const;

    /**
    * Clears internal list of errors.
    */
    void _ClearErrors();

    /**
    * A convenient method the derived classes should call to get the parameter value.
    * The method automatically adds an error description if a required parameter is missing or if a parameter value is malformed.
    * @param i_parameter_name Case-insensitive parameter name.
    * @param[out] o_parameter_value The output parameter value.
    * @param i_required true if the parameter is required. The method will add an error description if a required parameter is missing.
    * @return true if the parameter has been read successfully and false otherwise.
    */
    template<typename ParameterType>
    bool _GetParameter(const std::string &i_parameter_name, ParameterType &o_parameter_value, bool i_required);

  private:
    std::map<std::string, std::string> m_parameters;

    std::vector<std::string> m_errors;

    bool _GetParameterString(const std::string &i_parameter_name, std::string &o_parameter_value) const;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ParameterType>
inline bool BaseShape::_GetParameter(const std::string &i_parameter_name, ParameterType &o_parameter_value, bool i_required)
  {
  std::string str;
  if (_GetParameterString(i_parameter_name, str) == false)
    {
    if (i_required)
      _AddError(std::string("Required parameter is missing: ").append(i_parameter_name.c_str()));
    return false;
    }

  std::stringstream sstream;
  sstream << str;
  sstream >> o_parameter_value;
  if (sstream.fail())
    {
    _AddError(std::string("Parameter is malformed: ").append(i_parameter_name.c_str()));
    return false;
    }

  return true;
  }

#endif // SHAPE_H