/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Matrix.test.h"

static MatrixTestSuite suite_MatrixTestSuite;

static CxxTest::List Tests_MatrixTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_MatrixTestSuite( "./Matrix.test.h", 9, "MatrixTestSuite", suite_MatrixTestSuite, Tests_MatrixTestSuite );

static class TestDescription_MatrixTestSuite_test_Matrix_DefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_DefaultConstr() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 41, "test_Matrix_DefaultConstr" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_DefaultConstr(); }
} testDescription_MatrixTestSuite_test_Matrix_DefaultConstr;

static class TestDescription_MatrixTestSuite_test_Matrix_IdentityConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_IdentityConstr() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 49, "test_Matrix_IdentityConstr" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_IdentityConstr(); }
} testDescription_MatrixTestSuite_test_Matrix_IdentityConstr;

static class TestDescription_MatrixTestSuite_test_Matrix_Add : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_Add() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 57, "test_Matrix_Add" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_Add(); }
} testDescription_MatrixTestSuite_test_Matrix_Add;

static class TestDescription_MatrixTestSuite_test_Matrix_AddAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_AddAssign() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 65, "test_Matrix_AddAssign" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_AddAssign(); }
} testDescription_MatrixTestSuite_test_Matrix_AddAssign;

static class TestDescription_MatrixTestSuite_test_Matrix_Sub : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_Sub() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 74, "test_Matrix_Sub" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_Sub(); }
} testDescription_MatrixTestSuite_test_Matrix_Sub;

static class TestDescription_MatrixTestSuite_test_Matrix_SubAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_SubAssign() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 82, "test_Matrix_SubAssign" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_SubAssign(); }
} testDescription_MatrixTestSuite_test_Matrix_SubAssign;

static class TestDescription_MatrixTestSuite_test_Matrix_ScalarMult : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_ScalarMult() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 91, "test_Matrix_ScalarMult" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_ScalarMult(); }
} testDescription_MatrixTestSuite_test_Matrix_ScalarMult;

static class TestDescription_MatrixTestSuite_test_Matrix_ScalarMultAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_ScalarMultAssign() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 99, "test_Matrix_ScalarMultAssign" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_ScalarMultAssign(); }
} testDescription_MatrixTestSuite_test_Matrix_ScalarMultAssign;

static class TestDescription_MatrixTestSuite_test_Matrix_ScalarPreMult : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_ScalarPreMult() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 108, "test_Matrix_ScalarPreMult" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_ScalarPreMult(); }
} testDescription_MatrixTestSuite_test_Matrix_ScalarPreMult;

static class TestDescription_MatrixTestSuite_test_Matrix_PreMult : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_PreMult() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 116, "test_Matrix_PreMult" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_PreMult(); }
} testDescription_MatrixTestSuite_test_Matrix_PreMult;

static class TestDescription_MatrixTestSuite_test_Matrix_PostMult : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_PostMult() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 129, "test_Matrix_PostMult" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_PostMult(); }
} testDescription_MatrixTestSuite_test_Matrix_PostMult;

static class TestDescription_MatrixTestSuite_test_Matrix_OperatorMult : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_OperatorMult() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 142, "test_Matrix_OperatorMult" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_OperatorMult(); }
} testDescription_MatrixTestSuite_test_Matrix_OperatorMult;

static class TestDescription_MatrixTestSuite_test_Matrix_Transposed : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_Transposed() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 154, "test_Matrix_Transposed" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_Transposed(); }
} testDescription_MatrixTestSuite_test_Matrix_Transposed;

static class TestDescription_MatrixTestSuite_test_Matrix_Inverted : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_Inverted() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 162, "test_Matrix_Inverted" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_Inverted(); }
} testDescription_MatrixTestSuite_test_Matrix_Inverted;

static class TestDescription_MatrixTestSuite_test_Matrix_InvertedFail : public CxxTest::RealTestDescription {
public:
 TestDescription_MatrixTestSuite_test_Matrix_InvertedFail() : CxxTest::RealTestDescription( Tests_MatrixTestSuite, suiteDescription_MatrixTestSuite, 178, "test_Matrix_InvertedFail" ) {}
 void runTest() { suite_MatrixTestSuite.test_Matrix_InvertedFail(); }
} testDescription_MatrixTestSuite_test_Matrix_InvertedFail;

