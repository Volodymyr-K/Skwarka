/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Fresnel.test.h"

static FresnelTestSuite suite_FresnelTestSuite;

static CxxTest::List Tests_FresnelTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_FresnelTestSuite( "./Fresnel.test.h", 10, "FresnelTestSuite", suite_FresnelTestSuite, Tests_FresnelTestSuite );

static class TestDescription_FresnelTestSuite_test_Fresnel_DielectricRange : public CxxTest::RealTestDescription {
public:
 TestDescription_FresnelTestSuite_test_Fresnel_DielectricRange() : CxxTest::RealTestDescription( Tests_FresnelTestSuite, suiteDescription_FresnelTestSuite, 14, "test_Fresnel_DielectricRange" ) {}
 void runTest() { suite_FresnelTestSuite.test_Fresnel_DielectricRange(); }
} testDescription_FresnelTestSuite_test_Fresnel_DielectricRange;

static class TestDescription_FresnelTestSuite_test_Fresnel_DielectricTotalInternalReflection : public CxxTest::RealTestDescription {
public:
 TestDescription_FresnelTestSuite_test_Fresnel_DielectricTotalInternalReflection() : CxxTest::RealTestDescription( Tests_FresnelTestSuite, suiteDescription_FresnelTestSuite, 28, "test_Fresnel_DielectricTotalInternalReflection" ) {}
 void runTest() { suite_FresnelTestSuite.test_Fresnel_DielectricTotalInternalReflection(); }
} testDescription_FresnelTestSuite_test_Fresnel_DielectricTotalInternalReflection;

static class TestDescription_FresnelTestSuite_test_Fresnel_ConductorRange : public CxxTest::RealTestDescription {
public:
 TestDescription_FresnelTestSuite_test_Fresnel_ConductorRange() : CxxTest::RealTestDescription( Tests_FresnelTestSuite, suiteDescription_FresnelTestSuite, 37, "test_Fresnel_ConductorRange" ) {}
 void runTest() { suite_FresnelTestSuite.test_Fresnel_ConductorRange(); }
} testDescription_FresnelTestSuite_test_Fresnel_ConductorRange;

