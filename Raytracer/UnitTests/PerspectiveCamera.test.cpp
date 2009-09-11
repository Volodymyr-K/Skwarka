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

bool PerspectiveCameraTestSuite_init = false;
#include "./PerspectiveCamera.test.h"

static PerspectiveCameraTestSuite suite_PerspectiveCameraTestSuite;

static CxxTest::List Tests_PerspectiveCameraTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_PerspectiveCameraTestSuite( "./PerspectiveCamera.test.h", 12, "PerspectiveCameraTestSuite", suite_PerspectiveCameraTestSuite, Tests_PerspectiveCameraTestSuite );

static class TestDescription_PerspectiveCameraTestSuite_test_PerspectiveCamera_PinHoleCameraTransformation : public CxxTest::RealTestDescription {
public:
 TestDescription_PerspectiveCameraTestSuite_test_PerspectiveCamera_PinHoleCameraTransformation() : CxxTest::RealTestDescription( Tests_PerspectiveCameraTestSuite, suiteDescription_PerspectiveCameraTestSuite, 32, "test_PerspectiveCamera_PinHoleCameraTransformation" ) {}
 void runTest() { suite_PerspectiveCameraTestSuite.test_PerspectiveCamera_PinHoleCameraTransformation(); }
} testDescription_PerspectiveCameraTestSuite_test_PerspectiveCamera_PinHoleCameraTransformation;

static class TestDescription_PerspectiveCameraTestSuite_test_PerspectiveCamera_PinHoleCameraRange : public CxxTest::RealTestDescription {
public:
 TestDescription_PerspectiveCameraTestSuite_test_PerspectiveCamera_PinHoleCameraRange() : CxxTest::RealTestDescription( Tests_PerspectiveCameraTestSuite, suiteDescription_PerspectiveCameraTestSuite, 45, "test_PerspectiveCamera_PinHoleCameraRange" ) {}
 void runTest() { suite_PerspectiveCameraTestSuite.test_PerspectiveCamera_PinHoleCameraRange(); }
} testDescription_PerspectiveCameraTestSuite_test_PerspectiveCamera_PinHoleCameraRange;

static class TestDescription_PerspectiveCameraTestSuite_test_PerspectiveCamera_DepthOfField : public CxxTest::RealTestDescription {
public:
 TestDescription_PerspectiveCameraTestSuite_test_PerspectiveCamera_DepthOfField() : CxxTest::RealTestDescription( Tests_PerspectiveCameraTestSuite, suiteDescription_PerspectiveCameraTestSuite, 71, "test_PerspectiveCamera_DepthOfField" ) {}
 void runTest() { suite_PerspectiveCameraTestSuite.test_PerspectiveCamera_DepthOfField(); }
} testDescription_PerspectiveCameraTestSuite_test_PerspectiveCamera_DepthOfField;
