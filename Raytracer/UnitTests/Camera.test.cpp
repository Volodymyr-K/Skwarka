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

bool CameraTestSuite_init = false;
#include "./Camera.test.h"

static CameraTestSuite suite_CameraTestSuite;

static CxxTest::List Tests_CameraTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CameraTestSuite( "./Camera.test.h", 12, "CameraTestSuite", suite_CameraTestSuite, Tests_CameraTestSuite );

static class TestDescription_CameraTestSuite_test_Camera_Constr : public CxxTest::RealTestDescription {
public:
 TestDescription_CameraTestSuite_test_Camera_Constr() : CxxTest::RealTestDescription( Tests_CameraTestSuite, suiteDescription_CameraTestSuite, 15, "test_Camera_Constr" ) {}
 void runTest() { suite_CameraTestSuite.test_Camera_Constr(); }
} testDescription_CameraTestSuite_test_Camera_Constr;
