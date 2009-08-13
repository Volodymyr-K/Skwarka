#ifndef SHAPE_H
#define SHAPE_H

#include <Common\Common.h>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <Math\Geometry.h>
#include <Core\TriangleMesh.h>

class Shape
  {
  public:
    Shape() {}

    virtual void SetParameter(const std::string &i_parameter_name, const std::string &i_parameter_value) = 0;
    virtual shared_ptr<TriangleMesh> BuildMesh() = 0;

    virtual ~Shape() {}

  private:
    Shape(const Shape&);
    Shape &operator=(const Shape&);
  };

class BaseShape: public Shape
  {
  public:
    void SetParameter(const std::string &i_parameter_name, const std::string &i_parameter_value);

    virtual ~BaseShape() {}

  protected:
    BaseShape() {}

    template<typename ParameterType>
    void GetParameter(const std::string &i_parameter_name, ParameterType &o_parameter_value, bool i_required) const;

  private:
    std::map<std::string, std::string> m_parameters;

    bool GetParameterString(const std::string &i_parameter_name, std::string &o_parameter_value, bool i_required) const;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void BaseShape::SetParameter(const std::string &i_parameter_name, const std::string &i_parameter_value)
  {
  std::string parameter_name_lowercase = i_parameter_name;
  std::transform(parameter_name_lowercase.begin(), parameter_name_lowercase.end(), parameter_name_lowercase.begin(), ::tolower);

  if (m_parameters.find(parameter_name_lowercase) != m_parameters.end())
    Log::Warning("Duplicate parameter, overriding the value: \"%s\"", i_parameter_name.c_str());

  m_parameters[parameter_name_lowercase]=i_parameter_value;
  }

inline bool BaseShape::GetParameterString(const std::string &i_parameter_name, std::string &o_parameter_value, bool i_required) const
  {
  std::string parameter_name_lowercase = i_parameter_name;
  std::transform(parameter_name_lowercase.begin(), parameter_name_lowercase.end(), parameter_name_lowercase.begin(), ::tolower);

  std::map<std::string, std::string>::const_iterator it = m_parameters.find(parameter_name_lowercase);
  if (it == m_parameters.end())
    {
    if (i_required)
      Log::Error("Missing required parameter: \"%s\"", i_parameter_name.c_str());
    else
      Log::Info("Optional parameter missing, using default value: \"%s\"", i_parameter_name.c_str());

    return false;
    }
  
  o_parameter_value = it->second;
  return true;
  }

template<typename ParameterType>
inline void BaseShape::GetParameter(const std::string &i_parameter_name, ParameterType &o_parameter_value, bool i_required) const
  {
  std::string str;
  if (GetParameterString(i_parameter_name, str, i_required) == false)
    return;

  std::stringstream sstream;
  sstream << str;
  sstream >> o_parameter_value;
  }

template<>
inline void BaseShape::GetParameter(const std::string &i_parameter_name, Point3Df &o_parameter_value, bool i_required) const
  {
  std::string str;
  if (GetParameterString(i_parameter_name, str, i_required) == false)
    return;

  std::stringstream sstream;
  sstream << str;

  float x,y,z;
  sstream >> x >> y >> z;

  o_parameter_value = Point3Df(x,y,z);
  }

template<>
inline void BaseShape::GetParameter(const std::string &i_parameter_name, Vector3Df &o_parameter_value, bool i_required) const
  {
  std::string str;
  if (GetParameterString(i_parameter_name, str, i_required) == false)
    return;

  std::stringstream sstream;
  sstream << str;

  float x,y,z;
  sstream >> x >> y >> z;

  o_parameter_value = Vector3Df(x,y,z);
  }

#endif // SHAPE_H