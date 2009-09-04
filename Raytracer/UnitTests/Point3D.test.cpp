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

bool Point3DTestSuite_init = false;
#include "./Point3D.test.h"

static Point3DTestSuite suite_Point3DTestSuite;

static CxxTest::List Tests_Point3DTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Point3DTestSuite( "./Point3D.test.h", 9, "Point3DTestSuite", suite_Point3DTestSuite, Tests_Point3DTestSuite );

static class TestDescription_Point3DTestSuite_testPointDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointDefaultConstr() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 12, "testPointDefaultConstr" ) {}
 void runTest() { suite_Point3DTestSuite.testPointDefaultConstr(); }
} testDescription_Point3DTestSuite_testPointDefaultConstr;

static class TestDescription_Point3DTestSuite_testPointConstrWithCoords : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointConstrWithCoords() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 18, "testPointConstrWithCoords" ) {}
 void runTest() { suite_Point3DTestSuite.testPointConstrWithCoords(); }
} testDescription_Point3DTestSuite_testPointConstrWithCoords;

static class TestDescription_Point3DTestSuite_testPointAdd : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointAdd() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 24, "testPointAdd" ) {}
 void runTest() { suite_Point3DTestSuite.testPointAdd(); }
} testDescription_Point3DTestSuite_testPointAdd;

static class TestDescription_Point3DTestSuite_testPointAddAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointAddAssign() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 32, "testPointAddAssign" ) {}
 void runTest() { suite_Point3DTestSuite.testPointAddAssign(); }
} testDescription_Point3DTestSuite_testPointAddAssign;

static class TestDescription_Point3DTestSuite_testPointSub : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointSub() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 41, "testPointSub" ) {}
 void runTest() { suite_Point3DTestSuite.testPointSub(); }
} testDescription_Point3DTestSuite_testPointSub;

static class TestDescription_Point3DTestSuite_testPointSubAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointSubAssign() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 49, "testPointSubAssign" ) {}
 void runTest() { suite_Point3DTestSuite.testPointSubAssign(); }
} testDescription_Point3DTestSuite_testPointSubAssign;

static class TestDescription_Point3DTestSuite_testPointMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointMult() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 58, "testPointMult" ) {}
 void runTest() { suite_Point3DTestSuite.testPointMult(); }
} testDescription_Point3DTestSuite_testPointMult;

static class TestDescription_Point3DTestSuite_testPointPreMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointPreMult() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 65, "testPointPreMult" ) {}
 void runTest() { suite_Point3DTestSuite.testPointPreMult(); }
} testDescription_Point3DTestSuite_testPointPreMult;

static class TestDescription_Point3DTestSuite_testPointMultAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointMultAssign() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 72, "testPointMultAssign" ) {}
 void runTest() { suite_Point3DTestSuite.testPointMultAssign(); }
} testDescription_Point3DTestSuite_testPointMultAssign;

static class TestDescription_Point3DTestSuite_testPointDiv : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointDiv() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 80, "testPointDiv" ) {}
 void runTest() { suite_Point3DTestSuite.testPointDiv(); }
} testDescription_Point3DTestSuite_testPointDiv;

static class TestDescription_Point3DTestSuite_testPointDivAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointDivAssign() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 87, "testPointDivAssign" ) {}
 void runTest() { suite_Point3DTestSuite.testPointDivAssign(); }
} testDescription_Point3DTestSuite_testPointDivAssign;

static class TestDescription_Point3DTestSuite_testPointDivByZero : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointDivByZero() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 98, "testPointDivByZero" ) {}
 void runTest() { suite_Point3DTestSuite.testPointDivByZero(); }
} testDescription_Point3DTestSuite_testPointDivByZero;

static class TestDescription_Point3DTestSuite_testPointEqual : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointEqual() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 106, "testPointEqual" ) {}
 void runTest() { suite_Point3DTestSuite.testPointEqual(); }
} testDescription_Point3DTestSuite_testPointEqual;

static class TestDescription_Point3DTestSuite_testPointNotEqual : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointNotEqual() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 114, "testPointNotEqual" ) {}
 void runTest() { suite_Point3DTestSuite.testPointNotEqual(); }
} testDescription_Point3DTestSuite_testPointNotEqual;

static class TestDescription_Point3DTestSuite_testPointIndexOperator : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointIndexOperator() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 122, "testPointIndexOperator" ) {}
 void runTest() { suite_Point3DTestSuite.testPointIndexOperator(); }
} testDescription_Point3DTestSuite_testPointIndexOperator;

static class TestDescription_Point3DTestSuite_testPointIntegerMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointIntegerMult() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 128, "testPointIntegerMult" ) {}
 void runTest() { suite_Point3DTestSuite.testPointIntegerMult(); }
} testDescription_Point3DTestSuite_testPointIntegerMult;

static class TestDescription_Point3DTestSuite_testPointIntegerDiv : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointIntegerDiv() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 135, "testPointIntegerDiv" ) {}
 void runTest() { suite_Point3DTestSuite.testPointIntegerDiv(); }
} testDescription_Point3DTestSuite_testPointIntegerDiv;

static class TestDescription_Point3DTestSuite_testPointOutputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointOutputStream() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 142, "testPointOutputStream" ) {}
 void runTest() { suite_Point3DTestSuite.testPointOutputStream(); }
} testDescription_Point3DTestSuite_testPointOutputStream;

static class TestDescription_Point3DTestSuite_testPointInputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointInputStream() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 151, "testPointInputStream" ) {}
 void runTest() { suite_Point3DTestSuite.testPointInputStream(); }
} testDescription_Point3DTestSuite_testPointInputStream;

static class TestDescription_Point3DTestSuite_testPointConversion : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_testPointConversion() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 160, "testPointConversion" ) {}
 void runTest() { suite_Point3DTestSuite.testPointConversion(); }
} testDescription_Point3DTestSuite_testPointConversion;

