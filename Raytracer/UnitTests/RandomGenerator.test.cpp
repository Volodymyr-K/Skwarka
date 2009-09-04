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

bool RandomGeneratorTestSuite_init = false;
#include "./RandomGenerator.test.h"

static RandomGeneratorTestSuite suite_RandomGeneratorTestSuite;

static CxxTest::List Tests_RandomGeneratorTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_RandomGeneratorTestSuite( "./RandomGenerator.test.h", 8, "RandomGeneratorTestSuite", suite_RandomGeneratorTestSuite, Tests_RandomGeneratorTestSuite );

static class TestDescription_RandomGeneratorTestSuite_testRandomRange1 : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomGeneratorTestSuite_testRandomRange1() : CxxTest::RealTestDescription( Tests_RandomGeneratorTestSuite, suiteDescription_RandomGeneratorTestSuite, 21, "testRandomRange1" ) {}
 void runTest() { suite_RandomGeneratorTestSuite.testRandomRange1(); }
} testDescription_RandomGeneratorTestSuite_testRandomRange1;

static class TestDescription_RandomGeneratorTestSuite_testRandomRange2 : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomGeneratorTestSuite_testRandomRange2() : CxxTest::RealTestDescription( Tests_RandomGeneratorTestSuite, suiteDescription_RandomGeneratorTestSuite, 33, "testRandomRange2" ) {}
 void runTest() { suite_RandomGeneratorTestSuite.testRandomRange2(); }
} testDescription_RandomGeneratorTestSuite_testRandomRange2;

static class TestDescription_RandomGeneratorTestSuite_testRandomDistribution : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomGeneratorTestSuite_testRandomDistribution() : CxxTest::RealTestDescription( Tests_RandomGeneratorTestSuite, suiteDescription_RandomGeneratorTestSuite, 47, "testRandomDistribution" ) {}
 void runTest() { suite_RandomGeneratorTestSuite.testRandomDistribution(); }
} testDescription_RandomGeneratorTestSuite_testRandomDistribution;

static class TestDescription_RandomGeneratorTestSuite_testCorrelation : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomGeneratorTestSuite_testCorrelation() : CxxTest::RealTestDescription( Tests_RandomGeneratorTestSuite, suiteDescription_RandomGeneratorTestSuite, 68, "testCorrelation" ) {}
 void runTest() { suite_RandomGeneratorTestSuite.testCorrelation(); }
} testDescription_RandomGeneratorTestSuite_testCorrelation;

static class TestDescription_RandomGeneratorTestSuite_testSeed : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomGeneratorTestSuite_testSeed() : CxxTest::RealTestDescription( Tests_RandomGeneratorTestSuite, suiteDescription_RandomGeneratorTestSuite, 82, "testSeed" ) {}
 void runTest() { suite_RandomGeneratorTestSuite.testSeed(); }
} testDescription_RandomGeneratorTestSuite_testSeed;

