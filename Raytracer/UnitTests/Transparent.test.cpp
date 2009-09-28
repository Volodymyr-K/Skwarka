/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Transparent.test.h"

static TransparentTestSuite suite_TransparentTestSuite;

static CxxTest::List Tests_TransparentTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TransparentTestSuite( "./Transparent.test.h", 14, "TransparentTestSuite", suite_TransparentTestSuite, Tests_TransparentTestSuite );

static class TestDescription_TransparentTestSuite_test_Transparent_Components : public CxxTest::RealTestDescription {
public:
 TestDescription_TransparentTestSuite_test_Transparent_Components() : CxxTest::RealTestDescription( Tests_TransparentTestSuite, suiteDescription_TransparentTestSuite, 31, "test_Transparent_Components" ) {}
 void runTest() { suite_TransparentTestSuite.test_Transparent_Components(); }
} testDescription_TransparentTestSuite_test_Transparent_Components;

static class TestDescription_TransparentTestSuite_test_Transparent_RefractiveIndex : public CxxTest::RealTestDescription {
public:
 TestDescription_TransparentTestSuite_test_Transparent_RefractiveIndex() : CxxTest::RealTestDescription( Tests_TransparentTestSuite, suiteDescription_TransparentTestSuite, 44, "test_Transparent_RefractiveIndex" ) {}
 void runTest() { suite_TransparentTestSuite.test_Transparent_RefractiveIndex(); }
} testDescription_TransparentTestSuite_test_Transparent_RefractiveIndex;

static class TestDescription_TransparentTestSuite_test_Transparent_SpecularReflection : public CxxTest::RealTestDescription {
public:
 TestDescription_TransparentTestSuite_test_Transparent_SpecularReflection() : CxxTest::RealTestDescription( Tests_TransparentTestSuite, suiteDescription_TransparentTestSuite, 54, "test_Transparent_SpecularReflection" ) {}
 void runTest() { suite_TransparentTestSuite.test_Transparent_SpecularReflection(); }
} testDescription_TransparentTestSuite_test_Transparent_SpecularReflection;

static class TestDescription_TransparentTestSuite_test_Transparent_SpecularTransmittance : public CxxTest::RealTestDescription {
public:
 TestDescription_TransparentTestSuite_test_Transparent_SpecularTransmittance() : CxxTest::RealTestDescription( Tests_TransparentTestSuite, suiteDescription_TransparentTestSuite, 71, "test_Transparent_SpecularTransmittance" ) {}
 void runTest() { suite_TransparentTestSuite.test_Transparent_SpecularTransmittance(); }
} testDescription_TransparentTestSuite_test_Transparent_SpecularTransmittance;

