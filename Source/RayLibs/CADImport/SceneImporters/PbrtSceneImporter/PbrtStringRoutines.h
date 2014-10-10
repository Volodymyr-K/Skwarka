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

#ifndef PBRT_STRING_ROUTINES_H
#define PBRT_STRING_ROUTINES_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "PbrtUtils.h"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace PbrtImport
  {

  /**
  * Helper class that represents a sub-string defined by two string iterators of some parent string.
  * The actual storage of the string is not handled by this class and is defined by the parent string container,
  * therefore care must be taken to use this class only while the parent string container is live and unchanged.
  */
  struct SubString
    {
    std::string::const_iterator m_begin, m_end;

    SubString(std::string::const_iterator i_begin, std::string::const_iterator i_end) : m_begin(i_begin), m_end(i_end) { ASSERT(i_begin<=i_end); }
    SubString(const std::string &i_str) : m_begin(i_str.begin()), m_end(i_str.end()) {}

    size_t size() const
      {
      ASSERT(m_begin<=m_end);
      return m_end-m_begin;
      }

    bool empty() const
      {
      ASSERT(m_begin<=m_end);
      return m_begin>=m_end;
      }

    std::string::value_type front() const
      {
      ASSERT(m_begin<=m_end);
      return *m_begin;
      }

    std::string::value_type back() const
      {
      ASSERT(m_begin<=m_end);
      return *(m_end-1);
      }

    std::string to_string() const
      {
      ASSERT(m_begin<=m_end);
      return std::string(m_begin, m_end);
      }
    };

  namespace StringRoutines
    {

    inline SubString TrimQuotes(SubString i_str)
      {
      if (i_str.size()>=2 && i_str.front()=='\"' && i_str.back()=='\"')
        return SubString(i_str.m_begin+1, i_str.m_end-1);
      else
        return i_str;
      }

    inline std::string TrimQuotes(const std::string &i_str)
      {
      if (i_str.size()>=2 && i_str[0]=='\"' && i_str[i_str.size()-1]=='\"')
        return i_str.substr(1, i_str.size()-2);
      else
        return i_str;
      }

    inline std::string GetDirectoryPath(const std::string &i_filename)
      {
      size_t slash_pos = i_filename.find_last_of("\\/");
      if (slash_pos==std::string::npos)
        return "";
      else
        return i_filename.substr(0, slash_pos);
      }

    inline std::string CanonizeFilePath(const std::string &i_filename)
      {
      // First split the path into parts separated by slash symbols.
      std::string cur;
      std::vector<std::string> parts;
      for(size_t i=0;i<i_filename.size();++i)
        {
        if (i_filename[i]=='/' || i_filename[i]=='\\')
          {
          if (cur.empty()==false) parts.push_back(cur);
          cur.clear();
          }
        else
          cur = cur + i_filename[i];
        }

      if (cur.empty()==false) parts.push_back(cur);

      // Now remove all references to the parent directory ("..").
      while(true)
        {
        bool changed=false;
        for(size_t i=0;i<parts.size();++i)
          if (i>0 && parts[i]=="..")
            {
            changed=true;
            parts.erase(parts.begin()+i-1,parts.begin()+i+1);
            break;
            }

        if (changed==false) break;
        }

      std::string ret;
      for(size_t i=0;i<parts.size();++i)
        {
        if (i>0) ret += '/';
        ret += parts[i];
        }
      return ret;
      }

    inline bool Split(SubString i_str, std::vector<SubString> &o_parts, intrusive_ptr<Log> ip_log)
      {
      o_parts.clear();
      o_parts.reserve(i_str.size()/10);

      std::string::const_iterator cur_begin = i_str.m_begin;
      int quotes = 0, braces = 0;
      for (std::string::const_iterator it = i_str.m_begin; it != i_str.m_end; ++it)
        {
        if (*it==' ' || *it=='\t')
          {
          if (cur_begin==it) { cur_begin = it+1; continue; }
          if (quotes==0 && braces==0) { o_parts.push_back(SubString(cur_begin, it)); cur_begin = it+1; continue; }
          }
        else
          {
          if (*it=='\"') quotes = 1-quotes;
          if (*it=='[') ++braces;
          if (*it==']' && (--braces)<0) { PbrtImport::Utils::LogError(ip_log, "Closing square bracket (]) does not have a matching opening one."); return false; }
          }
        }

      if (quotes!=0) {PbrtImport::Utils::LogError(ip_log, "Quotes structure is incorrect.");return false;}
      if (braces!=0) {PbrtImport::Utils::LogError(ip_log, "Square brackets structure is incorrect.");return false;}

      if (cur_begin < i_str.m_end) o_parts.push_back(SubString(cur_begin, i_str.m_end));
      return true;
      }

    inline bool ExpandBraces(const std::vector<SubString> &i_parts, size_t i_start, std::vector<SubString> &o_expanded, intrusive_ptr<Log> ip_log)
      {
      for(size_t i=i_start;i<i_parts.size();++i)
        {
        if (i_parts[i].size()>=2 && i_parts[i].front()=='[' && i_parts[i].back()==']')
          {
          std::vector<SubString> expanded_parts;
          if (Split(SubString(i_parts[i].m_begin+1, i_parts[i].m_end-1), expanded_parts, ip_log)==false) return false;
          if (ExpandBraces(expanded_parts, 0, o_expanded, ip_log)==false) return false;
          }
        else
          o_expanded.push_back(i_parts[i]);
        }

      return true;
      }

    inline bool ParseFloatValues(const std::vector<SubString> &i_string_values, std::vector<float> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();
      try
        {
        for (size_t i = 0; i<i_string_values.size(); ++i) o_converted_values.push_back(std::stof(i_string_values[i].to_string()));
        }
      catch (std::invalid_argument &) { PbrtImport::Utils::LogError(ip_log, "Can not parse float values."); return false; }
      catch (std::out_of_range &) { PbrtImport::Utils::LogError(ip_log, "Can not parse float values. Values out of range."); return false; }

      return true;
      }

    inline bool ParseIntegerValues(const std::vector<SubString> &i_string_values, std::vector<int> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();
      try
        {
        for (size_t i = 0; i<i_string_values.size(); ++i) o_converted_values.push_back(std::stoi(i_string_values[i].to_string()));
        }
      catch (std::invalid_argument &) { PbrtImport::Utils::LogError(ip_log, "Can not parse integer values."); return false; }
      catch (std::out_of_range &) { PbrtImport::Utils::LogError(ip_log, "Can not parse integer values. Values out of range."); return false; }

      return true;
      }

    inline bool ParseStringValues(const std::vector<SubString> &i_string_values, std::vector<std::string> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();
      for(size_t i=0;i<i_string_values.size();++i) o_converted_values.push_back(TrimQuotes(i_string_values[i]).to_string());
      return true;
      }

    inline bool ParseBoolValues(const std::vector<SubString> &i_string_values, std::vector<char> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();
      try
        {
        for(size_t i=0;i<i_string_values.size();++i)
          {
          SubString trimmed(TrimQuotes(i_string_values[i]));
          if (boost::iequals(trimmed.to_string(), "true")) o_converted_values.push_back(1); else o_converted_values.push_back(0);
          }
        }
      catch(boost::bad_lexical_cast &) {PbrtImport::Utils::LogError(ip_log, "Can not parse bool values.");return false;}
      return true;
      }

    inline bool ParsePoint3DValues(const std::vector<SubString> &i_string_values, std::vector<Point3D_d> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();
      if ((i_string_values.size()%3)!=0) PbrtImport::Utils::LogWarning(ip_log, "Excess values given with point parameter. Ignoring the excess values.");

      try
        {
        for (size_t i = 0; i+2<i_string_values.size(); i += 3)
          {
          double x = std::stod(i_string_values[i+0].to_string());
          double y = std::stod(i_string_values[i+1].to_string());
          double z = std::stod(i_string_values[i+2].to_string());
          o_converted_values.push_back(Point3D_d(x, y, z));
          }
        }
      catch (std::invalid_argument &) { PbrtImport::Utils::LogError(ip_log, "Can not parse Point3D values."); return false; }
      catch (std::out_of_range &) { PbrtImport::Utils::LogError(ip_log, "Can not parse Point3D values. Values out of range."); return false; }

      return true;
      }

    inline bool ParseVector3DValues(const std::vector<SubString> &i_string_values, std::vector<Vector3D_d> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();
      if ((i_string_values.size()%3)!=0) PbrtImport::Utils::LogWarning(ip_log, "Excess values given with vector parameter. Ignoring the excess values.");
      
      try
        {
        for (size_t i = 0; i+2<i_string_values.size(); i += 3)
          {
          double x = std::stod(i_string_values[i+0].to_string());
          double y = std::stod(i_string_values[i+1].to_string());
          double z = std::stod(i_string_values[i+2].to_string());
          o_converted_values.push_back(Vector3D_d(x, y, z));
          }
        }
      catch (std::invalid_argument &) { PbrtImport::Utils::LogError(ip_log, "Can not parse Vector3D values."); return false; }
      catch (std::out_of_range &) { PbrtImport::Utils::LogError(ip_log, "Can not parse Vector3D values. Values out of range."); return false; }

      return true;
      }

    };
  };

#endif // PBRT_STRING_ROUTINES_H
