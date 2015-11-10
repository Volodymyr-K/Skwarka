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

#include "AsyncLoggingProgressWorker.h"

AsyncLoggingProgressWorker::AsyncLoggingProgressWorker(Nan::Callback *ip_callback, intrusive_ptr<Log> ip_log) : Nan::AsyncProgressWorker(ip_callback), mp_log(ip_log)
  {
  mp_logs_async = new uv_async_t;
  uv_async_init(uv_default_loop(), mp_logs_async, _AsyncLogs);
  mp_logs_async->data = this;

  uv_mutex_init(&m_logs_async_lock);
  }

AsyncLoggingProgressWorker::~AsyncLoggingProgressWorker()
  {
  uv_mutex_destroy(&m_logs_async_lock);
  }

void AsyncLoggingProgressWorker::AddLogMessage(Log::LogLevel i_level, const std::string &i_string)
  {
  uv_mutex_lock(&m_logs_async_lock);
  m_logs.push_back({ i_level, i_string });
  uv_mutex_unlock(&m_logs_async_lock);

  uv_async_send(mp_logs_async);
  }

void AsyncLoggingProgressWorker::Destroy()
  {
  uv_close(reinterpret_cast<uv_handle_t*>(mp_logs_async), _AsyncLogsClose);
  Nan::AsyncProgressWorker::Destroy();
  }

void AsyncLoggingProgressWorker::_FlushLogs()
  {
  std::vector<std::pair<Log::LogLevel, std::string>> logs;

  uv_mutex_lock(&m_logs_async_lock);
  logs.swap(m_logs);
  uv_mutex_unlock(&m_logs_async_lock);

  if (logs.empty() == false)
    for (auto &p : logs)
      mp_log->LogMessage(p.first, p.second);
  }

NAUV_WORK_CB(AsyncLoggingProgressWorker::_AsyncLogs)
  {
  AsyncLoggingProgressWorker *p_worker = static_cast<AsyncLoggingProgressWorker*>(async->data);
  p_worker->_FlushLogs();
  }

void AsyncLoggingProgressWorker::_AsyncLogsClose(uv_handle_t* ip_handle)
  {
  delete reinterpret_cast<uv_async_t*>(ip_handle);
  }
