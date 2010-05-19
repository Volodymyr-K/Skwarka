/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Substrate.test.h"

static SubstrateTestSuite suite_SubstrateTestSuite;

static CxxTest::List Tests_SubstrateTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SubstrateTestSuite( "./Substrate.test.h", 16, "SubstrateTestSuite", suite_SubstrateTestSuite, Tests_SubstrateTestSuite );

static class TestDescription_SubstrateTestSuite_test_Substrate : public CxxTest::RealTestDescription {
public:
 TestDescription_SubstrateTestSuite_test_Substrate() : CxxTest::RealTestDescription( Tests_SubstrateTestSuite, suiteDescription_SubstrateTestSuite, 20, "test_Substrate" ) {}
 void runTest() { suite_SubstrateTestSuite.test_Substrate(); }
} testDescription_SubstrateTestSuite_test_Substrate;

