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

bool Vector3DTestSuite_init = false;
#include "./Vector3D.test.h"

static Vector3DTestSuite suite_Vector3DTestSuite;

static CxxTest::List Tests_Vector3DTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Vector3DTestSuite( "./Vector3D.test.h", 9, "Vector3DTestSuite", suite_Vector3DTestSuite, Tests_Vector3DTestSuite );

static class TestDescription_Vector3DTestSuite_testVectorDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorDefaultConstr() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 12, "testVectorDefaultConstr" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorDefaultConstr(); }
} testDescription_Vector3DTestSuite_testVectorDefaultConstr;

static class TestDescription_Vector3DTestSuite_testVectorConstrWithCoords : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorConstrWithCoords() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 18, "testVectorConstrWithCoords" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorConstrWithCoords(); }
} testDescription_Vector3DTestSuite_testVectorConstrWithCoords;

static class TestDescription_Vector3DTestSuite_testVectorConstrWithPoint : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorConstrWithPoint() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 24, "testVectorConstrWithPoint" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorConstrWithPoint(); }
} testDescription_Vector3DTestSuite_testVectorConstrWithPoint;

static class TestDescription_Vector3DTestSuite_testVectorAdd : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorAdd() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 31, "testVectorAdd" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorAdd(); }
} testDescription_Vector3DTestSuite_testVectorAdd;

static class TestDescription_Vector3DTestSuite_testVectorAddAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorAddAssign() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 39, "testVectorAddAssign" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorAddAssign(); }
} testDescription_Vector3DTestSuite_testVectorAddAssign;

static class TestDescription_Vector3DTestSuite_testVectorSub : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorSub() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 48, "testVectorSub" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorSub(); }
} testDescription_Vector3DTestSuite_testVectorSub;

static class TestDescription_Vector3DTestSuite_testVectorSubAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorSubAssign() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 56, "testVectorSubAssign" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorSubAssign(); }
} testDescription_Vector3DTestSuite_testVectorSubAssign;

static class TestDescription_Vector3DTestSuite_testVectorScalarMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorScalarMult() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 65, "testVectorScalarMult" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorScalarMult(); }
} testDescription_Vector3DTestSuite_testVectorScalarMult;

static class TestDescription_Vector3DTestSuite_testVectorScalarPreMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorScalarPreMult() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 72, "testVectorScalarPreMult" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorScalarPreMult(); }
} testDescription_Vector3DTestSuite_testVectorScalarPreMult;

static class TestDescription_Vector3DTestSuite_testVectorScalarMultAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorScalarMultAssign() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 79, "testVectorScalarMultAssign" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorScalarMultAssign(); }
} testDescription_Vector3DTestSuite_testVectorScalarMultAssign;

static class TestDescription_Vector3DTestSuite_testVectorDiv : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorDiv() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 87, "testVectorDiv" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorDiv(); }
} testDescription_Vector3DTestSuite_testVectorDiv;

static class TestDescription_Vector3DTestSuite_testVectorDivAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorDivAssign() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 94, "testVectorDivAssign" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorDivAssign(); }
} testDescription_Vector3DTestSuite_testVectorDivAssign;

static class TestDescription_Vector3DTestSuite_testVectorDivByZero : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorDivByZero() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 105, "testVectorDivByZero" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorDivByZero(); }
} testDescription_Vector3DTestSuite_testVectorDivByZero;

static class TestDescription_Vector3DTestSuite_testVectorEqual : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorEqual() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 113, "testVectorEqual" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorEqual(); }
} testDescription_Vector3DTestSuite_testVectorEqual;

static class TestDescription_Vector3DTestSuite_testVectorDotMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorDotMult() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 121, "testVectorDotMult" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorDotMult(); }
} testDescription_Vector3DTestSuite_testVectorDotMult;

static class TestDescription_Vector3DTestSuite_testVectorCrossMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorCrossMult() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 129, "testVectorCrossMult" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorCrossMult(); }
} testDescription_Vector3DTestSuite_testVectorCrossMult;

static class TestDescription_Vector3DTestSuite_testVectorLength : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorLength() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 137, "testVectorLength" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorLength(); }
} testDescription_Vector3DTestSuite_testVectorLength;

static class TestDescription_Vector3DTestSuite_testVectorLengthSqr : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorLengthSqr() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 144, "testVectorLengthSqr" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorLengthSqr(); }
} testDescription_Vector3DTestSuite_testVectorLengthSqr;

static class TestDescription_Vector3DTestSuite_testVectorNormalize : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorNormalize() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 151, "testVectorNormalize" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorNormalize(); }
} testDescription_Vector3DTestSuite_testVectorNormalize;

static class TestDescription_Vector3DTestSuite_testVectorNormalizeZeroVector : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorNormalizeZeroVector() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 160, "testVectorNormalizeZeroVector" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorNormalizeZeroVector(); }
} testDescription_Vector3DTestSuite_testVectorNormalizeZeroVector;

static class TestDescription_Vector3DTestSuite_testVectorNormalized : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorNormalized() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 169, "testVectorNormalized" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorNormalized(); }
} testDescription_Vector3DTestSuite_testVectorNormalized;

static class TestDescription_Vector3DTestSuite_testVectorIndexOperator : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorIndexOperator() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 178, "testVectorIndexOperator" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorIndexOperator(); }
} testDescription_Vector3DTestSuite_testVectorIndexOperator;

static class TestDescription_Vector3DTestSuite_testVectorIntegerScalarMult : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorIntegerScalarMult() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 184, "testVectorIntegerScalarMult" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorIntegerScalarMult(); }
} testDescription_Vector3DTestSuite_testVectorIntegerScalarMult;

static class TestDescription_Vector3DTestSuite_testVectorIntegerDiv : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorIntegerDiv() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 191, "testVectorIntegerDiv" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorIntegerDiv(); }
} testDescription_Vector3DTestSuite_testVectorIntegerDiv;

static class TestDescription_Vector3DTestSuite_testVectorVectorAdd : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorVectorAdd() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 198, "testVectorVectorAdd" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorVectorAdd(); }
} testDescription_Vector3DTestSuite_testVectorVectorAdd;

static class TestDescription_Vector3DTestSuite_testVectorVectorSub : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorVectorSub() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 206, "testVectorVectorSub" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorVectorSub(); }
} testDescription_Vector3DTestSuite_testVectorVectorSub;

static class TestDescription_Vector3DTestSuite_testVectorOutputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorOutputStream() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 214, "testVectorOutputStream" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorOutputStream(); }
} testDescription_Vector3DTestSuite_testVectorOutputStream;

static class TestDescription_Vector3DTestSuite_testVectorInputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorInputStream() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 223, "testVectorInputStream" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorInputStream(); }
} testDescription_Vector3DTestSuite_testVectorInputStream;

static class TestDescription_Vector3DTestSuite_testVectorConversion : public CxxTest::RealTestDescription {
public:
 TestDescription_Vector3DTestSuite_testVectorConversion() : CxxTest::RealTestDescription( Tests_Vector3DTestSuite, suiteDescription_Vector3DTestSuite, 232, "testVectorConversion" ) {}
 void runTest() { suite_Vector3DTestSuite.testVectorConversion(); }
} testDescription_Vector3DTestSuite_testVectorConversion;

