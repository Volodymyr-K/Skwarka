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

#ifndef LOG_TEST_H
#define LOG_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <sstream>
#include <string>

class StreamLogTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_StreamLog()
      {
      std::stringstream sstream;
      StreamLog log(sstream);

      log.LogMessage(Log::INFO_LEVEL, "Info message.");
      log.LogMessage(Log::WARNING_LEVEL, "Warning message.");
      log.LogMessage(Log::ERROR_LEVEL, "Error message.");

      char buf[1024];

      sstream.getline(buf, 256);
      std::string res1(buf);

      sstream.getline(buf, 256);
      std::string res2(buf);

      sstream.getline(buf, 256);
      std::string res3(buf);

      TS_ASSERT(res1 == "Info: Info message.");
      TS_ASSERT(res2 == "Warning: Warning message.");
      TS_ASSERT(res3 == "Error: Error message.");
      }


    void test_StreamLog_MinLevel()
      {
      std::stringstream sstream;
      StreamLog log(sstream);
      log.SetMinLevel(Log::ERROR_LEVEL);

      log.LogMessage(Log::INFO_LEVEL, "Info message.");
      log.LogMessage(Log::WARNING_LEVEL, "Warning message.");
      log.LogMessage(Log::ERROR_LEVEL, "Error message.");

      char buf[1024];

      sstream.getline(buf, 256);
      std::string res1(buf);

      TS_ASSERT(res1 == "Error: Error message.");
      }

  };

#endif // LOG_TEST_H