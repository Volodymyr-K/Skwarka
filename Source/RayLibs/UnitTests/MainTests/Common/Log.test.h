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