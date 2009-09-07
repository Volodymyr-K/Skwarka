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

static class TestDescription_UtilTestSuite_testSolveLinearSystem2x2 : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilTestSuite_testSolveLinearSystem2x2() : CxxTest::RealTestDescription( Tests_UtilTestSuite, suiteDescription_UtilTestSuite, 11, "testSolveLinearSystem2x2" ) {}
 void runTest() { suite_UtilTestSuite.testSolveLinearSystem2x2(); }
} testDescription_UtilTestSuite_testSolveLinearSystem2x2;

static class TestDescription_UtilTestSuite_testSolveLinearSystem2x2Degenerated : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilTestSuite_testSolveLinearSystem2x2Degenerated() : CxxTest::RealTestDescription( Tests_UtilTestSuite, suiteDescription_UtilTestSuite, 27, "testSolveLinearSystem2x2Degenerated" ) {}
 void runTest() { suite_UtilTestSuite.testSolveLinearSystem2x2Degenerated(); }
} testDescription_UtilTestSuite_testSolveLinearSystem2x2Degenerated;

static class TestDescription_UtilTestSuite_testClamp1 : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilTestSuite_testClamp1() : CxxTest::RealTestDescription( Tests_UtilTestSuite, suiteDescription_UtilTestSuite, 38, "testClamp1" ) {}
 void runTest() { suite_UtilTestSuite.testClamp1(); }
} testDescription_UtilTestSuite_testClamp1;

static class TestDescription_UtilTestSuite_testClamp2 : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilTestSuite_testClamp2() : CxxTest::RealTestDescription( Tests_UtilTestSuite, suiteDescription_UtilTestSuite, 45, "testClamp2" ) {}
 void runTest() { suite_UtilTestSuite.testClamp2(); }
} testDescription_UtilTestSuite_testClamp2;

static class TestDescription_UtilTestSuite_testClamp3 : public CxxTest::RealTestDescription {
public:
 TestDescription_UtilTestSuite_testClamp3() : CxxTest::RealTestDescription( Tests_UtilTestSuite, suiteDescription_UtilTestSuite, 52, "testClamp3" ) {}
 void runTest() { suite_UtilTestSuite.testClamp3(); }
} testDescription_UtilTestSuite_testClamp3;

