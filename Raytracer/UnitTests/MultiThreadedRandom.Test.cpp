/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_LONGLONG long long
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>

bool MultiThreadedRandomTestSuite_init = false;
#include "./MultiThreadedRandom.Test.h"

static MultiThreadedRandomTestSuite suite_MultiThreadedRandomTestSuite;

static CxxTest::List Tests_MultiThreadedRandomTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_MultiThreadedRandomTestSuite( "./MultiThreadedRandom.Test.h", 9, "MultiThreadedRandomTestSuite", suite_MultiThreadedRandomTestSuite, Tests_MultiThreadedRandomTestSuite );

static class TestDescription_MultiThreadedRandomTestSuite_testRandomDoubleRange1 : public CxxTest::RealTestDescription {
public:
 TestDescription_MultiThreadedRandomTestSuite_testRandomDoubleRange1() : CxxTest::RealTestDescription( Tests_MultiThreadedRandomTestSuite, suiteDescription_MultiThreadedRandomTestSuite, 12, "testRandomDoubleRange1" ) {}
 void runTest() { suite_MultiThreadedRandomTestSuite.testRandomDoubleRange1(); }
} testDescription_MultiThreadedRandomTestSuite_testRandomDoubleRange1;

static class TestDescription_MultiThreadedRandomTestSuite_testRandomDoubleRange2 : public CxxTest::RealTestDescription {
public:
 TestDescription_MultiThreadedRandomTestSuite_testRandomDoubleRange2() : CxxTest::RealTestDescription( Tests_MultiThreadedRandomTestSuite, suiteDescription_MultiThreadedRandomTestSuite, 24, "testRandomDoubleRange2" ) {}
 void runTest() { suite_MultiThreadedRandomTestSuite.testRandomDoubleRange2(); }
} testDescription_MultiThreadedRandomTestSuite_testRandomDoubleRange2;

static class TestDescription_MultiThreadedRandomTestSuite_testRandomIntRange1 : public CxxTest::RealTestDescription {
public:
 TestDescription_MultiThreadedRandomTestSuite_testRandomIntRange1() : CxxTest::RealTestDescription( Tests_MultiThreadedRandomTestSuite, suiteDescription_MultiThreadedRandomTestSuite, 37, "testRandomIntRange1" ) {}
 void runTest() { suite_MultiThreadedRandomTestSuite.testRandomIntRange1(); }
} testDescription_MultiThreadedRandomTestSuite_testRandomIntRange1;

static class TestDescription_MultiThreadedRandomTestSuite_testRandomIntRange2 : public CxxTest::RealTestDescription {
public:
 TestDescription_MultiThreadedRandomTestSuite_testRandomIntRange2() : CxxTest::RealTestDescription( Tests_MultiThreadedRandomTestSuite, suiteDescription_MultiThreadedRandomTestSuite, 49, "testRandomIntRange2" ) {}
 void runTest() { suite_MultiThreadedRandomTestSuite.testRandomIntRange2(); }
} testDescription_MultiThreadedRandomTestSuite_testRandomIntRange2;

static class TestDescription_MultiThreadedRandomTestSuite_testRandomDistribution : public CxxTest::RealTestDescription {
public:
 TestDescription_MultiThreadedRandomTestSuite_testRandomDistribution() : CxxTest::RealTestDescription( Tests_MultiThreadedRandomTestSuite, suiteDescription_MultiThreadedRandomTestSuite, 63, "testRandomDistribution" ) {}
 void runTest() { suite_MultiThreadedRandomTestSuite.testRandomDistribution(); }
} testDescription_MultiThreadedRandomTestSuite_testRandomDistribution;

