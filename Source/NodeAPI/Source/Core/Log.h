/*
* Copyright (C) 2015 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
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

#ifndef SKWARKA_NODE_LOG_H
#define SKWARKA_NODE_LOG_H

#include <nan.h>
#include <Common/Common.h>

#include "AsyncLoggingProgressWorker.h"

/**
* JS function for the logger function. This logger can be set globally and is used by CallbackLog class (see below).
* The JS method is expected to take two JS strings as parameters: logging level and message.
*/
extern Nan::Persistent<v8::Function> global_log_callback;

/**
* Logs messages by calling the global_log_callback JS function.
*/
class CallbackLog : public Log
  {
  public:
    CallbackLog();

    CallbackLog(Log::LogLevel i_min_level);

  private:
    virtual void _LogMessage(Log::LogLevel i_level, const std::string &i_string);
  };

/**
* Logger implementation that acts as a bridge between the worker threads and V8 main thread.
* It logs all messages by calling AsyncLoggingProgressWorker::AddLogMessage() method.
*/
class AsyncLog : public Log
  {
  public:
    AsyncLog(AsyncLoggingProgressWorker *ip_worker);

    AsyncLog(AsyncLoggingProgressWorker *ip_worker, Log::LogLevel i_min_level);

  private:
    virtual void _LogMessage(Log::LogLevel i_level, const std::string &i_string);

  private:
    AsyncLoggingProgressWorker *mp_worker;
  };

#endif // SKWARKA_NODE_LOG_H