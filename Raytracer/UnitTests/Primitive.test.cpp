/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Primitive.test.h"

static PrimitiveTestSuite suite_PrimitiveTestSuite;

static CxxTest::List Tests_PrimitiveTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_PrimitiveTestSuite( "./Primitive.test.h", 15, "PrimitiveTestSuite", suite_PrimitiveTestSuite, Tests_PrimitiveTestSuite );

static class TestDescription_PrimitiveTestSuite_test_Primitive_Constr : public CxxTest::RealTestDescription {
public:
 TestDescription_PrimitiveTestSuite_test_Primitive_Constr() : CxxTest::RealTestDescription( Tests_PrimitiveTestSuite, suiteDescription_PrimitiveTestSuite, 18, "test_Primitive_Constr" ) {}
 void runTest() { suite_PrimitiveTestSuite.test_Primitive_Constr(); }
} testDescription_PrimitiveTestSuite_test_Primitive_Constr;

static class TestDescription_PrimitiveTestSuite_test_Primitive_BumpMap1 : public CxxTest::RealTestDescription {
public:
 TestDescription_PrimitiveTestSuite_test_Primitive_BumpMap1() : CxxTest::RealTestDescription( Tests_PrimitiveTestSuite, suiteDescription_PrimitiveTestSuite, 30, "test_Primitive_BumpMap1" ) {}
 void runTest() { suite_PrimitiveTestSuite.test_Primitive_BumpMap1(); }
} testDescription_PrimitiveTestSuite_test_Primitive_BumpMap1;

