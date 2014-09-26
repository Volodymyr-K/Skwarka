#ifndef PBRT_UTILS_H
#define PBRT_UTILS_H

#include <Common/Common.h>
#include <vector>
#include <ctype.h>
#include <cstdlib>
#include <string>
#include <boost/algorithm/string.hpp>
#include <Raytracer/Core/ImageSource.h>
#include <Raytracer/ImageSources/OpenEXRRgbaImageSource.h>
#include <Raytracer/ImageSources/RGBImageSource.h>

namespace PbrtImport
  {
  namespace Utils
    {

    inline void LogInfo(intrusive_ptr<Log> ip_log, const std::string &i_message)
      {
      if (ip_log)
        ip_log->LogMessage(Log::INFO_LEVEL, i_message);
      }

    inline void LogWarning(intrusive_ptr<Log> ip_log, const std::string &i_message)
      {
      if (ip_log)
        ip_log->LogMessage(Log::WARNING_LEVEL, i_message);
      }

    inline void LogError(intrusive_ptr<Log> ip_log, const std::string &i_message)
      {
      if (ip_log)
        ip_log->LogMessage(Log::ERROR_LEVEL, i_message);
      }

    inline bool ReadFloatFile(const std::string &filename, std::vector<float> *op_values, intrusive_ptr<Log> ip_log = NULL)
      {
      FILE *f = fopen(filename.c_str(), "r");
      if (!f)
        {
        LogError(ip_log, std::string("Unable to open file \"") + filename + std::string("\""));
        return false;
        }

      int c;
      bool inNumber = false;
      char curNumber[32];
      int curNumberPos = 0;
      int lineNumber = 1;
      while ((c = getc(f)) != EOF)
        {
        if (c == '\n') ++lineNumber;
        if (inNumber)
          {
          if (isdigit(c) || c == '.' || c == 'e' || c == '-' || c == '+')
            curNumber[curNumberPos++] = c;
          else
            {
            curNumber[curNumberPos++] = '\0';
            op_values->push_back((float)atof(curNumber));
            ASSERT(curNumberPos < (int)sizeof(curNumber));
            inNumber = false;
            curNumberPos = 0;
            }
          }
        else
          {
          if (isdigit(c) || c == '.' || c == '-' || c == '+')
            {
            inNumber = true;
            curNumber[curNumberPos++] = c;
            }
          else if (c == '#')
            {
            while ((c = getc(f)) != '\n' && c != EOF)
              ;
            ++lineNumber;
            }
          else if (!isspace(c) && ip_log)
            {
            std::string message("Unexpected text found at line ");
            message += lineNumber;message += std::string(" of float file \"");
            message += filename;message += std::string("\".");
            LogError(ip_log, message);
            }
          }
        }
      fclose(f);
      return true;
      }

    inline bool IsAbsolutePath(const std::string &i_filename)
      {
      if (i_filename.size() == 0)
        return false;
      return (i_filename[0] == '\\' || i_filename[0] == '/' || i_filename.find(':') != std::string::npos);
      }

    template<typename T>
    intrusive_ptr<const ImageSource<T> > CreateImageSourceFromFile(const std::string &i_filename, bool i_E_whitepoint, double i_scale, intrusive_ptr<Log> ip_log)
      {
      size_t dot_pos = i_filename.find_last_of('.');
      if (dot_pos == std::string::npos || dot_pos+1 == i_filename.size())
        {
        LogError(ip_log, "Incorrect filename: " + i_filename);
        return NULL;
        }

      std::string ext = i_filename.substr(dot_pos+1);

      try
        {
        if (boost::iequals(ext, "exr"))
          return new OpenEXRRgbaImageSource<T>(i_filename, i_E_whitepoint, i_scale);
        else
          {
          intrusive_ptr<const ImageSource<T> > p_image_source;
          if (i_E_whitepoint)
            p_image_source.reset(new RGBImageSource<T>(i_filename, global_sRGB_E_ColorSystem, i_scale));
          else
            p_image_source.reset(new RGBImageSource<T>(i_filename, global_sRGB_D65_ColorSystem, i_scale));

          return p_image_source;
          }
        }
      catch (const std::exception &)
        {
        LogError(ip_log, "Exception occurred when reading image file: " + i_filename );
        return NULL;
        }

      LogError(ip_log, "Unsupported image type : " + ext);
      return NULL;
      }

    };

  };

#endif // PBRT_UTILS_H
