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

static class TestDescription_Vector2DTestSuite_testVectorDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorDefaultConstr() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 12, "testVectorDefaultConstr" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorDefaultConstr(); }
} testDescription_Vector2DTestSuite_testVectorDefaultConstr;

static class TestDescription_Vector2DTestSuite_testPointConstrWithCoords : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testPointConstrWithCoords() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 18, "testPointConstrWithCoords" ) {}
 void runTest() { suite_Vector2DTestSuite.testPointConstrWithCoords(); }
} testDescription_Vector2DTestSuite_testPointConstrWithCoords;

static class TestDescription_Vector2DTestSuite_testPointConstrWithPoint : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testPointConstrWithPoint() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 24, "testPointConstrWithPoint" ) {}
 void runTest() { suite_Vector2DTestSuite.testPointConstrWithPoint(); }
} testDescription_Vector2DTestSuite_testPointConstrWithPoint;

static class TestDescription_Vector2DTestSuite_testVectorAdd : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorAdd() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 31, "testVectorAdd" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorAdd(); }
} testDescription_Vector2DTestSuite_testVectorAdd;

static class TestDescription_Vector2DTestSuite_testVectorAddAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorAddAssign() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 39, "testVectorAddAssign" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorAddAssign(); }
} testDescription_Vector2DTestSuite_testVectorAddAssign;

static class TestDescription_Vector2DTestSuite_testVectorSub : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorSub() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 48, "testVectorSub" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorSub(); }
} testDescription_Vector2DTestSuite_testVectorSub;

static class TestDescription_Vector2DTestSuite_testVectorSubAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorSubAssign() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 56, "testVectorSubAssign" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorSubAssign(); }
} testDescription_Vector2DTestSuite_testVectorSubAssign;

static class TestDescription_Vector2DTestSuite_testVectorScalarMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorScalarMult() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 65, "testVectorScalarMult" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorScalarMult(); }
} testDescription_Vector2DTestSuite_testVectorScalarMult;

static class TestDescription_Vector2DTestSuite_testVectorScalarPreMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorScalarPreMult() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 72, "testVectorScalarPreMult" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorScalarPreMult(); }
} testDescription_Vector2DTestSuite_testVectorScalarPreMult;

static class TestDescription_Vector2DTestSuite_testVectorScalarMultAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorScalarMultAssign() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 79, "testVectorScalarMultAssign" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorScalarMultAssign(); }
} testDescription_Vector2DTestSuite_testVectorScalarMultAssign;

static class TestDescription_Vector2DTestSuite_testVectorDiv : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorDiv() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 87, "testVectorDiv" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorDiv(); }
} testDescription_Vector2DTestSuite_testVectorDiv;

static class TestDescription_Vector2DTestSuite_testVectorDivAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorDivAssign() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 94, "testVectorDivAssign" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorDivAssign(); }
} testDescription_Vector2DTestSuite_testVectorDivAssign;

static class TestDescription_Vector2DTestSuite_testVectorDivByZero : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorDivByZero() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 105, "testVectorDivByZero" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorDivByZero(); }
} testDescription_Vector2DTestSuite_testVectorDivByZero;

static class TestDescription_Vector2DTestSuite_testVectorEqual : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorEqual() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 113, "testVectorEqual" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorEqual(); }
} testDescription_Vector2DTestSuite_testVectorEqual;

static class TestDescription_Vector2DTestSuite_testVectorDotMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorDotMult() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 121, "testVectorDotMult" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorDotMult(); }
} testDescription_Vector2DTestSuite_testVectorDotMult;

static class TestDescription_Vector2DTestSuite_testVectorCrossMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorCrossMult() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 129, "testVectorCrossMult" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorCrossMult(); }
} testDescription_Vector2DTestSuite_testVectorCrossMult;

static class TestDescription_Vector2DTestSuite_testVectorLength : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorLength() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 137, "testVectorLength" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorLength(); }
} testDescription_Vector2DTestSuite_testVectorLength;

static class TestDescription_Vector2DTestSuite_testVectorLengthSqr : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorLengthSqr() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 144, "testVectorLengthSqr" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorLengthSqr(); }
} testDescription_Vector2DTestSuite_testVectorLengthSqr;

static class TestDescription_Vector2DTestSuite_testVectorNormalize : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorNormalize() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 151, "testVectorNormalize" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorNormalize(); }
} testDescription_Vector2DTestSuite_testVectorNormalize;

static class TestDescription_Vector2DTestSuite_testVectorNormalizeZeroVector : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorNormalizeZeroVector() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 160, "testVectorNormalizeZeroVector" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorNormalizeZeroVector(); }
} testDescription_Vector2DTestSuite_testVectorNormalizeZeroVector;

static class TestDescription_Vector2DTestSuite_testVectorNormalized : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorNormalized() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 169, "testVectorNormalized" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorNormalized(); }
} testDescription_Vector2DTestSuite_testVectorNormalized;

static class TestDescription_Vector2DTestSuite_testVectorIndexOperator : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorIndexOperator() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 178, "testVectorIndexOperator" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorIndexOperator(); }
} testDescription_Vector2DTestSuite_testVectorIndexOperator;

static class TestDescription_Vector2DTestSuite_testVectorIntegerScalarMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorIntegerScalarMult() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 184, "testVectorIntegerScalarMult" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorIntegerScalarMult(); }
} testDescription_Vector2DTestSuite_testVectorIntegerScalarMult;

static class TestDescription_Vector2DTestSuite_testVectorIntegerDiv : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorIntegerDiv() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 191, "testVectorIntegerDiv" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorIntegerDiv(); }
} testDescription_Vector2DTestSuite_testVectorIntegerDiv;

static class TestDescription_Vector2DTestSuite_testVectorVectorAdd : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorVectorAdd() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 198, "testVectorVectorAdd" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorVectorAdd(); }
} testDescription_Vector2DTestSuite_testVectorVectorAdd;

static class TestDescription_Vector2DTestSuite_testVectorVectorSub : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorVectorSub() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 206, "testVectorVectorSub" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorVectorSub(); }
} testDescription_Vector2DTestSuite_testVectorVectorSub;

static class TestDescription_Vector2DTestSuite_testVectorOutputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorOutputStream() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 214, "testVectorOutputStream" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorOutputStream(); }
} testDescription_Vector2DTestSuite_testVectorOutputStream;

static class TestDescription_Vector2DTestSuite_testVectorInputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorInputStream() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 223, "testVectorInputStream" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorInputStream(); }
} testDescription_Vector2DTestSuite_testVectorInputStream;

static class TestDescription_Vector2DTestSuite_testVectorConversion : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector2DTestSuite_testVectorConversion() : CxxTest::RealTestDescription( Tests_Vector2DTestSuite, suiteDescription_Vector2DTestSuite, 232, "testVectorConversion" ) {}
 void runTest() { suite_Vector2DTestSuite.testVectorConversion(); }
} testDescription_Vector2DTestSuite_testVectorConversion;

