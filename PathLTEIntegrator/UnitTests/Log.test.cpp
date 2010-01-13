/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Log.test.h"

static StreamLogTestSuite suite_StreamLogTestSuite;

static CxxTest::List Tests_StreamLogTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_StreamLogTestSuite( "./Log.test.h", 10, "StreamLogTestSuite", suite_StreamLogTestSuite, Tests_StreamLogTestSuite );

static class TestDescription_StreamLogTestSuite_test_StreamLog : public CxxTest::RealTestDescription {
public:
 TestDescription_StreamLogTestSuite_test_StreamLog() : CxxTest::RealTestDescription( Tests_StreamLogTestSuite, suiteDescription_StreamLogTestSuite, 14, "test_StreamLog" ) {}
 void runTest() { suite_StreamLogTestSuite.test_StreamLog(); }
} testDescription_StreamLogTestSuite_test_StreamLog;

