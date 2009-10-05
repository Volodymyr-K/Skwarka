/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./LightSources.test.h"

static LightSourcesTestSuite suite_LightSourcesTestSuite;

static CxxTest::List Tests_LightSourcesTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_LightSourcesTestSuite( "./LightSources.test.h", 12, "LightSourcesTestSuite", suite_LightSourcesTestSuite, Tests_LightSourcesTestSuite );

static class TestDescription_LightSourcesTestSuite_test_AreaLightSource_Radiance : public CxxTest::RealTestDescription {
public:
 TestDescription_LightSourcesTestSuite_test_AreaLightSource_Radiance() : CxxTest::RealTestDescription( Tests_LightSourcesTestSuite, suiteDescription_LightSourcesTestSuite, 27, "test_AreaLightSource_Radiance" ) {}
 void runTest() { suite_LightSourcesTestSuite.test_AreaLightSource_Radiance(); }
} testDescription_LightSourcesTestSuite_test_AreaLightSource_Radiance;

static class TestDescription_LightSourcesTestSuite_test_AreaLightSource_Power : public CxxTest::RealTestDescription {
public:
 TestDescription_LightSourcesTestSuite_test_AreaLightSource_Power() : CxxTest::RealTestDescription( Tests_LightSourcesTestSuite, suiteDescription_LightSourcesTestSuite, 33, "test_AreaLightSource_Power" ) {}
 void runTest() { suite_LightSourcesTestSuite.test_AreaLightSource_Power(); }
} testDescription_LightSourcesTestSuite_test_AreaLightSource_Power;

static class TestDescription_LightSourcesTestSuite_test_AreaLightSource_SampleLighting : public CxxTest::RealTestDescription {
public:
 TestDescription_LightSourcesTestSuite_test_AreaLightSource_SampleLighting() : CxxTest::RealTestDescription( Tests_LightSourcesTestSuite, suiteDescription_LightSourcesTestSuite, 43, "test_AreaLightSource_SampleLighting" ) {}
 void runTest() { suite_LightSourcesTestSuite.test_AreaLightSource_SampleLighting(); }
} testDescription_LightSourcesTestSuite_test_AreaLightSource_SampleLighting;

static class TestDescription_LightSourcesTestSuite_test_AreaLightSource_LightingPDF : public CxxTest::RealTestDescription {
public:
 TestDescription_LightSourcesTestSuite_test_AreaLightSource_LightingPDF() : CxxTest::RealTestDescription( Tests_LightSourcesTestSuite, suiteDescription_LightSourcesTestSuite, 81, "test_AreaLightSource_LightingPDF" ) {}
 void runTest() { suite_LightSourcesTestSuite.test_AreaLightSource_LightingPDF(); }
} testDescription_LightSourcesTestSuite_test_AreaLightSource_LightingPDF;

static class TestDescription_LightSourcesTestSuite_test_AreaLightSource_LightingPDFSum : public CxxTest::RealTestDescription {
public:
 TestDescription_LightSourcesTestSuite_test_AreaLightSource_LightingPDFSum() : CxxTest::RealTestDescription( Tests_LightSourcesTestSuite, suiteDescription_LightSourcesTestSuite, 107, "test_AreaLightSource_LightingPDFSum" ) {}
 void runTest() { suite_LightSourcesTestSuite.test_AreaLightSource_LightingPDFSum(); }
} testDescription_LightSourcesTestSuite_test_AreaLightSource_LightingPDFSum;

static class TestDescription_LightSourcesTestSuite_test_AreaLightSource_SamplePhoton : public CxxTest::RealTestDescription {
public:
 TestDescription_LightSourcesTestSuite_test_AreaLightSource_SamplePhoton() : CxxTest::RealTestDescription( Tests_LightSourcesTestSuite, suiteDescription_LightSourcesTestSuite, 136, "test_AreaLightSource_SamplePhoton" ) {}
 void runTest() { suite_LightSourcesTestSuite.test_AreaLightSource_SamplePhoton(); }
} testDescription_LightSourcesTestSuite_test_AreaLightSource_SamplePhoton;

