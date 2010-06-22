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
      INFO_LEVEL,
      WARNING_LEVEL,
      ERROR_LEVEL
      };

    /**
    * Prints the information message with the given importance level.
    */
    virtual void LogMessage(Log::LogLevel i_level, const std::string &i_string) = 0;

    virtual ~Log();

  protected:
    Log();

    std::string LogLevelToString(Log::LogLevel i_level) const;

  private:
    // Not implemented, not a value type.
    Log(const Log&);
    Log &operator=(const Log&);
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
    * Prints the information message with the given importance level to the stream.
    */
    virtual void LogMessage(Log::LogLevel i_level, const std::string &i_string);

  private:
    std::ostream& m_ostream;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Log::Log()
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

inline StreamLog::StreamLog(std::ostream& i_ostream): Log(), m_ostream(i_ostream)
  {
  }

inline void StreamLog::LogMessage(Log::LogLevel i_level, const std::string &i_string)
  {
  m_ostream << LogLevelToString(i_level) << ": " << i_string << std::endl;
  }

#endif // LOG_H