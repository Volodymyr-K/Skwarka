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

bool Vector2DTestSuite_init = false;
#include "./Vector2D.test.h"

static Vector2DTestSuite suite_Vector2DTestSuite;

static CxxTest::List Tests_Vector2DTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Vector2DTestSuite( "./Vector2D.test.h", 9, "Vector2DTestSuite", suite_Vector2DTestSuite, Tests_Vector2DTestSuite );

static class TestDescription_Vector2DTestSuite_test_Vector2D_DefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_DefaultConstr() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 12, "test_Vector2D_DefaultConstr" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_DefaultConstr(); }
} testDescription_Vector2DTestSuite_test_Vector2D_DefaultConstr;

static class TestDescription_Vector2DTestSuite_test_Vector2D_ConstrWithCoords : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_ConstrWithCoords() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 18, "test_Vector2D_ConstrWithCoords" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_ConstrWithCoords(); }
} testDescription_Vector2DTestSuite_test_Vector2D_ConstrWithCoords;

static class TestDescription_Vector2DTestSuite_test_Vector2D_ConstrWithPoint : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_ConstrWithPoint() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 24, "test_Vector2D_ConstrWithPoint" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_ConstrWithPoint(); }
} testDescription_Vector2DTestSuite_test_Vector2D_ConstrWithPoint;

static class TestDescription_Vector2DTestSuite_test_Vector2D_Add : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_Add() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 31, "test_Vector2D_Add" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_Add(); }
} testDescription_Vector2DTestSuite_test_Vector2D_Add;

static class TestDescription_Vector2DTestSuite_test_Vector2D_AddAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_AddAssign() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 39, "test_Vector2D_AddAssign" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_AddAssign(); }
} testDescription_Vector2DTestSuite_test_Vector2D_AddAssign;

static class TestDescription_Vector2DTestSuite_test_Vector2D_Sub : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_Sub() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 48, "test_Vector2D_Sub" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_Sub(); }
} testDescription_Vector2DTestSuite_test_Vector2D_Sub;

static class TestDescription_Vector2DTestSuite_test_Vector2D_SubAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_SubAssign() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 56, "test_Vector2D_SubAssign" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_SubAssign(); }
} testDescription_Vector2DTestSuite_test_Vector2D_SubAssign;

static class TestDescription_Vector2DTestSuite_test_Vector2D_ScalarMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_ScalarMult() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 65, "test_Vector2D_ScalarMult" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_ScalarMult(); }
} testDescription_Vector2DTestSuite_test_Vector2D_ScalarMult;

static class TestDescription_Vector2DTestSuite_test_Vector2D_ScalarPreMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_ScalarPreMult() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 72, "test_Vector2D_ScalarPreMult" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_ScalarPreMult(); }
} testDescription_Vector2DTestSuite_test_Vector2D_ScalarPreMult;

static class TestDescription_Vector2DTestSuite_test_Vector2D_ScalarMultAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_ScalarMultAssign() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 79, "test_Vector2D_ScalarMultAssign" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_ScalarMultAssign(); }
} testDescription_Vector2DTestSuite_test_Vector2D_ScalarMultAssign;

static class TestDescription_Vector2DTestSuite_test_Vector2D_Div : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_Div() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 87, "test_Vector2D_Div" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_Div(); }
} testDescription_Vector2DTestSuite_test_Vector2D_Div;

static class TestDescription_Vector2DTestSuite_test_Vector2D_DivAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_DivAssign() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 94, "test_Vector2D_DivAssign" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_DivAssign(); }
} testDescription_Vector2DTestSuite_test_Vector2D_DivAssign;

static class TestDescription_Vector2DTestSuite_test_Vector2D_DivByZero : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_DivByZero() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 105, "test_Vector2D_DivByZero" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_DivByZero(); }
} testDescription_Vector2DTestSuite_test_Vector2D_DivByZero;

static class TestDescription_Vector2DTestSuite_test_Vector2D_Equal : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_Equal() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 113, "test_Vector2D_Equal" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_Equal(); }
} testDescription_Vector2DTestSuite_test_Vector2D_Equal;

static class TestDescription_Vector2DTestSuite_test_Vector2D_DotMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_DotMult() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 121, "test_Vector2D_DotMult" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_DotMult(); }
} testDescription_Vector2DTestSuite_test_Vector2D_DotMult;

static class TestDescription_Vector2DTestSuite_test_Vector2D_CrossMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_CrossMult() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 129, "test_Vector2D_CrossMult" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_CrossMult(); }
} testDescription_Vector2DTestSuite_test_Vector2D_CrossMult;

static class TestDescription_Vector2DTestSuite_test_Vector2D_Length : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_Length() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 137, "test_Vector2D_Length" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_Length(); }
} testDescription_Vector2DTestSuite_test_Vector2D_Length;

static class TestDescription_Vector2DTestSuite_test_Vector2D_LengthSqr : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_LengthSqr() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 144, "test_Vector2D_LengthSqr" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_LengthSqr(); }
} testDescription_Vector2DTestSuite_test_Vector2D_LengthSqr;

static class TestDescription_Vector2DTestSuite_test_Vector2D_Normalize : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_Normalize() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 151, "test_Vector2D_Normalize" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_Normalize(); }
} testDescription_Vector2DTestSuite_test_Vector2D_Normalize;

static class TestDescription_Vector2DTestSuite_test_Vector2D_NormalizeZeroVector : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_NormalizeZeroVector() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 160, "test_Vector2D_NormalizeZeroVector" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_NormalizeZeroVector(); }
} testDescription_Vector2DTestSuite_test_Vector2D_NormalizeZeroVector;

static class TestDescription_Vector2DTestSuite_test_Vector2D_Normalized : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_Normalized() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 169, "test_Vector2D_Normalized" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_Normalized(); }
} testDescription_Vector2DTestSuite_test_Vector2D_Normalized;

static class TestDescription_Vector2DTestSuite_test_Vector2D_IndexOperator : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_IndexOperator() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 178, "test_Vector2D_IndexOperator" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_IndexOperator(); }
} testDescription_Vector2DTestSuite_test_Vector2D_IndexOperator;

static class TestDescription_Vector2DTestSuite_test_Vector2D_IntegerScalarMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_IntegerScalarMult() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 184, "test_Vector2D_IntegerScalarMult" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_IntegerScalarMult(); }
} testDescription_Vector2DTestSuite_test_Vector2D_IntegerScalarMult;

static class TestDescription_Vector2DTestSuite_test_Vector2D_IntegerDiv : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_IntegerDiv() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 191, "test_Vector2D_IntegerDiv" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_IntegerDiv(); }
} testDescription_Vector2DTestSuite_test_Vector2D_IntegerDiv;

static class TestDescription_Vector2DTestSuite_test_Vector2D_VectorAdd : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_VectorAdd() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 198, "test_Vector2D_VectorAdd" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_VectorAdd(); }
} testDescription_Vector2DTestSuite_test_Vector2D_VectorAdd;

static class TestDescription_Vector2DTestSuite_test_Vector2D_VectorSub : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_VectorSub() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 206, "test_Vector2D_VectorSub" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_VectorSub(); }
} testDescription_Vector2DTestSuite_test_Vector2D_VectorSub;

static class TestDescription_Vector2DTestSuite_test_Vector2D_OutputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_OutputStream() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 214, "test_Vector2D_OutputStream" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_OutputStream(); }
} testDescription_Vector2DTestSuite_test_Vector2D_OutputStream;

static class TestDescription_Vector2DTestSuite_test_Vector2D_InputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_InputStream() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 223, "test_Vector2D_InputStream" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_InputStream(); }
} testDescription_Vector2DTestSuite_test_Vector2D_InputStream;

static class TestDescription_Vector2DTestSuite_test_Vector2D_Conversion : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_test_Vector2D_Conversion() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 232, "test_Vector2D_Conversion" ) {}
 void runTest() { suite_Vector2DTestSuite.test_Vector2D_Conversion(); }
} testDescription_Vector2DTestSuite_test_Vector2D_Conversion;

