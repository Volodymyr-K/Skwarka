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
CxxTest::StaticSuiteDescription suiteDescription_PerspectiveCameraTestSuite( "./PerspectiveCamera.test.h", 11, "PerspectiveCameraTestSuite", suite_PerspectiveCameraTestSuite, Tests_PerspectiveCameraTestSuite );

static class TestDescription_PerspectiveCameraTestSuite_testPerspectivePinHoleCameraTransformation : public CxxTest::RealTestDescription {
public:
 TestDescription_PerspectiveCameraTestSuite_testPerspectivePinHoleCameraTransformation() : CxxTest::RealTestDescription( Tests_PerspectiveCameraTestSuite, suiteDescription_PerspectiveCameraTestSuite, 31, "testPerspectivePinHoleCameraTransformation" ) {}
 void runTest() { suite_PerspectiveCameraTestSuite.testPerspectivePinHoleCameraTransformation(); }
} testDescription_PerspectiveCameraTestSuite_testPerspectivePinHoleCameraTransformation;

static class TestDescription_PerspectiveCameraTestSuite_testPerspectivePinHoleCameraRange : public CxxTest::RealTestDescription {
public:
 TestDescription_PerspectiveCameraTestSuite_testPerspectivePinHoleCameraRange() : CxxTest::RealTestDescription( Tests_PerspectiveCameraTestSuite, suiteDescription_PerspectiveCameraTestSuite, 44, "testPerspectivePinHoleCameraRange" ) {}
 void runTest() { suite_PerspectiveCameraTestSuite.testPerspectivePinHoleCameraRange(); }
} testDescription_PerspectiveCameraTestSuite_testPerspectivePinHoleCameraRange;

static class TestDescription_PerspectiveCameraTestSuite_testPerspectiveCameraDepthOfField : public CxxTest::RealTestDescription {
public:
 TestDescription_PerspectiveCameraTestSuite_testPerspectiveCameraDepthOfField() : CxxTest::RealTestDescription( Tests_PerspectiveCameraTestSuite, suiteDescription_PerspectiveCameraTestSuite, 70, "testPerspectiveCameraDepthOfField" ) {}
 void runTest() { suite_PerspectiveCameraTestSuite.testPerspectiveCameraDepthOfField(); }
} testDescription_PerspectiveCameraTestSuite_testPerspectiveCameraDepthOfField;

