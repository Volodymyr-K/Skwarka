/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Triangle3D.test.h"

static Triangle3DTestSuite suite_Triangle3DTestSuite;

static CxxTest::List Tests_Triangle3DTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Triangle3DTestSuite( "./Triangle3D.test.h", 9, "Triangle3DTestSuite", suite_Triangle3DTestSuite, Tests_Triangle3DTestSuite );

static class TestDescription_Triangle3DTestSuite_test_Triangle3D_DefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_test_Triangle3D_DefaultConstr() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 26, "test_Triangle3D_DefaultConstr" ) {}
 void runTest() { suite_Triangle3DTestSuite.test_Triangle3D_DefaultConstr(); }
} testDescription_Triangle3DTestSuite_test_Triangle3D_DefaultConstr;

static class TestDescription_Triangle3DTestSuite_test_Triangle3D_ConstrWithVertices : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_test_Triangle3D_ConstrWithVertices() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 34, "test_Triangle3D_ConstrWithVertices" ) {}
 void runTest() { suite_Triangle3DTestSuite.test_Triangle3D_ConstrWithVertices(); }
} testDescription_Triangle3DTestSuite_test_Triangle3D_ConstrWithVertices;

static class TestDescription_Triangle3DTestSuite_test_Triangle3D_Normal : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_test_Triangle3D_Normal() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 42, "test_Triangle3D_Normal" ) {}
 void runTest() { suite_Triangle3DTestSuite.test_Triangle3D_Normal(); }
} testDescription_Triangle3DTestSuite_test_Triangle3D_Normal;

static class TestDescription_Triangle3DTestSuite_test_Triangle3D_NormalOrientation : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_test_Triangle3D_NormalOrientation() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 49, "test_Triangle3D_NormalOrientation" ) {}
 void runTest() { suite_Triangle3DTestSuite.test_Triangle3D_NormalOrientation(); }
} testDescription_Triangle3DTestSuite_test_Triangle3D_NormalOrientation;

static class TestDescription_Triangle3DTestSuite_test_Triangle3D_Area : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_test_Triangle3D_Area() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 60, "test_Triangle3D_Area" ) {}
 void runTest() { suite_Triangle3DTestSuite.test_Triangle3D_Area(); }
} testDescription_Triangle3DTestSuite_test_Triangle3D_Area;

static class TestDescription_Triangle3DTestSuite_test_Triangle3D_IndexOperator : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_test_Triangle3D_IndexOperator() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 67, "test_Triangle3D_IndexOperator" ) {}
 void runTest() { suite_Triangle3DTestSuite.test_Triangle3D_IndexOperator(); }
} testDescription_Triangle3DTestSuite_test_Triangle3D_IndexOperator;

