/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./ConstantTexture.test.h"

static ConstantTextureTestSuite suite_ConstantTextureTestSuite;

static CxxTest::List Tests_ConstantTextureTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ConstantTextureTestSuite( "./ConstantTexture.test.h", 9, "ConstantTextureTestSuite", suite_ConstantTextureTestSuite, Tests_ConstantTextureTestSuite );

static class TestDescription_ConstantTextureTestSuite_test_ConstantTexture_Evaluate : public CxxTest::RealTestDescription {
public:
 TestDescription_ConstantTextureTestSuite_test_ConstantTexture_Evaluate() : CxxTest::RealTestDescription( Tests_ConstantTextureTestSuite, suiteDescription_ConstantTextureTestSuite, 12, "test_ConstantTexture_Evaluate" ) {}
 void runTest() { suite_ConstantTextureTestSuite.test_ConstantTexture_Evaluate(); }
} testDescription_ConstantTextureTestSuite_test_ConstantTexture_Evaluate;

