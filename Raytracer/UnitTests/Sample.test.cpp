/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_LONGLONG long long
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>

bool SampleTestSuite_init = false;
#include "./Sample.test.h"

static SampleTestSuite suite_SampleTestSuite;

static CxxTest::List Tests_SampleTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SampleTestSuite( "./Sample.test.h", 9, "SampleTestSuite", suite_SampleTestSuite, Tests_SampleTestSuite );

static class TestDescription_SampleTestSuite_testSampleConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_SampleTestSuite_testSampleConstr() : CxxTest::RealTestDescription( Tests_SampleTestSuite, suiteDescription_SampleTestSuite, 12, "testSampleConstr" ) {}
 void runTest() { suite_SampleTestSuite.testSampleConstr(); }
} testDescription_SampleTestSuite_testSampleConstr;

static class TestDescription_SampleTestSuite_testSampleImagePoint : public CxxTest::RealTestDescription {
public:
 TestDescription_SampleTestSuite_testSampleImagePoint() : CxxTest::RealTestDescription( Tests_SampleTestSuite, suiteDescription_SampleTestSuite, 21, "testSampleImagePoint" ) {}
 void runTest() { suite_SampleTestSuite.testSampleImagePoint(); }
} testDescription_SampleTestSuite_testSampleImagePoint;

static class TestDescription_SampleTestSuite_testSampleSequences : public CxxTest::RealTestDescription {
public:
 TestDescription_SampleTestSuite_testSampleSequences() : CxxTest::RealTestDescription( Tests_SampleTestSuite, suiteDescription_SampleTestSuite, 31, "testSampleSequences" ) {}
 void runTest() { suite_SampleTestSuite.testSampleSequences(); }
} testDescription_SampleTestSuite_testSampleSequences;

