/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./ParallelLight.test.h"

static ParallelLightTestSuite suite_ParallelLightTestSuite;

static CxxTest::List Tests_ParallelLightTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ParallelLightTestSuite( "./ParallelLight.test.h", 11, "ParallelLightTestSuite", suite_ParallelLightTestSuite, Tests_ParallelLightTestSuite );

static class TestDescription_ParallelLightTestSuite_test_ParallelLight_Power : public CxxTest::RealTestDescription {
public:
 TestDescription_ParallelLightTestSuite_test_ParallelLight_Power() : CxxTest::RealTestDescription( Tests_ParallelLightTestSuite, suiteDescription_ParallelLightTestSuite, 14, "test_ParallelLight_Power" ) {}
 void runTest() { suite_ParallelLightTestSuite.test_ParallelLight_Power(); }
} testDescription_ParallelLightTestSuite_test_ParallelLight_Power;

static class TestDescription_ParallelLightTestSuite_test_ParallelLight_Lighting : public CxxTest::RealTestDescription {
public:
 TestDescription_ParallelLightTestSuite_test_ParallelLight_Lighting() : CxxTest::RealTestDescription( Tests_ParallelLightTestSuite, suiteDescription_ParallelLightTestSuite, 24, "test_ParallelLight_Lighting" ) {}
 void runTest() { suite_ParallelLightTestSuite.test_ParallelLight_Lighting(); }
} testDescription_ParallelLightTestSuite_test_ParallelLight_Lighting;

static class TestDescription_ParallelLightTestSuite_test_ParallelLight_SamplePhoton : public CxxTest::RealTestDescription {
public:
 TestDescription_ParallelLightTestSuite_test_ParallelLight_SamplePhoton() : CxxTest::RealTestDescription( Tests_ParallelLightTestSuite, suiteDescription_ParallelLightTestSuite, 44, "test_ParallelLight_SamplePhoton" ) {}
 void runTest() { suite_ParallelLightTestSuite.test_ParallelLight_SamplePhoton(); }
} testDescription_ParallelLightTestSuite_test_ParallelLight_SamplePhoton;

