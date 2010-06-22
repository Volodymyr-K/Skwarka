/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./LDSampler.test.h"

static LDSamplerTestSuite suite_LDSamplerTestSuite;

static CxxTest::List Tests_LDSamplerTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_LDSamplerTestSuite( "./LDSampler.test.h", 12, "LDSamplerTestSuite", suite_LDSamplerTestSuite, Tests_LDSamplerTestSuite );

static class TestDescription_LDSamplerTestSuite_test_LDSampler_Constr : public CxxTest::RealTestDescription {
public:
 TestDescription_LDSamplerTestSuite_test_LDSampler_Constr() : CxxTest::RealTestDescription( Tests_LDSamplerTestSuite, suiteDescription_LDSamplerTestSuite, 15, "test_LDSampler_Constr" ) {}
 void runTest() { suite_LDSamplerTestSuite.test_LDSampler_Constr(); }
} testDescription_LDSamplerTestSuite_test_LDSampler_Constr;

static class TestDescription_LDSamplerTestSuite_test_LDSampler_ImagePoints : public CxxTest::RealTestDescription {
public:
 TestDescription_LDSamplerTestSuite_test_LDSampler_ImagePoints() : CxxTest::RealTestDescription( Tests_LDSamplerTestSuite, suiteDescription_LDSamplerTestSuite, 23, "test_LDSampler_ImagePoints" ) {}
 void runTest() { suite_LDSamplerTestSuite.test_LDSampler_ImagePoints(); }
} testDescription_LDSamplerTestSuite_test_LDSampler_ImagePoints;

static class TestDescription_LDSamplerTestSuite_test_LDSampler_LensUVs : public CxxTest::RealTestDescription {
public:
 TestDescription_LDSamplerTestSuite_test_LDSampler_LensUVs() : CxxTest::RealTestDescription( Tests_LDSamplerTestSuite, suiteDescription_LDSamplerTestSuite, 53, "test_LDSampler_LensUVs" ) {}
 void runTest() { suite_LDSamplerTestSuite.test_LDSampler_LensUVs(); }
} testDescription_LDSamplerTestSuite_test_LDSampler_LensUVs;

static class TestDescription_LDSamplerTestSuite_test_LDSampler_1DSequences : public CxxTest::RealTestDescription {
public:
 TestDescription_LDSamplerTestSuite_test_LDSampler_1DSequences() : CxxTest::RealTestDescription( Tests_LDSamplerTestSuite, suiteDescription_LDSamplerTestSuite, 83, "test_LDSampler_1DSequences" ) {}
 void runTest() { suite_LDSamplerTestSuite.test_LDSampler_1DSequences(); }
} testDescription_LDSamplerTestSuite_test_LDSampler_1DSequences;

static class TestDescription_LDSamplerTestSuite_test_LDSampler_2DSequences : public CxxTest::RealTestDescription {
public:
 TestDescription_LDSamplerTestSuite_test_LDSampler_2DSequences() : CxxTest::RealTestDescription( Tests_LDSamplerTestSuite, suiteDescription_LDSamplerTestSuite, 119, "test_LDSampler_2DSequences" ) {}
 void runTest() { suite_LDSamplerTestSuite.test_LDSampler_2DSequences(); }
} testDescription_LDSamplerTestSuite_test_LDSampler_2DSequences;

