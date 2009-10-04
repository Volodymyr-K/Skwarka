/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./PointLight.test.h"

static PointLightTestSuite suite_PointLightTestSuite;

static CxxTest::List Tests_PointLightTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_PointLightTestSuite( "./PointLight.test.h", 10, "PointLightTestSuite", suite_PointLightTestSuite, Tests_PointLightTestSuite );

static class TestDescription_PointLightTestSuite_test_PointLight_Power : public CxxTest::RealTestDescription {
public:
 TestDescription_PointLightTestSuite_test_PointLight_Power() : CxxTest::RealTestDescription( Tests_PointLightTestSuite, suiteDescription_PointLightTestSuite, 13, "test_PointLight_Power" ) {}
 void runTest() { suite_PointLightTestSuite.test_PointLight_Power(); }
} testDescription_PointLightTestSuite_test_PointLight_Power;

static class TestDescription_PointLightTestSuite_test_PointLight_Lighting : public CxxTest::RealTestDescription {
public:
 TestDescription_PointLightTestSuite_test_PointLight_Lighting() : CxxTest::RealTestDescription( Tests_PointLightTestSuite, suiteDescription_PointLightTestSuite, 21, "test_PointLight_Lighting" ) {}
 void runTest() { suite_PointLightTestSuite.test_PointLight_Lighting(); }
} testDescription_PointLightTestSuite_test_PointLight_Lighting;

static class TestDescription_PointLightTestSuite_test_PointLight_SamplePhoton : public CxxTest::RealTestDescription {
public:
 TestDescription_PointLightTestSuite_test_PointLight_SamplePhoton() : CxxTest::RealTestDescription( Tests_PointLightTestSuite, suiteDescription_PointLightTestSuite, 38, "test_PointLight_SamplePhoton" ) {}
 void runTest() { suite_PointLightTestSuite.test_PointLight_SamplePhoton(); }
} testDescription_PointLightTestSuite_test_PointLight_SamplePhoton;

