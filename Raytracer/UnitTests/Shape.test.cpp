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

bool ShapeTestSuite_init = false;
#include "./Shape.test.h"

static ShapeTestSuite suite_ShapeTestSuite;

static CxxTest::List Tests_ShapeTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ShapeTestSuite( "./Shape.test.h", 12, "ShapeTestSuite", suite_ShapeTestSuite, Tests_ShapeTestSuite );

static class TestDescription_ShapeTestSuite_test_Shape_ReadCorrectParameter : public CxxTest::RealTestDescription {
public:
 TestDescription_ShapeTestSuite_test_Shape_ReadCorrectParameter() : CxxTest::RealTestDescription( Tests_ShapeTestSuite, suiteDescription_ShapeTestSuite, 16, "test_Shape_ReadCorrectParameter" ) {}
 void runTest() { suite_ShapeTestSuite.test_Shape_ReadCorrectParameter(); }
} testDescription_ShapeTestSuite_test_Shape_ReadCorrectParameter;

static class TestDescription_ShapeTestSuite_test_Shape_ReadOverwrittenParameter : public CxxTest::RealTestDescription {
public:
 TestDescription_ShapeTestSuite_test_Shape_ReadOverwrittenParameter() : CxxTest::RealTestDescription( Tests_ShapeTestSuite, suiteDescription_ShapeTestSuite, 29, "test_Shape_ReadOverwrittenParameter" ) {}
 void runTest() { suite_ShapeTestSuite.test_Shape_ReadOverwrittenParameter(); }
} testDescription_ShapeTestSuite_test_Shape_ReadOverwrittenParameter;

static class TestDescription_ShapeTestSuite_test_Shape_ReadIncorrectParameter : public CxxTest::RealTestDescription {
public:
 TestDescription_ShapeTestSuite_test_Shape_ReadIncorrectParameter() : CxxTest::RealTestDescription( Tests_ShapeTestSuite, suiteDescription_ShapeTestSuite, 42, "test_Shape_ReadIncorrectParameter" ) {}
 void runTest() { suite_ShapeTestSuite.test_Shape_ReadIncorrectParameter(); }
} testDescription_ShapeTestSuite_test_Shape_ReadIncorrectParameter;

static class TestDescription_ShapeTestSuite_test_Shape_ReadMissingParameter : public CxxTest::RealTestDescription {
public:
 TestDescription_ShapeTestSuite_test_Shape_ReadMissingParameter() : CxxTest::RealTestDescription( Tests_ShapeTestSuite, suiteDescription_ShapeTestSuite, 53, "test_Shape_ReadMissingParameter" ) {}
 void runTest() { suite_ShapeTestSuite.test_Shape_ReadMissingParameter(); }
} testDescription_ShapeTestSuite_test_Shape_ReadMissingParameter;

static class TestDescription_ShapeTestSuite_test_Shape_AddError : public CxxTest::RealTestDescription {
public:
 TestDescription_ShapeTestSuite_test_Shape_AddError() : CxxTest::RealTestDescription( Tests_ShapeTestSuite, suiteDescription_ShapeTestSuite, 64, "test_Shape_AddError" ) {}
 void runTest() { suite_ShapeTestSuite.test_Shape_AddError(); }
} testDescription_ShapeTestSuite_test_Shape_AddError;

static class TestDescription_ShapeTestSuite_test_Shape_ClearErrors : public CxxTest::RealTestDescription {
public:
 TestDescription_ShapeTestSuite_test_Shape_ClearErrors() : CxxTest::RealTestDescription( Tests_ShapeTestSuite, suiteDescription_ShapeTestSuite, 73, "test_Shape_ClearErrors" ) {}
 void runTest() { suite_ShapeTestSuite.test_Shape_ClearErrors(); }
} testDescription_ShapeTestSuite_test_Shape_ClearErrors;

static class TestDescription_ShapeTestSuite_test_Shape_MissingParameterError1 : public CxxTest::RealTestDescription {
public:
 TestDescription_ShapeTestSuite_test_Shape_MissingParameterError1() : CxxTest::RealTestDescription( Tests_ShapeTestSuite, suiteDescription_ShapeTestSuite, 83, "test_Shape_MissingParameterError1" ) {}
 void runTest() { suite_ShapeTestSuite.test_Shape_MissingParameterError1(); }
} testDescription_ShapeTestSuite_test_Shape_MissingParameterError1;

static class TestDescription_ShapeTestSuite_test_Shape_MissingParameterError2 : public CxxTest::RealTestDescription {
public:
 TestDescription_ShapeTestSuite_test_Shape_MissingParameterError2() : CxxTest::RealTestDescription( Tests_ShapeTestSuite, suiteDescription_ShapeTestSuite, 94, "test_Shape_MissingParameterError2" ) {}
 void runTest() { suite_ShapeTestSuite.test_Shape_MissingParameterError2(); }
} testDescription_ShapeTestSuite_test_Shape_MissingParameterError2;

