/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Metal.test.h"

static MetalTestSuite suite_MetalTestSuite;

static CxxTest::List Tests_MetalTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_MetalTestSuite( "./Metal.test.h", 16, "MetalTestSuite", suite_MetalTestSuite, Tests_MetalTestSuite );

static class TestDescription_MetalTestSuite_test_Metal : public CxxTest::RealTestDescription {
public:
 TestDescription_MetalTestSuite_test_Metal() : CxxTest::RealTestDescription( Tests_MetalTestSuite, suiteDescription_MetalTestSuite, 20, "test_Metal" ) {}
 void runTest() { suite_MetalTestSuite.test_Metal(); }
} testDescription_MetalTestSuite_test_Metal;

