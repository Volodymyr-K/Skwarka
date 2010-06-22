/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Matte.test.h"

static MatteTestSuite suite_MatteTestSuite;

static CxxTest::List Tests_MatteTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_MatteTestSuite( "./Matte.test.h", 14, "MatteTestSuite", suite_MatteTestSuite, Tests_MatteTestSuite );

static class TestDescription_MatteTestSuite_test_Matte_EvaluateZeroSigma : public CxxTest::RealTestDescription {
public:
 TestDescription_MatteTestSuite_test_Matte_EvaluateZeroSigma() : CxxTest::RealTestDescription( Tests_MatteTestSuite, suiteDescription_MatteTestSuite, 18, "test_Matte_EvaluateZeroSigma" ) {}
 void runTest() { suite_MatteTestSuite.test_Matte_EvaluateZeroSigma(); }
} testDescription_MatteTestSuite_test_Matte_EvaluateZeroSigma;

static class TestDescription_MatteTestSuite_test_Matte_EvaluateNotZeroSigma : public CxxTest::RealTestDescription {
public:
 TestDescription_MatteTestSuite_test_Matte_EvaluateNotZeroSigma() : CxxTest::RealTestDescription( Tests_MatteTestSuite, suiteDescription_MatteTestSuite, 38, "test_Matte_EvaluateNotZeroSigma" ) {}
 void runTest() { suite_MatteTestSuite.test_Matte_EvaluateNotZeroSigma(); }
} testDescription_MatteTestSuite_test_Matte_EvaluateNotZeroSigma;

