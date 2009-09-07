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

bool RayTestSuite_init = false;
#include "./Ray.test.h"

static RayTestSuite suite_RayTestSuite;

static CxxTest::List Tests_RayTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_RayTestSuite( "./Ray.test.h", 8, "RayTestSuite", suite_RayTestSuite, Tests_RayTestSuite );

static class TestDescription_RayTestSuite_testRayDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_RayTestSuite_testRayDefaultConstr() : CxxTest::RealTestDescription( Tests_RayTestSuite, suiteDescription_RayTestSuite, 11, "testRayDefaultConstr" ) {}
 void runTest() { suite_RayTestSuite.testRayDefaultConstr(); }
} testDescription_RayTestSuite_testRayDefaultConstr;

static class TestDescription_RayTestSuite_testRayConstrWithCoords : public CxxTest::RealTestDescription {
public:
 TestDescription_RayTestSuite_testRayConstrWithCoords() : CxxTest::RealTestDescription( Tests_RayTestSuite, suiteDescription_RayTestSuite, 23, "testRayConstrWithCoords" ) {}
 void runTest() { suite_RayTestSuite.testRayConstrWithCoords(); }
} testDescription_RayTestSuite_testRayConstrWithCoords;

static class TestDescription_RayTestSuite_testRayOperator : public CxxTest::RealTestDescription {
public:
 TestDescription_RayTestSuite_testRayOperator() : CxxTest::RealTestDescription( Tests_RayTestSuite, suiteDescription_RayTestSuite, 35, "testRayOperator" ) {}
 void runTest() { suite_RayTestSuite.testRayOperator(); }
} testDescription_RayTestSuite_testRayOperator;

static class TestDescription_RayTestSuite_testRayDifferentialDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_RayTestSuite_testRayDifferentialDefaultConstr() : CxxTest::RealTestDescription( Tests_RayTestSuite, suiteDescription_RayTestSuite, 45, "testRayDifferentialDefaultConstr" ) {}
 void runTest() { suite_RayTestSuite.testRayDifferentialDefaultConstr(); }
} testDescription_RayTestSuite_testRayDifferentialDefaultConstr;

static class TestDescription_RayTestSuite_testRayAndRayDifferentialMutualConstructors : public CxxTest::RealTestDescription {
public:
 TestDescription_RayTestSuite_testRayAndRayDifferentialMutualConstructors() : CxxTest::RealTestDescription( Tests_RayTestSuite, suiteDescription_RayTestSuite, 51, "testRayAndRayDifferentialMutualConstructors" ) {}
 void runTest() { suite_RayTestSuite.testRayAndRayDifferentialMutualConstructors(); }
} testDescription_RayTestSuite_testRayAndRayDifferentialMutualConstructors;

