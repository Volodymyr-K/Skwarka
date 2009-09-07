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

bool ThreadSafeRandomTestSuite_init = false;
#include "./ThreadSafeRandom.test.h"

static ThreadSafeRandomTestSuite suite_ThreadSafeRandomTestSuite;

static CxxTest::List Tests_ThreadSafeRandomTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ThreadSafeRandomTestSuite( "./ThreadSafeRandom.test.h", 9, "ThreadSafeRandomTestSuite", suite_ThreadSafeRandomTestSuite, Tests_ThreadSafeRandomTestSuite );

static class TestDescription_ThreadSafeRandomTestSuite_testRandomDoubleRange1 : public CxxTest::RealTestDescription {
public:
 TestDescription_ThreadSafeRandomTestSuite_testRandomDoubleRange1() : CxxTest::RealTestDescription( Tests_ThreadSafeRandomTestSuite, suiteDescription_ThreadSafeRandomTestSuite, 12, "testRandomDoubleRange1" ) {}
 void runTest() { suite_ThreadSafeRandomTestSuite.testRandomDoubleRange1(); }
} testDescription_ThreadSafeRandomTestSuite_testRandomDoubleRange1;

static class TestDescription_ThreadSafeRandomTestSuite_testRandomDoubleRange2 : public CxxTest::RealTestDescription {
public:
 TestDescription_ThreadSafeRandomTestSuite_testRandomDoubleRange2() : CxxTest::RealTestDescription( Tests_ThreadSafeRandomTestSuite, suiteDescription_ThreadSafeRandomTestSuite, 24, "testRandomDoubleRange2" ) {}
 void runTest() { suite_ThreadSafeRandomTestSuite.testRandomDoubleRange2(); }
} testDescription_ThreadSafeRandomTestSuite_testRandomDoubleRange2;

static class TestDescription_ThreadSafeRandomTestSuite_testRandomIntRange1 : public CxxTest::RealTestDescription {
public:
 TestDescription_ThreadSafeRandomTestSuite_testRandomIntRange1() : CxxTest::RealTestDescription( Tests_ThreadSafeRandomTestSuite, suiteDescription_ThreadSafeRandomTestSuite, 37, "testRandomIntRange1" ) {}
 void runTest() { suite_ThreadSafeRandomTestSuite.testRandomIntRange1(); }
} testDescription_ThreadSafeRandomTestSuite_testRandomIntRange1;

static class TestDescription_ThreadSafeRandomTestSuite_testRandomIntRange2 : public CxxTest::RealTestDescription {
public:
 TestDescription_ThreadSafeRandomTestSuite_testRandomIntRange2() : CxxTest::RealTestDescription( Tests_ThreadSafeRandomTestSuite, suiteDescription_ThreadSafeRandomTestSuite, 49, "testRandomIntRange2" ) {}
 void runTest() { suite_ThreadSafeRandomTestSuite.testRandomIntRange2(); }
} testDescription_ThreadSafeRandomTestSuite_testRandomIntRange2;

static class TestDescription_ThreadSafeRandomTestSuite_testRandomDistribution : public CxxTest::RealTestDescription {
public:
 TestDescription_ThreadSafeRandomTestSuite_testRandomDistribution() : CxxTest::RealTestDescription( Tests_ThreadSafeRandomTestSuite, suiteDescription_ThreadSafeRandomTestSuite, 63, "testRandomDistribution" ) {}
 void runTest() { suite_ThreadSafeRandomTestSuite.testRandomDistribution(); }
} testDescription_ThreadSafeRandomTestSuite_testRandomDistribution;

