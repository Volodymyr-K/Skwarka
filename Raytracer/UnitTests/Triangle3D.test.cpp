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

bool Triangle3DTestSuite_init = false;
#include "./Triangle3D.test.h"

static Triangle3DTestSuite suite_Triangle3DTestSuite;

static CxxTest::List Tests_Triangle3DTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Triangle3DTestSuite( "./Triangle3D.test.h", 9, "Triangle3DTestSuite", suite_Triangle3DTestSuite, Tests_Triangle3DTestSuite );

static class TestDescription_Triangle3DTestSuite_testTriangleDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_testTriangleDefaultConstr() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 26, "testTriangleDefaultConstr" ) {}
 void runTest() { suite_Triangle3DTestSuite.testTriangleDefaultConstr(); }
} testDescription_Triangle3DTestSuite_testTriangleDefaultConstr;

static class TestDescription_Triangle3DTestSuite_testTriangleConstrWithVertices : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_testTriangleConstrWithVertices() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 34, "testTriangleConstrWithVertices" ) {}
 void runTest() { suite_Triangle3DTestSuite.testTriangleConstrWithVertices(); }
} testDescription_Triangle3DTestSuite_testTriangleConstrWithVertices;

static class TestDescription_Triangle3DTestSuite_testTriangleNormal : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_testTriangleNormal() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 42, "testTriangleNormal" ) {}
 void runTest() { suite_Triangle3DTestSuite.testTriangleNormal(); }
} testDescription_Triangle3DTestSuite_testTriangleNormal;

static class TestDescription_Triangle3DTestSuite_testTriangleNormalOrientation : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_testTriangleNormalOrientation() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 49, "testTriangleNormalOrientation" ) {}
 void runTest() { suite_Triangle3DTestSuite.testTriangleNormalOrientation(); }
} testDescription_Triangle3DTestSuite_testTriangleNormalOrientation;

static class TestDescription_Triangle3DTestSuite_testTriangleArea : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_testTriangleArea() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 60, "testTriangleArea" ) {}
 void runTest() { suite_Triangle3DTestSuite.testTriangleArea(); }
} testDescription_Triangle3DTestSuite_testTriangleArea;

static class TestDescription_Triangle3DTestSuite_testTriangleIndexOperator : public CxxTest::RealTestDescription {
public:
 TestDescription_Triangle3DTestSuite_testTriangleIndexOperator() : CxxTest::RealTestDescription( Tests_Triangle3DTestSuite, suiteDescription_Triangle3DTestSuite, 67, "testTriangleIndexOperator" ) {}
 void runTest() { suite_Triangle3DTestSuite.testTriangleIndexOperator(); }
} testDescription_Triangle3DTestSuite_testTriangleIndexOperator;

