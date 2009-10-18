#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <cstdlib>
#include <stdarg.h>

/**
* Logger class that prints the messages to stderr.
*/
class Log
  {
  public:
    /**
    * Prints information message.
    * The method has a printf-like signature.
    */
    static void Info(const char *format, ...);

    /**
    * Prints warning message.
    * The method has a printf-like signature.
    */
    static void Warning(const char *format, ...);

    /**
    * Prints error message and terminates the program. Call it for fatal errors only.
    * The method has a printf-like signature.
    */
    static void Error(const char *format, ...);

  private:
    Log();

    static void processError(const char *format, va_list args, const char *message, bool terminate);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void Log::processError(const char *format, va_list args, const char *message, bool terminate)
  {
  char errorBuf[2048];
  _vsnprintf_s(errorBuf, sizeof(errorBuf), sizeof(errorBuf), format, args);
  fprintf(stderr, "%s: %s\n", message, errorBuf);
  if (terminate)
    exit(1);
  }

inline void Log::Info(const char *format, ...) {
  va_list args;
  va_start(args, format);
  processError(format, args, "Info", false);
  va_end(args);
  }

inline void Log::Warning(const char *format, ...) {
  va_list args;
  va_start(args, format);
  processError(format, args, "Warning", false);
  va_end(args);
  }

inline void Log::Error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  processError(format, args, "Error", true);
  va_end(args);
  }

#endif // LOG_H