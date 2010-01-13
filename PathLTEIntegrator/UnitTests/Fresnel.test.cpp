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

static class TestDescription_FresnelTestSuite_test_FresnelDielectric_Range : public CxxTest::RealTestDescription {
public:
 TestDescription_FresnelTestSuite_test_FresnelDielectric_Range() : CxxTest::RealTestDescription( Tests_FresnelTestSuite, suiteDescription_FresnelTestSuite, 14, "test_FresnelDielectric_Range" ) {}
 void runTest() { suite_FresnelTestSuite.test_FresnelDielectric_Range(); }
} testDescription_FresnelTestSuite_test_FresnelDielectric_Range;

static class TestDescription_FresnelTestSuite_test_FresnelDielectric_TotalInternalReflection : public CxxTest::RealTestDescription {
public:
 TestDescription_FresnelTestSuite_test_FresnelDielectric_TotalInternalReflection() : CxxTest::RealTestDescription( Tests_FresnelTestSuite, suiteDescription_FresnelTestSuite, 30, "test_FresnelDielectric_TotalInternalReflection" ) {}
 void runTest() { suite_FresnelTestSuite.test_FresnelDielectric_TotalInternalReflection(); }
} testDescription_FresnelTestSuite_test_FresnelDielectric_TotalInternalReflection;

static class TestDescription_FresnelTestSuite_test_FresnelConductor_Range : public CxxTest::RealTestDescription {
public:
 TestDescription_FresnelTestSuite_test_FresnelConductor_Range() : CxxTest::RealTestDescription( Tests_FresnelTestSuite, suiteDescription_FresnelTestSuite, 41, "test_FresnelConductor_Range" ) {}
 void runTest() { suite_FresnelTestSuite.test_FresnelConductor_Range(); }
} testDescription_FresnelTestSuite_test_FresnelConductor_Range;

static class TestDescription_FresnelTestSuite_test_FresnelConductor_Limit : public CxxTest::RealTestDescription {
public:
 TestDescription_FresnelTestSuite_test_FresnelConductor_Limit() : CxxTest::RealTestDescription( Tests_FresnelTestSuite, suiteDescription_FresnelTestSuite, 57, "test_FresnelConductor_Limit" ) {}
 void runTest() { suite_FresnelTestSuite.test_FresnelConductor_Limit(); }
} testDescription_FresnelTestSuite_test_FresnelConductor_Limit;

static class TestDescription_FresnelTestSuite_test_ApproximateFresnelParameters : public CxxTest::RealTestDescription {
public:
 TestDescription_FresnelTestSuite_test_ApproximateFresnelParameters() : CxxTest::RealTestDescription( Tests_FresnelTestSuite, suiteDescription_FresnelTestSuite, 66, "test_ApproximateFresnelParameters" ) {}
 void runTest() { suite_FresnelTestSuite.test_ApproximateFresnelParameters(); }
} testDescription_FresnelTestSuite_test_ApproximateFresnelParameters;

