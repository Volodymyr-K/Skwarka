#include "Shape.h"

Shape::Shape()
  {
  }

Shape::~Shape()
  {
  }

BaseShape::BaseShape()
  {
  }

BaseShape::~BaseShape()
  {
  }

void BaseShape::SetParameter(const std::string &i_parameter_name, const std::string &i_parameter_value)
  {
  std::string parameter_name_lowercase = i_parameter_name;
  std::transform(parameter_name_lowercase.begin(), parameter_name_lowercase.end(), parameter_name_lowercase.begin(), ::tolower);

  m_parameters[parameter_name_lowercase]=i_parameter_value;
  }

std::vector<std::string> BaseShape::GetErrors() const
  {
  return m_errors;
  }

void BaseShape::_AddError(const std::string &i_error)
  {
  m_errors.push_back(i_error);
  }

bool BaseShape::_ErrorsExist() const
  {
  return m_errors.empty()==false;
  }

void BaseShape::_ClearErrors()
  {
  m_errors.clear();
  }

bool BaseShape::_GetParameterString(const std::string &i_parameter_name, std::string &o_parameter_value) const
  {
  std::string parameter_name_lowercase = i_parameter_name;
  std::transform(parameter_name_lowercase.begin(), parameter_name_lowercase.end(), parameter_name_lowercase.begin(), ::tolower);

  std::map<std::string, std::string>::const_iterator it = m_parameters.find(parameter_name_lowercase);
  if (it == m_parameters.end())
    return false;

  o_parameter_value = it->second;
  return true;
  }