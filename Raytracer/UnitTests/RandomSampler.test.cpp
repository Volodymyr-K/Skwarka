/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./RandomSampler.test.h"

static RandomSamplerTestSuite suite_RandomSamplerTestSuite;

static CxxTest::List Tests_RandomSamplerTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_RandomSamplerTestSuite( "./RandomSampler.test.h", 11, "RandomSamplerTestSuite", suite_RandomSamplerTestSuite, Tests_RandomSamplerTestSuite );

static class TestDescription_RandomSamplerTestSuite_test_RandomSampler_Constr : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomSamplerTestSuite_test_RandomSampler_Constr() : CxxTest::RealTestDescription( Tests_RandomSamplerTestSuite, suiteDescription_RandomSamplerTestSuite, 14, "test_RandomSampler_Constr" ) {}
 void runTest() { suite_RandomSamplerTestSuite.test_RandomSampler_Constr(); }
} testDescription_RandomSamplerTestSuite_test_RandomSampler_Constr;

static class TestDescription_RandomSamplerTestSuite_test_RandomSampler_ImagePoints : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomSamplerTestSuite_test_RandomSampler_ImagePoints() : CxxTest::RealTestDescription( Tests_RandomSamplerTestSuite, suiteDescription_RandomSamplerTestSuite, 20, "test_RandomSampler_ImagePoints" ) {}
 void runTest() { suite_RandomSamplerTestSuite.test_RandomSampler_ImagePoints(); }
} testDescription_RandomSamplerTestSuite_test_RandomSampler_ImagePoints;

static class TestDescription_RandomSamplerTestSuite_test_RandomSampler_LensUVs : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomSamplerTestSuite_test_RandomSampler_LensUVs() : CxxTest::RealTestDescription( Tests_RandomSamplerTestSuite, suiteDescription_RandomSamplerTestSuite, 41, "test_RandomSampler_LensUVs" ) {}
 void runTest() { suite_RandomSamplerTestSuite.test_RandomSampler_LensUVs(); }
} testDescription_RandomSamplerTestSuite_test_RandomSampler_LensUVs;

static class TestDescription_RandomSamplerTestSuite_test_RandomSampler_1DSequences : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomSamplerTestSuite_test_RandomSampler_1DSequences() : CxxTest::RealTestDescription( Tests_RandomSamplerTestSuite, suiteDescription_RandomSamplerTestSuite, 71, "test_RandomSampler_1DSequences" ) {}
 void runTest() { suite_RandomSamplerTestSuite.test_RandomSampler_1DSequences(); }
} testDescription_RandomSamplerTestSuite_test_RandomSampler_1DSequences;

static class TestDescription_RandomSamplerTestSuite_test_RandomSampler_2DSequences : public CxxTest::RealTestDescription {
public:
 TestDescription_RandomSamplerTestSuite_test_RandomSampler_2DSequences() : CxxTest::RealTestDescription( Tests_RandomSamplerTestSuite, suiteDescription_RandomSamplerTestSuite, 102, "test_RandomSampler_2DSequences" ) {}
 void runTest() { suite_RandomSamplerTestSuite.test_RandomSampler_2DSequences(); }
} testDescription_RandomSamplerTestSuite_test_RandomSampler_2DSequences;

