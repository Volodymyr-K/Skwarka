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

bool UtilTestSuite_init = false;
#include "./Util.test.h"

static UtilTestSuite suite_UtilTestSuite;

static CxxTest::List Tests_UtilTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_UtilTestSuite( "./Util.test.h", 8, "UtilTestSuite", suite_UtilTestSuite, Tests_UtilTestSuite );

static class TestDescription_UtilTestSuite_test_SolveLinearSystem2x2 : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilTestSuite_test_SolveLinearSystem2x2() : CxxTest::RealTestDescription( Tests_UtilTestSuite, suiteDescription_UtilTestSuite, 11, "test_SolveLinearSystem2x2" ) {}
 void runTest() { suite_UtilTestSuite.test_SolveLinearSystem2x2(); }
} testDescription_UtilTestSuite_test_SolveLinearSystem2x2;

static class TestDescription_UtilTestSuite_test_SolveLinearSystem2x2Degenerated : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilTestSuite_test_SolveLinearSystem2x2Degenerated() : CxxTest::RealTestDescription( Tests_UtilTestSuite, suiteDescription_UtilTestSuite, 27, "test_SolveLinearSystem2x2Degenerated" ) {}
 void runTest() { suite_UtilTestSuite.test_SolveLinearSystem2x2Degenerated(); }
} testDescription_UtilTestSuite_test_SolveLinearSystem2x2Degenerated;

static class TestDescription_UtilTestSuite_test_Clamp1 : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilTestSuite_test_Clamp1() : CxxTest::RealTestDescription( Tests_UtilTestSuite, suiteDescription_UtilTestSuite, 38, "test_Clamp1" ) {}
 void runTest() { suite_UtilTestSuite.test_Clamp1(); }
} testDescription_UtilTestSuite_test_Clamp1;

static class TestDescription_UtilTestSuite_test_Clamp3 : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilTestSuite_test_Clamp3() : CxxTest::RealTestDescription( Tests_UtilTestSuite, suiteDescription_UtilTestSuite, 52, "test_Clamp3" ) {}
 void runTest() { suite_UtilTestSuite.test_Clamp3(); }
} testDescription_UtilTestSuite_test_Clamp3;

