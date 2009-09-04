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

bool MatrixTestSuite_init = false;
#include "./Matrix.test.h"

static MatrixTestSuite suite_MatrixTestSuite;

static CxxTest::List Tests_MatrixTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_MatrixTestSuite( "./Matrix.test.h", 9, "MatrixTestSuite", suite_MatrixTestSuite, Tests_MatrixTestSuite );

static class TestDescription_MatrixTestSuite_testMatrixDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixDefaultConstr() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 50, "testMatrixDefaultConstr" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixDefaultConstr(); }
} testDescription_MatrixTestSuite_testMatrixDefaultConstr;

static class TestDescription_MatrixTestSuite_testMatrixIdentityConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixIdentityConstr() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 58, "testMatrixIdentityConstr" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixIdentityConstr(); }
} testDescription_MatrixTestSuite_testMatrixIdentityConstr;

static class TestDescription_MatrixTestSuite_testMatrixAdd : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixAdd() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 66, "testMatrixAdd" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixAdd(); }
} testDescription_MatrixTestSuite_testMatrixAdd;

static class TestDescription_MatrixTestSuite_testMatrixAddAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixAddAssign() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 74, "testMatrixAddAssign" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixAddAssign(); }
} testDescription_MatrixTestSuite_testMatrixAddAssign;

static class TestDescription_MatrixTestSuite_testMatrixSub : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixSub() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 83, "testMatrixSub" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixSub(); }
} testDescription_MatrixTestSuite_testMatrixSub;

static class TestDescription_MatrixTestSuite_testMatrixSubAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixSubAssign() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 91, "testMatrixSubAssign" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixSubAssign(); }
} testDescription_MatrixTestSuite_testMatrixSubAssign;

static class TestDescription_MatrixTestSuite_testMatrixScalarMult : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixScalarMult() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 100, "testMatrixScalarMult" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixScalarMult(); }
} testDescription_MatrixTestSuite_testMatrixScalarMult;

static class TestDescription_MatrixTestSuite_testMatrixScalarMultAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixScalarMultAssign() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 108, "testMatrixScalarMultAssign" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixScalarMultAssign(); }
} testDescription_MatrixTestSuite_testMatrixScalarMultAssign;

static class TestDescription_MatrixTestSuite_testMatrixScalarPreMult : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixScalarPreMult() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 117, "testMatrixScalarPreMult" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixScalarPreMult(); }
} testDescription_MatrixTestSuite_testMatrixScalarPreMult;

static class TestDescription_MatrixTestSuite_testMatrixPreMult : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixPreMult() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 125, "testMatrixPreMult" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixPreMult(); }
} testDescription_MatrixTestSuite_testMatrixPreMult;

static class TestDescription_MatrixTestSuite_testMatrixPostMult : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixPostMult() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 138, "testMatrixPostMult" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixPostMult(); }
} testDescription_MatrixTestSuite_testMatrixPostMult;

static class TestDescription_MatrixTestSuite_testMatrixOperatorMult : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixOperatorMult() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 151, "testMatrixOperatorMult" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixOperatorMult(); }
} testDescription_MatrixTestSuite_testMatrixOperatorMult;

static class TestDescription_MatrixTestSuite_testMatrixTransposed : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixTransposed() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 163, "testMatrixTransposed" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixTransposed(); }
} testDescription_MatrixTestSuite_testMatrixTransposed;

static class TestDescription_MatrixTestSuite_testMatrixInverted : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixInverted() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 171, "testMatrixInverted" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixInverted(); }
} testDescription_MatrixTestSuite_testMatrixInverted;

static class TestDescription_MatrixTestSuite_testMatrixInvertedFail : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_testMatrixInvertedFail() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 187, "testMatrixInvertedFail" ) {}
 void runTest() { suite_MatrixTestSuite.testMatrixInvertedFail(); }
} testDescription_MatrixTestSuite_testMatrixInvertedFail;

