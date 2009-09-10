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

static class TestDescription_Point3DTestSuite_test_Point3D_DefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_DefaultConstr() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 12, "test_Point3D_DefaultConstr" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_DefaultConstr(); }
} testDescription_Point3DTestSuite_test_Point3D_DefaultConstr;

static class TestDescription_Point3DTestSuite_test_Point3D_ConstrWithCoords : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_ConstrWithCoords() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 18, "test_Point3D_ConstrWithCoords" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_ConstrWithCoords(); }
} testDescription_Point3DTestSuite_test_Point3D_ConstrWithCoords;

static class TestDescription_Point3DTestSuite_test_Point3D_Add : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_Add() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 24, "test_Point3D_Add" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_Add(); }
} testDescription_Point3DTestSuite_test_Point3D_Add;

static class TestDescription_Point3DTestSuite_test_Point3D_AddAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_AddAssign() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 32, "test_Point3D_AddAssign" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_AddAssign(); }
} testDescription_Point3DTestSuite_test_Point3D_AddAssign;

static class TestDescription_Point3DTestSuite_test_Point3D_Sub : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_Sub() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 41, "test_Point3D_Sub" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_Sub(); }
} testDescription_Point3DTestSuite_test_Point3D_Sub;

static class TestDescription_Point3DTestSuite_test_Point3D_SubAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_SubAssign() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 49, "test_Point3D_SubAssign" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_SubAssign(); }
} testDescription_Point3DTestSuite_test_Point3D_SubAssign;

static class TestDescription_Point3DTestSuite_test_Point3D_Mult : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_Mult() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 58, "test_Point3D_Mult" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_Mult(); }
} testDescription_Point3DTestSuite_test_Point3D_Mult;

static class TestDescription_Point3DTestSuite_test_Point3D_PreMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_PreMult() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 65, "test_Point3D_PreMult" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_PreMult(); }
} testDescription_Point3DTestSuite_test_Point3D_PreMult;

static class TestDescription_Point3DTestSuite_test_Point3D_MultAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_MultAssign() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 72, "test_Point3D_MultAssign" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_MultAssign(); }
} testDescription_Point3DTestSuite_test_Point3D_MultAssign;

static class TestDescription_Point3DTestSuite_test_Point3D_Div : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_Div() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 80, "test_Point3D_Div" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_Div(); }
} testDescription_Point3DTestSuite_test_Point3D_Div;

static class TestDescription_Point3DTestSuite_test_Point3D_DivAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_DivAssign() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 87, "test_Point3D_DivAssign" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_DivAssign(); }
} testDescription_Point3DTestSuite_test_Point3D_DivAssign;

static class TestDescription_Point3DTestSuite_test_Point3D_DivByZero : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_DivByZero() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 98, "test_Point3D_DivByZero" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_DivByZero(); }
} testDescription_Point3DTestSuite_test_Point3D_DivByZero;

static class TestDescription_Point3DTestSuite_test_Point3D_Equal : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_Equal() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 106, "test_Point3D_Equal" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_Equal(); }
} testDescription_Point3DTestSuite_test_Point3D_Equal;

static class TestDescription_Point3DTestSuite_test_Point3D_NotEqual : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_NotEqual() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 114, "test_Point3D_NotEqual" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_NotEqual(); }
} testDescription_Point3DTestSuite_test_Point3D_NotEqual;

static class TestDescription_Point3DTestSuite_test_Point3D_IndexOperator : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_IndexOperator() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 122, "test_Point3D_IndexOperator" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_IndexOperator(); }
} testDescription_Point3DTestSuite_test_Point3D_IndexOperator;

static class TestDescription_Point3DTestSuite_test_Point3D_IntegerMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_IntegerMult() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 128, "test_Point3D_IntegerMult" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_IntegerMult(); }
} testDescription_Point3DTestSuite_test_Point3D_IntegerMult;

static class TestDescription_Point3DTestSuite_test_Point3D_IntegerDiv : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_IntegerDiv() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 135, "test_Point3D_IntegerDiv" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_IntegerDiv(); }
} testDescription_Point3DTestSuite_test_Point3D_IntegerDiv;

static class TestDescription_Point3DTestSuite_test_Point3D_OutputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_OutputStream() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 142, "test_Point3D_OutputStream" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_OutputStream(); }
} testDescription_Point3DTestSuite_test_Point3D_OutputStream;

static class TestDescription_Point3DTestSuite_test_Point3D_InputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_InputStream() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 151, "test_Point3D_InputStream" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_InputStream(); }
} testDescription_Point3DTestSuite_test_Point3D_InputStream;

static class TestDescription_Point3DTestSuite_test_Point3D_Conversion : public CxxTest::RealTestDescription {
public:
 TestDescription_Point3DTestSuite_test_Point3D_Conversion() : CxxTest::RealTestDescription( Tests_Point3DTestSuite, suiteDescription_Point3DTestSuite, 160, "test_Point3D_Conversion" ) {}
 void runTest() { suite_Point3DTestSuite.test_Point3D_Conversion(); }
} testDescription_Point3DTestSuite_test_Point3D_Conversion;

