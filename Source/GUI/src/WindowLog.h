#ifndef WINDOW_LOG_H
#define WINDOW_LOG_H

#include <Common/Log.h>
#include <QQuickItem>

class WindowLog: public Log
  {
  public:
    /**
    * Creates StreamLog instance with the specified output stream.
    * The calling code must make sure that the stream is valid throughout the log's lifetime.
    */
    WindowLog(QQuickItem *ip_log_item);

    /**
    * Creates StreamLog instance with the specified output stream and min log level.
    * The calling code must make sure that the stream is valid throughout the log's lifetime.
    */
    WindowLog(QQuickItem *ip_log_item, Log::LogLevel i_min_level);

  private:
    /**
    * Prints the information message with the given importance level to the stream.
    */
    virtual void _LogMessage(Log::LogLevel i_level, const std::string &i_string);

  private:
    QQuickItem *mp_log_item;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline WindowLog::WindowLog(QQuickItem *ip_log_item): Log(), mp_log_item(ip_log_item)
  {
  }

inline WindowLog::WindowLog(QQuickItem *ip_log_item, Log::LogLevel i_min_level): Log(i_min_level), mp_log_item(ip_log_item)
  {
  }

inline void WindowLog::_LogMessage(Log::LogLevel i_level, const std::string &i_string)
  {
  std::string msg = LogLevelToString(i_level) + ": " + i_string;
  QVariant arg = msg.c_str();
  QMetaObject::invokeMethod(mp_log_item, "append", Q_ARG(QVariant, arg));
  }

#endif // WINDOW_LOG_H
