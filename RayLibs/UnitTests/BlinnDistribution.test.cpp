/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./BlinnDistribution.test.h"

static BlinnDistributionTestSuite suite_BlinnDistributionTestSuite;

static CxxTest::List Tests_BlinnDistributionTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_BlinnDistributionTestSuite( "./BlinnDistribution.test.h", 12, "BlinnDistributionTestSuite", suite_BlinnDistributionTestSuite, Tests_BlinnDistributionTestSuite );

static class TestDescription_BlinnDistributionTestSuite_test_BlinnDistribution_EvaluateZeroExponent : public CxxTest::RealTestDescription {
public:
 TestDescription_BlinnDistributionTestSuite_test_BlinnDistribution_EvaluateZeroExponent() : CxxTest::RealTestDescription( Tests_BlinnDistributionTestSuite, suiteDescription_BlinnDistributionTestSuite, 15, "test_BlinnDistribution_EvaluateZeroExponent" ) {}
 void runTest() { suite_BlinnDistributionTestSuite.test_BlinnDistribution_EvaluateZeroExponent(); }
} testDescription_BlinnDistributionTestSuite_test_BlinnDistribution_EvaluateZeroExponent;

static class TestDescription_BlinnDistributionTestSuite_test_BlinnDistribution_Sample : public CxxTest::RealTestDescription {
public:
 TestDescription_BlinnDistributionTestSuite_test_BlinnDistribution_Sample() : CxxTest::RealTestDescription( Tests_BlinnDistributionTestSuite, suiteDescription_BlinnDistributionTestSuite, 22, "test_BlinnDistribution_Sample" ) {}
 void runTest() { suite_BlinnDistributionTestSuite.test_BlinnDistribution_Sample(); }
} testDescription_BlinnDistributionTestSuite_test_BlinnDistribution_Sample;

static class TestDescription_BlinnDistributionTestSuite_test_BlinnDistribution_SampleHighExponent : public CxxTest::RealTestDescription {
public:
 TestDescription_BlinnDistributionTestSuite_test_BlinnDistribution_SampleHighExponent() : CxxTest::RealTestDescription( Tests_BlinnDistributionTestSuite, suiteDescription_BlinnDistributionTestSuite, 43, "test_BlinnDistribution_SampleHighExponent" ) {}
 void runTest() { suite_BlinnDistributionTestSuite.test_BlinnDistribution_SampleHighExponent(); }
} testDescription_BlinnDistributionTestSuite_test_BlinnDistribution_SampleHighExponent;

static class TestDescription_BlinnDistributionTestSuite_test_BlinnDistribution_PDF : public CxxTest::RealTestDescription {
public:
 TestDescription_BlinnDistributionTestSuite_test_BlinnDistribution_PDF() : CxxTest::RealTestDescription( Tests_BlinnDistributionTestSuite, suiteDescription_BlinnDistributionTestSuite, 65, "test_BlinnDistribution_PDF" ) {}
 void runTest() { suite_BlinnDistributionTestSuite.test_BlinnDistribution_PDF(); }
} testDescription_BlinnDistributionTestSuite_test_BlinnDistribution_PDF;

