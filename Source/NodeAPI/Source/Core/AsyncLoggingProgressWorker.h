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

#ifndef ASYNC_LOGGING_PROGRESS_WORKER_H
#define ASYNC_LOGGING_PROGRESS_WORKER_H

#include <nan.h>
#include <vector>
#include <string>

#include <Common/Log.h>

/**
* Extends the NAN's AsyncProgressWorker class to add asynchronous logging similar to how AsyncProgressWorker asynchronously reports progress data.
* The class is needed to provide a mechanism to transfer logs from the worker threads to the V8 main thread in a thread-safe manner.
*/
class AsyncLoggingProgressWorker : public Nan::AsyncProgressWorker
  {
  public:
    /**
    * Constructs AsyncLoggingProgressWorker with the given callback and logger.
    * @param ip_callback The progress callback, which will be passed to AsyncProgressWorker's constructor.
    * @param ip_log Logger instance that will be called in V8 main thread.
    */
    explicit AsyncLoggingProgressWorker(Nan::Callback *ip_callback, intrusive_ptr<Log> ip_log);

    virtual ~AsyncLoggingProgressWorker();

    /**
    * This method can be called by the worker threads to add logs, that will eventually be passed to the V8 main thread.
    */
    void AddLogMessage(Log::LogLevel i_level, const std::string &i_string);

    /**
    * Cleans up internal stuff.
    * This method is called by the internals of NAN and must not be called by the user.
    */
    virtual void Destroy();

  private:
    /**
    * Sends all accumulated logs to mp_log.
    * This method is called in the V8 main thread.
    */
    void _FlushLogs();

    static NAUV_WORK_CB(_AsyncLogs);

    static void _AsyncLogsClose(uv_handle_t* ip_handle);

    uv_async_t *mp_logs_async;
    uv_mutex_t m_logs_async_lock;
  
    std::vector<std::pair<Log::LogLevel, std::string>> m_logs;
    intrusive_ptr<Log> mp_log;
  };

#endif // ASYNC_LOGGING_PROGRESS_WORKER_H