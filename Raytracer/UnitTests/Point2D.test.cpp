/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_LONGLONG long long
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>

bool Point2DTestSuite_init = false;
#include "./Point2D.test.h"

static Point2DTestSuite suite_Point2DTestSuite;

static CxxTest::List Tests_Point2DTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Point2DTestSuite( "./Point2D.test.h", 9, "Point2DTestSuite", suite_Point2DTestSuite, Tests_Point2DTestSuite );

static class TestDescription_Point2DTestSuite_testPointDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointDefaultConstr() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 12, "testPointDefaultConstr" ) {}
 void runTest() { suite_Point2DTestSuite.testPointDefaultConstr(); }
} testDescription_Point2DTestSuite_testPointDefaultConstr;

static class TestDescription_Point2DTestSuite_testPointConstrWithCoords : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointConstrWithCoords() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 18, "testPointConstrWithCoords" ) {}
 void runTest() { suite_Point2DTestSuite.testPointConstrWithCoords(); }
} testDescription_Point2DTestSuite_testPointConstrWithCoords;

static class TestDescription_Point2DTestSuite_testPointAdd : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointAdd() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 24, "testPointAdd" ) {}
 void runTest() { suite_Point2DTestSuite.testPointAdd(); }
} testDescription_Point2DTestSuite_testPointAdd;

static class TestDescription_Point2DTestSuite_testPointAddAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointAddAssign() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 32, "testPointAddAssign" ) {}
 void runTest() { suite_Point2DTestSuite.testPointAddAssign(); }
} testDescription_Point2DTestSuite_testPointAddAssign;

static class TestDescription_Point2DTestSuite_testPointSub : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointSub() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 41, "testPointSub" ) {}
 void runTest() { suite_Point2DTestSuite.testPointSub(); }
} testDescription_Point2DTestSuite_testPointSub;

static class TestDescription_Point2DTestSuite_testPointSubAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointSubAssign() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 49, "testPointSubAssign" ) {}
 void runTest() { suite_Point2DTestSuite.testPointSubAssign(); }
} testDescription_Point2DTestSuite_testPointSubAssign;

static class TestDescription_Point2DTestSuite_testPointMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointMult() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 58, "testPointMult" ) {}
 void runTest() { suite_Point2DTestSuite.testPointMult(); }
} testDescription_Point2DTestSuite_testPointMult;

static class TestDescription_Point2DTestSuite_testPointPreMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointPreMult() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 65, "testPointPreMult" ) {}
 void runTest() { suite_Point2DTestSuite.testPointPreMult(); }
} testDescription_Point2DTestSuite_testPointPreMult;

static class TestDescription_Point2DTestSuite_testPointMultAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointMultAssign() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 72, "testPointMultAssign" ) {}
 void runTest() { suite_Point2DTestSuite.testPointMultAssign(); }
} testDescription_Point2DTestSuite_testPointMultAssign;

static class TestDescription_Point2DTestSuite_testPointDiv : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointDiv() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 80, "testPointDiv" ) {}
 void runTest() { suite_Point2DTestSuite.testPointDiv(); }
} testDescription_Point2DTestSuite_testPointDiv;

static class TestDescription_Point2DTestSuite_testPointDivAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointDivAssign() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 87, "testPointDivAssign" ) {}
 void runTest() { suite_Point2DTestSuite.testPointDivAssign(); }
} testDescription_Point2DTestSuite_testPointDivAssign;

static class TestDescription_Point2DTestSuite_testPointDivByZero : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointDivByZero() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 98, "testPointDivByZero" ) {}
 void runTest() { suite_Point2DTestSuite.testPointDivByZero(); }
} testDescription_Point2DTestSuite_testPointDivByZero;

static class TestDescription_Point2DTestSuite_testPointEqual : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointEqual() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 106, "testPointEqual" ) {}
 void runTest() { suite_Point2DTestSuite.testPointEqual(); }
} testDescription_Point2DTestSuite_testPointEqual;

static class TestDescription_Point2DTestSuite_testPointNotEqual : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointNotEqual() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 114, "testPointNotEqual" ) {}
 void runTest() { suite_Point2DTestSuite.testPointNotEqual(); }
} testDescription_Point2DTestSuite_testPointNotEqual;

static class TestDescription_Point2DTestSuite_testPointIndexOperator : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointIndexOperator() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 122, "testPointIndexOperator" ) {}
 void runTest() { suite_Point2DTestSuite.testPointIndexOperator(); }
} testDescription_Point2DTestSuite_testPointIndexOperator;

static class TestDescription_Point2DTestSuite_testPointIntegerMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointIntegerMult() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 128, "testPointIntegerMult" ) {}
 void runTest() { suite_Point2DTestSuite.testPointIntegerMult(); }
} testDescription_Point2DTestSuite_testPointIntegerMult;

static class TestDescription_Point2DTestSuite_testPointIntegerDiv : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointIntegerDiv() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 135, "testPointIntegerDiv" ) {}
 void runTest() { suite_Point2DTestSuite.testPointIntegerDiv(); }
} testDescription_Point2DTestSuite_testPointIntegerDiv;

static class TestDescription_Point2DTestSuite_testPointOutputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointOutputStream() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 142, "testPointOutputStream" ) {}
 void runTest() { suite_Point2DTestSuite.testPointOutputStream(); }
} testDescription_Point2DTestSuite_testPointOutputStream;

static class TestDescription_Point2DTestSuite_testPointInputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointInputStream() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 151, "testPointInputStream" ) {}
 void runTest() { suite_Point2DTestSuite.testPointInputStream(); }
} testDescription_Point2DTestSuite_testPointInputStream;

static class TestDescription_Point2DTestSuite_testPointConversion : public CxxTest::RealTestDescription {
public:
 TestDescription_Point2DTestSuite_testPointConversion() : CxxTest::RealTestDescription( Tests_Point2DTestSuite, suiteDescription_Point2DTestSuite, 160, "testPointConversion" ) {}
 void runTest() { suite_Point2DTestSuite.testPointConversion(); }
} testDescription_Point2DTestSuite_testPointConversion;

