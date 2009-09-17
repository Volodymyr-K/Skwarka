/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./ThreadSafeRandom.test.h"

static ThreadSafeRandomTestSuite suite_ThreadSafeRandomTestSuite;

static CxxTest::List Tests_ThreadSafeRandomTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ThreadSafeRandomTestSuite( "./ThreadSafeRandom.test.h", 9, "ThreadSafeRandomTestSuite", suite_ThreadSafeRandomTestSuite, Tests_ThreadSafeRandomTestSuite );

static class TestDescription_ThreadSafeRandomTestSuite_test_RandomDoubleRange1 : public CxxTest::RealTestDescription {
public:
 TestDescription_ThreadSafeRandomTestSuite_test_RandomDoubleRange1() : CxxTest::RealTestDescription( Tests_ThreadSafeRandomTestSuite, suiteDescription_ThreadSafeRandomTestSuite, 12, "test_RandomDoubleRange1" ) {}
 void runTest() { suite_ThreadSafeRandomTestSuite.test_RandomDoubleRange1(); }
} testDescription_ThreadSafeRandomTestSuite_test_RandomDoubleRange1;

static class TestDescription_ThreadSafeRandomTestSuite_test_RandomDoubleRange2 : public CxxTest::RealTestDescription {
public:
 TestDescription_ThreadSafeRandomTestSuite_test_RandomDoubleRange2() : CxxTest::RealTestDescription( Tests_ThreadSafeRandomTestSuite, suiteDescription_ThreadSafeRandomTestSuite, 24, "test_RandomDoubleRange2" ) {}
 void runTest() { suite_ThreadSafeRandomTestSuite.test_RandomDoubleRange2(); }
} testDescription_ThreadSafeRandomTestSuite_test_RandomDoubleRange2;

static class TestDescription_ThreadSafeRandomTestSuite_test_RandomIntRange1 : public CxxTest::RealTestDescription {
public:
 TestDescription_ThreadSafeRandomTestSuite_test_RandomIntRange1() : CxxTest::RealTestDescription( Tests_ThreadSafeRandomTestSuite, suiteDescription_ThreadSafeRandomTestSuite, 37, "test_RandomIntRange1" ) {}
 void runTest() { suite_ThreadSafeRandomTestSuite.test_RandomIntRange1(); }
} testDescription_ThreadSafeRandomTestSuite_test_RandomIntRange1;

static class TestDescription_ThreadSafeRandomTestSuite_test_RandomIntRange2 : public CxxTest::RealTestDescription {
public:
 TestDescription_ThreadSafeRandomTestSuite_test_RandomIntRange2() : CxxTest::RealTestDescription( Tests_ThreadSafeRandomTestSuite, suiteDescription_ThreadSafeRandomTestSuite, 49, "test_RandomIntRange2" ) {}
 void runTest() { suite_ThreadSafeRandomTestSuite.test_RandomIntRange2(); }
} testDescription_ThreadSafeRandomTestSuite_test_RandomIntRange2;

static class TestDescription_ThreadSafeRandomTestSuite_test_RandomDistribution : public CxxTest::RealTestDescription {
public:
 TestDescription_ThreadSafeRandomTestSuite_test_RandomDistribution() : CxxTest::RealTestDescription( Tests_ThreadSafeRandomTestSuite, suiteDescription_ThreadSafeRandomTestSuite, 63, "test_RandomDistribution" ) {}
 void runTest() { suite_ThreadSafeRandomTestSuite.test_RandomDistribution(); }
} testDescription_ThreadSafeRandomTestSuite_test_RandomDistribution;

