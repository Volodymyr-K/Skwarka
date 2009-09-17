/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./BoxFilter.test.h"

static BoxFilterTestSuite suite_BoxFilterTestSuite;

static CxxTest::List Tests_BoxFilterTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_BoxFilterTestSuite( "./BoxFilter.test.h", 9, "BoxFilterTestSuite", suite_BoxFilterTestSuite, Tests_BoxFilterTestSuite );

static class TestDescription_BoxFilterTestSuite_test_BoxFilter_EvaluateInside1 : public CxxTest::RealTestDescription {
public:
 TestDescription_BoxFilterTestSuite_test_BoxFilter_EvaluateInside1() : CxxTest::RealTestDescription( Tests_BoxFilterTestSuite, suiteDescription_BoxFilterTestSuite, 12, "test_BoxFilter_EvaluateInside1" ) {}
 void runTest() { suite_BoxFilterTestSuite.test_BoxFilter_EvaluateInside1(); }
} testDescription_BoxFilterTestSuite_test_BoxFilter_EvaluateInside1;

static class TestDescription_BoxFilterTestSuite_test_BoxFilter_EvaluateInside2 : public CxxTest::RealTestDescription {
public:
 TestDescription_BoxFilterTestSuite_test_BoxFilter_EvaluateInside2() : CxxTest::RealTestDescription( Tests_BoxFilterTestSuite, suiteDescription_BoxFilterTestSuite, 19, "test_BoxFilter_EvaluateInside2" ) {}
 void runTest() { suite_BoxFilterTestSuite.test_BoxFilter_EvaluateInside2(); }
} testDescription_BoxFilterTestSuite_test_BoxFilter_EvaluateInside2;

static class TestDescription_BoxFilterTestSuite_test_BoxFilter_EvaluateOutside : public CxxTest::RealTestDescription {
public:
 TestDescription_BoxFilterTestSuite_test_BoxFilter_EvaluateOutside() : CxxTest::RealTestDescription( Tests_BoxFilterTestSuite, suiteDescription_BoxFilterTestSuite, 26, "test_BoxFilter_EvaluateOutside" ) {}
 void runTest() { suite_BoxFilterTestSuite.test_BoxFilter_EvaluateOutside(); }
} testDescription_BoxFilterTestSuite_test_BoxFilter_EvaluateOutside;

