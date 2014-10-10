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

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>
#include "Assert.h"
#include "IntrusiveSmartPointer.h"

/**
* An abstract base logger class.
*/
class Log: public ReferenceCounted
  {
  public:

    enum LogLevel
      {
      INFO_LEVEL = 0,
      WARNING_LEVEL = 1,
      ERROR_LEVEL = 2
      };

    /**
    * Prints the information message for the given importance level.
    */
    void LogMessage(Log::LogLevel i_level, const std::string &i_string);

    /**
    * Sets the minimum log level. Messages below this level will not be logged.
    * By default the min log level is INFO_LEVEL, which means that all messages are always logged.
    */
    void SetMinLevel(Log::LogLevel i_min_level);

    /**
    * Returns current min log level.
    */
    Log::LogLevel GetMinLevel() const;

    virtual ~Log();

  protected:
    Log();

    Log(Log::LogLevel i_min_level);

    /**
    * Derived classes can call this method to get a string representation of the log level.
    */
    std::string LogLevelToString(Log::LogLevel i_level) const;

    /**
    * Derived classes must implement this method. This method is called only when the log level is above the min log level.
    */
    virtual void _LogMessage(Log::LogLevel i_level, const std::string &i_string) = 0;

  private:
    // Not implemented, not a value type.
    Log(const Log&);
    Log &operator=(const Log&);

    Log::LogLevel m_min_level;
  };

/**
* A simple Log implementation that prints log messages to the specified output stream.
* This is the caller's responsibility to make sure the specified stream is valid throughout the log's lifetime.
* The class is not thread-safe.
*/
class StreamLog: public Log
  { 
  public:
    /**
    * Creates StreamLog instance with the specified output stream.
    * The calling code must make sure that the stream is valid throughout the log's lifetime.
    */
    StreamLog(std::ostream& i_ostream = std::cerr);

    /**
    * Creates StreamLog instance with the specified output stream and min log level.
    * The calling code must make sure that the stream is valid throughout the log's lifetime.
    */
    StreamLog(std::ostream& i_ostream, Log::LogLevel i_min_level);

  private:
    /**
    * Prints the information message with the given importance level to the stream.
    */
    virtual void _LogMessage(Log::LogLevel i_level, const std::string &i_string);

  private:
    std::ostream& m_ostream;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Log::Log() : m_min_level(Log::LogLevel::INFO_LEVEL)
  {
  }

inline Log::Log(Log::LogLevel i_min_level) : m_min_level(i_min_level)
  {
  }

inline Log::~Log()
  {
  }

inline std::string Log::LogLevelToString(Log::LogLevel i_level) const
  {
  switch (i_level)
    {
    case INFO_LEVEL:
      return "Info";
    case WARNING_LEVEL:
      return "Warning";
    case ERROR_LEVEL:
      return "Error";
    default:
      ASSERT(0 && "Unknown log level.");
      return "Unknown log level";
    };
  }

inline void Log::LogMessage(Log::LogLevel i_level, const std::string &i_string)
  {
  if (i_level >= m_min_level)
    _LogMessage(i_level, i_string);
  }

inline void Log::SetMinLevel(Log::LogLevel i_min_level)
  {
  m_min_level = i_min_level;
  }

inline Log::LogLevel Log::GetMinLevel() const
  {
  return m_min_level;
  }

inline StreamLog::StreamLog(std::ostream& i_ostream): Log(), m_ostream(i_ostream)
  {
  }

inline StreamLog::StreamLog(std::ostream& i_ostream, Log::LogLevel i_min_level): Log(i_min_level), m_ostream(i_ostream)
  {
  }

inline void StreamLog::_LogMessage(Log::LogLevel i_level, const std::string &i_string)
  {
  m_ostream << LogLevelToString(i_level) << ": " << i_string << std::endl;
  }

#endif // LOG_H