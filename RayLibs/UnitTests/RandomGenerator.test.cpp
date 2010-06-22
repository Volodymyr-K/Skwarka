/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./RandomGenerator.test.h"

static RandomGeneratorTestSuite suite_RandomGeneratorTestSuite;

static CxxTest::List Tests_RandomGeneratorTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_RandomGeneratorTestSuite( "./RandomGenerator.test.h", 10, "RandomGeneratorTestSuite", suite_RandomGeneratorTestSuite, Tests_RandomGeneratorTestSuite );

static class TestDescription_RandomGeneratorTestSuite_test_RandomRange1 : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomGeneratorTestSuite_test_RandomRange1() : CxxTest::RealTestDescription( Tests_RandomGeneratorTestSuite, suiteDescription_RandomGeneratorTestSuite, 23, "test_RandomRange1" ) {}
 void runTest() { suite_RandomGeneratorTestSuite.test_RandomRange1(); }
} testDescription_RandomGeneratorTestSuite_test_RandomRange1;

static class TestDescription_RandomGeneratorTestSuite_test_RandomRange2 : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomGeneratorTestSuite_test_RandomRange2() : CxxTest::RealTestDescription( Tests_RandomGeneratorTestSuite, suiteDescription_RandomGeneratorTestSuite, 35, "test_RandomRange2" ) {}
 void runTest() { suite_RandomGeneratorTestSuite.test_RandomRange2(); }
} testDescription_RandomGeneratorTestSuite_test_RandomRange2;

static class TestDescription_RandomGeneratorTestSuite_test_RandomDistribution : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomGeneratorTestSuite_test_RandomDistribution() : CxxTest::RealTestDescription( Tests_RandomGeneratorTestSuite, suiteDescription_RandomGeneratorTestSuite, 49, "test_RandomDistribution" ) {}
 void runTest() { suite_RandomGeneratorTestSuite.test_RandomDistribution(); }
} testDescription_RandomGeneratorTestSuite_test_RandomDistribution;

static class TestDescription_RandomGeneratorTestSuite_test_Correlation : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomGeneratorTestSuite_test_Correlation() : CxxTest::RealTestDescription( Tests_RandomGeneratorTestSuite, suiteDescription_RandomGeneratorTestSuite, 60, "test_Correlation" ) {}
 void runTest() { suite_RandomGeneratorTestSuite.test_Correlation(); }
} testDescription_RandomGeneratorTestSuite_test_Correlation;

static class TestDescription_RandomGeneratorTestSuite_test_Seed : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomGeneratorTestSuite_test_Seed() : CxxTest::RealTestDescription( Tests_RandomGeneratorTestSuite, suiteDescription_RandomGeneratorTestSuite, 74, "test_Seed" ) {}
 void runTest() { suite_RandomGeneratorTestSuite.test_Seed(); }
} testDescription_RandomGeneratorTestSuite_test_Seed;

