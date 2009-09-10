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

bool StratifiedSamplerTestSuite_init = false;
#include "./StratifiedSampler.test.h"

static StratifiedSamplerTestSuite suite_StratifiedSamplerTestSuite;

static CxxTest::List Tests_StratifiedSamplerTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_StratifiedSamplerTestSuite( "./StratifiedSampler.test.h", 11, "StratifiedSamplerTestSuite", suite_StratifiedSamplerTestSuite, Tests_StratifiedSamplerTestSuite );

static class TestDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_Constr : public CxxTest::RealTestDescription {
public:
 TestDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_Constr() : CxxTest::RealTestDescription( Tests_StratifiedSamplerTestSuite, suiteDescription_StratifiedSamplerTestSuite, 14, "test_StratifiedSampler_Constr" ) {}
 void runTest() { suite_StratifiedSamplerTestSuite.test_StratifiedSampler_Constr(); }
} testDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_Constr;

static class TestDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_ImagePoints : public CxxTest::RealTestDescription {
public:
 TestDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_ImagePoints() : CxxTest::RealTestDescription( Tests_StratifiedSamplerTestSuite, suiteDescription_StratifiedSamplerTestSuite, 20, "test_StratifiedSampler_ImagePoints" ) {}
 void runTest() { suite_StratifiedSamplerTestSuite.test_StratifiedSampler_ImagePoints(); }
} testDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_ImagePoints;

static class TestDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_LensUVs : public CxxTest::RealTestDescription {
public:
 TestDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_LensUVs() : CxxTest::RealTestDescription( Tests_StratifiedSamplerTestSuite, suiteDescription_StratifiedSamplerTestSuite, 44, "test_StratifiedSampler_LensUVs" ) {}
 void runTest() { suite_StratifiedSamplerTestSuite.test_StratifiedSampler_LensUVs(); }
} testDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_LensUVs;

static class TestDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_1DSequences : public CxxTest::RealTestDescription {
public:
 TestDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_1DSequences() : CxxTest::RealTestDescription( Tests_StratifiedSamplerTestSuite, suiteDescription_StratifiedSamplerTestSuite, 77, "test_StratifiedSampler_1DSequences" ) {}
 void runTest() { suite_StratifiedSamplerTestSuite.test_StratifiedSampler_1DSequences(); }
} testDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_1DSequences;

static class TestDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_2DSequences : public CxxTest::RealTestDescription {
public:
 TestDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_2DSequences() : CxxTest::RealTestDescription( Tests_StratifiedSamplerTestSuite, suiteDescription_StratifiedSamplerTestSuite, 110, "test_StratifiedSampler_2DSequences" ) {}
 void runTest() { suite_StratifiedSamplerTestSuite.test_StratifiedSampler_2DSequences(); }
} testDescription_StratifiedSamplerTestSuite_test_StratifiedSampler_2DSequences;

