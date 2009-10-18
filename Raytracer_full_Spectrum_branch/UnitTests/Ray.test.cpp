/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Ray.test.h"

static RayTestSuite suite_RayTestSuite;

static CxxTest::List Tests_RayTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_RayTestSuite( "./Ray.test.h", 8, "RayTestSuite", suite_RayTestSuite, Tests_RayTestSuite );

static class TestDescription_RayTestSuite_test_Ray_DefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_RayTestSuite_test_Ray_DefaultConstr() : CxxTest::RealTestDescription( Tests_RayTestSuite, suiteDescription_RayTestSuite, 11, "test_Ray_DefaultConstr" ) {}
 void runTest() { suite_RayTestSuite.test_Ray_DefaultConstr(); }
} testDescription_RayTestSuite_test_Ray_DefaultConstr;

static class TestDescription_RayTestSuite_test_Ray_ConstrWithCoords : public CxxTest::RealTestDescription {
public:
 TestDescription_RayTestSuite_test_Ray_ConstrWithCoords() : CxxTest::RealTestDescription( Tests_RayTestSuite, suiteDescription_RayTestSuite, 23, "test_Ray_ConstrWithCoords" ) {}
 void runTest() { suite_RayTestSuite.test_Ray_ConstrWithCoords(); }
} testDescription_RayTestSuite_test_Ray_ConstrWithCoords;

static class TestDescription_RayTestSuite_test_Ray_Operator : public CxxTest::RealTestDescription {
public:
 TestDescription_RayTestSuite_test_Ray_Operator() : CxxTest::RealTestDescription( Tests_RayTestSuite, suiteDescription_RayTestSuite, 35, "test_Ray_Operator" ) {}
 void runTest() { suite_RayTestSuite.test_Ray_Operator(); }
} testDescription_RayTestSuite_test_Ray_Operator;

static class TestDescription_RayTestSuite_test_Ray_DifferentialDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_RayTestSuite_test_Ray_DifferentialDefaultConstr() : CxxTest::RealTestDescription( Tests_RayTestSuite, suiteDescription_RayTestSuite, 45, "test_Ray_DifferentialDefaultConstr" ) {}
 void runTest() { suite_RayTestSuite.test_Ray_DifferentialDefaultConstr(); }
} testDescription_RayTestSuite_test_Ray_DifferentialDefaultConstr;

static class TestDescription_RayTestSuite_test_Ray_RayDifferentialMutualConstructors : public CxxTest::RealTestDescription {
public:
 TestDescription_RayTestSuite_test_Ray_RayDifferentialMutualConstructors() : CxxTest::RealTestDescription( Tests_RayTestSuite, suiteDescription_RayTestSuite, 51, "test_Ray_RayDifferentialMutualConstructors" ) {}
 void runTest() { suite_RayTestSuite.test_Ray_RayDifferentialMutualConstructors(); }
} testDescription_RayTestSuite_test_Ray_RayDifferentialMutualConstructors;

