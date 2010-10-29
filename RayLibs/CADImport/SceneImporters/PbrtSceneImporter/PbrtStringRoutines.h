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
  namespace StringRoutines
    {

    inline std::string TrimQuotes(const std::string &i_str)
      {
      if (i_str.size()>=2 && i_str[0]=='\"' && i_str[i_str.size()-1]=='\"')
        return i_str.substr(1, i_str.size()-2);
      else
        return i_str;
      }

    inline std::string GetFirstWord(const std::string &i_str)
      {
      size_t space_pos = i_str.find_first_of(" \t");
      if (space_pos == std::string::npos)
        return i_str;
      else
        return i_str.substr(0,space_pos);
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

      // Now remove all references top the parent directory ("..").
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

    inline bool Split(const std::string &i_str, std::vector<std::string> &o_parts, intrusive_ptr<Log> ip_log)
      {
      o_parts.clear();

      std::string cur;
      int quotes = 0, braces = 0;

      for(size_t i=0;i<i_str.size();++i)
        {
        if (i_str[i]==' ' || i_str[i]=='\t')
          {
          if (cur.empty()) continue;
          if (quotes==0 && braces==0) {o_parts.push_back(cur);cur="";continue;}
          cur += i_str[i];
          }
        else
          {
          cur += i_str[i];
          if (i_str[i]=='\"') quotes = 1-quotes;
          if (i_str[i]=='[') ++braces;
          if (i_str[i]==']') if (--braces<0) {PbrtImport::Utils::LogError(ip_log, "Closing square bracket (]) does not have a matching opening one.");return false;}
          }
        }

      if (quotes!=0) {PbrtImport::Utils::LogError(ip_log, "Quotes structure is incorrect.");return false;}
      if (braces!=0) {PbrtImport::Utils::LogError(ip_log, "Square brackets structure is incorrect.");return false;}

      if (cur.empty()==false) o_parts.push_back(cur);
      return true;
      }

    inline bool ExpandBraces(const std::vector<std::string> &i_parts, size_t i_start, std::vector<std::string> &o_expanded, intrusive_ptr<Log> ip_log)
      {
      for(size_t i=i_start;i<i_parts.size();++i)
        {
        if (i_parts[i].size()>=2 && i_parts[i][0]=='[' && i_parts[i][i_parts[i].size()-1]==']')
          {
          std::vector<std::string> expanded_parts;
          if (Split(i_parts[i].substr(1,i_parts[i].size()-2), expanded_parts, ip_log)==false) return false;
          if (ExpandBraces(expanded_parts, 0, o_expanded, ip_log)==false) return false;
          }
        else
          o_expanded.push_back(i_parts[i]);
        }

      return true;
      }

    inline bool ParseFloatValues(const std::vector<std::string> &i_string_values, std::vector<float> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();

      try
        {
        for(size_t i=0;i<i_string_values.size();++i) o_converted_values.push_back( boost::lexical_cast<float>( i_string_values[i] ) );
        }
      catch(boost::bad_lexical_cast &) {PbrtImport::Utils::LogError(ip_log, "Can not parse float values.");return false;}
      return true;
      }

    inline bool ParseIntegerValues(const std::vector<std::string> &i_string_values, std::vector<int> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();

      try
        {
        for(size_t i=0;i<i_string_values.size();++i) o_converted_values.push_back( boost::lexical_cast<int>( i_string_values[i] ) );
        }
      catch(boost::bad_lexical_cast &) {PbrtImport::Utils::LogError(ip_log, "Can not parse integer values.");return false;}
      return true;
      }

    inline bool ParseStringValues(const std::vector<std::string> &i_string_values, std::vector<std::string> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();

      try
        {
        for(size_t i=0;i<i_string_values.size();++i) o_converted_values.push_back(TrimQuotes(i_string_values[i]));
        }
      catch(boost::bad_lexical_cast &) {PbrtImport::Utils::LogError(ip_log, "Can not parse string values.");return false;}
      return true;
      }

    inline bool ParseBoolValues(const std::vector<std::string> &i_string_values, std::vector<char> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();

      try
        {
        for(size_t i=0;i<i_string_values.size();++i)
          {
          if (boost::iequals(TrimQuotes(i_string_values[i]),"true")) o_converted_values.push_back(1); else o_converted_values.push_back(0);
          }
        }
      catch(boost::bad_lexical_cast &) {PbrtImport::Utils::LogError(ip_log, "Can not parse bool values.");return false;}
      return true;
      }

    inline bool ParsePoint3DValues(const std::vector<std::string> &i_string_values, std::vector<Point3D_d> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();
      if ((i_string_values.size()%3)!=0) PbrtImport::Utils::LogWarning(ip_log, "Excess values given with point parameter. Ignoring the excess values.");

      try
        {
        for(size_t i=0;i+2<i_string_values.size();i+=3)
          {
          double x = boost::lexical_cast<double>(i_string_values[i+0]);
          double y = boost::lexical_cast<double>(i_string_values[i+1]);
          double z = boost::lexical_cast<double>(i_string_values[i+2]);
          o_converted_values.push_back(Point3D_d(x,y,z));
          }
        }
      catch(boost::bad_lexical_cast &) {PbrtImport::Utils::LogError(ip_log, "Can not parse Point3D values.");return false;}
      return true;
      }

    inline bool ParseVector3DValues(const std::vector<std::string> &i_string_values, std::vector<Vector3D_d> &o_converted_values, intrusive_ptr<Log> ip_log)
      {
      o_converted_values.clear();
      if ((i_string_values.size()%3)!=0) PbrtImport::Utils::LogWarning(ip_log, "Excess values given with vector parameter. Ignoring the excess values.");

      try
        {
        for(size_t i=0;i+2<i_string_values.size();i+=3)
          {
          double x = boost::lexical_cast<double>(i_string_values[i+0]);
          double y = boost::lexical_cast<double>(i_string_values[i+1]);
          double z = boost::lexical_cast<double>(i_string_values[i+2]);
          o_converted_values.push_back(Vector3D_d(x,y,z));
          }
        }
      catch(boost::bad_lexical_cast &) {PbrtImport::Utils::LogError(ip_log,"Can not parse Vector3D values.");return false;}
      return true;
      }

    };
  };

#endif // PBRT_STRING_ROUTINES_H
