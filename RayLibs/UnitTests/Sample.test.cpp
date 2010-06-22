/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Sample.test.h"

static SampleTestSuite suite_SampleTestSuite;

static CxxTest::List Tests_SampleTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SampleTestSuite( "./Sample.test.h", 9, "SampleTestSuite", suite_SampleTestSuite, Tests_SampleTestSuite );

static class TestDescription_SampleTestSuite_test_Sample_Constr : public CxxTest::RealTestDescription {
public:
 TestDescription_SampleTestSuite_test_Sample_Constr() : CxxTest::RealTestDescription( Tests_SampleTestSuite, suiteDescription_SampleTestSuite, 12, "test_Sample_Constr" ) {}
 void runTest() { suite_SampleTestSuite.test_Sample_Constr(); }
} testDescription_SampleTestSuite_test_Sample_Constr;

static class TestDescription_SampleTestSuite_test_Sample_ImagePoint : public CxxTest::RealTestDescription {
public:
 TestDescription_SampleTestSuite_test_Sample_ImagePoint() : CxxTest::RealTestDescription( Tests_SampleTestSuite, suiteDescription_SampleTestSuite, 21, "test_Sample_ImagePoint" ) {}
 void runTest() { suite_SampleTestSuite.test_Sample_ImagePoint(); }
} testDescription_SampleTestSuite_test_Sample_ImagePoint;

static class TestDescription_SampleTestSuite_test_Sample_Sequences : public CxxTest::RealTestDescription {
public:
 TestDescription_SampleTestSuite_test_Sample_Sequences() : CxxTest::RealTestDescription( Tests_SampleTestSuite, suiteDescription_SampleTestSuite, 31, "test_Sample_Sequences" ) {}
 void runTest() { suite_SampleTestSuite.test_Sample_Sequences(); }
} testDescription_SampleTestSuite_test_Sample_Sequences;

