/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./DirectLightingIntegrator.test.h"

static DirectLightingIntegratorTestSuite suite_DirectLightingIntegratorTestSuite;

static CxxTest::List Tests_DirectLightingIntegratorTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_DirectLightingIntegratorTestSuite( "./DirectLightingIntegrator.test.h", 16, "DirectLightingIntegratorTestSuite", suite_DirectLightingIntegratorTestSuite, Tests_DirectLightingIntegratorTestSuite );

static class TestDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_PointLight : public CxxTest::RealTestDescription {
public:
 TestDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_PointLight() : CxxTest::RealTestDescription( Tests_DirectLightingIntegratorTestSuite, suiteDescription_DirectLightingIntegratorTestSuite, 32, "test_DirectLightingIntegrator_PointLight" ) {}
 void runTest() { suite_DirectLightingIntegratorTestSuite.test_DirectLightingIntegrator_PointLight(); }
} testDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_PointLight;

static class TestDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_InfinityLights : public CxxTest::RealTestDescription {
public:
 TestDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_InfinityLights() : CxxTest::RealTestDescription( Tests_DirectLightingIntegratorTestSuite, suiteDescription_DirectLightingIntegratorTestSuite, 60, "test_DirectLightingIntegrator_InfinityLights" ) {}
 void runTest() { suite_DirectLightingIntegratorTestSuite.test_DirectLightingIntegrator_InfinityLights(); }
} testDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_InfinityLights;

static class TestDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_AreaLights : public CxxTest::RealTestDescription {
public:
 TestDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_AreaLights() : CxxTest::RealTestDescription( Tests_DirectLightingIntegratorTestSuite, suiteDescription_DirectLightingIntegratorTestSuite, 90, "test_DirectLightingIntegrator_AreaLights" ) {}
 void runTest() { suite_DirectLightingIntegratorTestSuite.test_DirectLightingIntegrator_AreaLights(); }
} testDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_AreaLights;

static class TestDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_InfinityAndAreaLights : public CxxTest::RealTestDescription {
public:
 TestDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_InfinityAndAreaLights() : CxxTest::RealTestDescription( Tests_DirectLightingIntegratorTestSuite, suiteDescription_DirectLightingIntegratorTestSuite, 124, "test_DirectLightingIntegrator_InfinityAndAreaLights" ) {}
 void runTest() { suite_DirectLightingIntegratorTestSuite.test_DirectLightingIntegrator_InfinityAndAreaLights(); }
} testDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_InfinityAndAreaLights;

static class TestDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_NoLighting : public CxxTest::RealTestDescription {
public:
 TestDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_NoLighting() : CxxTest::RealTestDescription( Tests_DirectLightingIntegratorTestSuite, suiteDescription_DirectLightingIntegratorTestSuite, 159, "test_DirectLightingIntegrator_NoLighting" ) {}
 void runTest() { suite_DirectLightingIntegratorTestSuite.test_DirectLightingIntegrator_NoLighting(); }
} testDescription_DirectLightingIntegratorTestSuite_test_DirectLightingIntegrator_NoLighting;

