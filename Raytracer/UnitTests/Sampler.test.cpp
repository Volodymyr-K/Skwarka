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

bool SamplerTestSuite_init = false;
#include "./Sampler.test.h"

static SamplerTestSuite suite_SamplerTestSuite;

static CxxTest::List Tests_SamplerTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SamplerTestSuite( "./Sampler.test.h", 10, "SamplerTestSuite", suite_SamplerTestSuite, Tests_SamplerTestSuite );

static class TestDescription_SamplerTestSuite_testSamplerConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplerTestSuite_testSamplerConstr() : CxxTest::RealTestDescription( Tests_SamplerTestSuite, suiteDescription_SamplerTestSuite, 13, "testSamplerConstr" ) {}
 void runTest() { suite_SamplerTestSuite.testSamplerConstr(); }
} testDescription_SamplerTestSuite_testSamplerConstr;

static class TestDescription_SamplerTestSuite_testSamplerCreateSample : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplerTestSuite_testSamplerCreateSample() : CxxTest::RealTestDescription( Tests_SamplerTestSuite, suiteDescription_SamplerTestSuite, 20, "testSamplerCreateSample" ) {}
 void runTest() { suite_SamplerTestSuite.testSamplerCreateSample(); }
} testDescription_SamplerTestSuite_testSamplerCreateSample;

static class TestDescription_SamplerTestSuite_testSamplerGenerateSamples : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplerTestSuite_testSamplerGenerateSamples() : CxxTest::RealTestDescription( Tests_SamplerTestSuite, suiteDescription_SamplerTestSuite, 44, "testSamplerGenerateSamples" ) {}
 void runTest() { suite_SamplerTestSuite.testSamplerGenerateSamples(); }
} testDescription_SamplerTestSuite_testSamplerGenerateSamples;

