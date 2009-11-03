/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Sampler.test.h"

static SamplerTestSuite suite_SamplerTestSuite;

static CxxTest::List Tests_SamplerTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SamplerTestSuite( "./Sampler.test.h", 10, "SamplerTestSuite", suite_SamplerTestSuite, Tests_SamplerTestSuite );

static class TestDescription_SamplerTestSuite_test_Sampler_Constr : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplerTestSuite_test_Sampler_Constr() : CxxTest::RealTestDescription( Tests_SamplerTestSuite, suiteDescription_SamplerTestSuite, 13, "test_Sampler_Constr" ) {}
 void runTest() { suite_SamplerTestSuite.test_Sampler_Constr(); }
} testDescription_SamplerTestSuite_test_Sampler_Constr;

static class TestDescription_SamplerTestSuite_test_Sampler_CreateSample : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplerTestSuite_test_Sampler_CreateSample() : CxxTest::RealTestDescription( Tests_SamplerTestSuite, suiteDescription_SamplerTestSuite, 20, "test_Sampler_CreateSample" ) {}
 void runTest() { suite_SamplerTestSuite.test_Sampler_CreateSample(); }
} testDescription_SamplerTestSuite_test_Sampler_CreateSample;

static class TestDescription_SamplerTestSuite_test_Sampler_ClearSamplesSequences : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplerTestSuite_test_Sampler_ClearSamplesSequences() : CxxTest::RealTestDescription( Tests_SamplerTestSuite, suiteDescription_SamplerTestSuite, 44, "test_Sampler_ClearSamplesSequences" ) {}
 void runTest() { suite_SamplerTestSuite.test_Sampler_ClearSamplesSequences(); }
} testDescription_SamplerTestSuite_test_Sampler_ClearSamplesSequences;

static class TestDescription_SamplerTestSuite_test_Sampler_GeneratedSamplesCount : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplerTestSuite_test_Sampler_GeneratedSamplesCount() : CxxTest::RealTestDescription( Tests_SamplerTestSuite, suiteDescription_SamplerTestSuite, 68, "test_Sampler_GeneratedSamplesCount" ) {}
 void runTest() { suite_SamplerTestSuite.test_Sampler_GeneratedSamplesCount(); }
} testDescription_SamplerTestSuite_test_Sampler_GeneratedSamplesCount;

static class TestDescription_SamplerTestSuite_test_Sampler_Reset : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplerTestSuite_test_Sampler_Reset() : CxxTest::RealTestDescription( Tests_SamplerTestSuite, suiteDescription_SamplerTestSuite, 80, "test_Sampler_Reset" ) {}
 void runTest() { suite_SamplerTestSuite.test_Sampler_Reset(); }
} testDescription_SamplerTestSuite_test_Sampler_Reset;
