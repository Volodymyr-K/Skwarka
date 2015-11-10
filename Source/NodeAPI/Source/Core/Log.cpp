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

#include "Log.h"

Nan::Persistent<v8::Function> global_log_callback;

CallbackLog::CallbackLog() : Log()
  {
  }

CallbackLog::CallbackLog(Log::LogLevel i_min_level) : Log(i_min_level)
  {
  }

void CallbackLog::_LogMessage(Log::LogLevel i_level, const std::string &i_string)
  {
  if (global_log_callback.IsEmpty() == false)
    {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = { Nan::New(LogLevelToString(i_level)).ToLocalChecked(), Nan::New(i_string).ToLocalChecked() };

    auto log_callback_local = Nan::New<v8::Function>(global_log_callback);
    shared_ptr<Nan::Callback> p_callback(new Nan::Callback(log_callback_local));
    p_callback->Call(2, argv);
    }
  }

AsyncLog::AsyncLog(AsyncLoggingProgressWorker *ip_worker) :
Log(), mp_worker(ip_worker)
  {
  }

AsyncLog::AsyncLog(AsyncLoggingProgressWorker *ip_worker, Log::LogLevel i_min_level) :
Log(i_min_level), mp_worker(ip_worker)
  {
  }

void AsyncLog::_LogMessage(Log::LogLevel i_level, const std::string &i_string)
  {
  mp_worker->AddLogMessage(i_level, i_string);
  }