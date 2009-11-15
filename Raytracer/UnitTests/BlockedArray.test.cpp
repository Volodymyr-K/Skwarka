/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./BlockedArray.test.h"

static BlockedArrayTestSuite suite_BlockedArrayTestSuite;

static CxxTest::List Tests_BlockedArrayTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_BlockedArrayTestSuite( "./BlockedArray.test.h", 9, "BlockedArrayTestSuite", suite_BlockedArrayTestSuite, Tests_BlockedArrayTestSuite );

static class TestDescription_BlockedArrayTestSuite_test_BlockedArray_ConstrFromData : public CxxTest::RealTestDescription {
public:
 TestDescription_BlockedArrayTestSuite_test_BlockedArray_ConstrFromData() : CxxTest::RealTestDescription( Tests_BlockedArrayTestSuite, suiteDescription_BlockedArrayTestSuite, 12, "test_BlockedArray_ConstrFromData" ) {}
 void runTest() { suite_BlockedArrayTestSuite.test_BlockedArray_ConstrFromData(); }
} testDescription_BlockedArrayTestSuite_test_BlockedArray_ConstrFromData;

static class TestDescription_BlockedArrayTestSuite_test_BlockedArray_SetGet : public CxxTest::RealTestDescription {
public:
 TestDescription_BlockedArrayTestSuite_test_BlockedArray_SetGet() : CxxTest::RealTestDescription( Tests_BlockedArrayTestSuite, suiteDescription_BlockedArrayTestSuite, 30, "test_BlockedArray_SetGet" ) {}
 void runTest() { suite_BlockedArrayTestSuite.test_BlockedArray_SetGet(); }
} testDescription_BlockedArrayTestSuite_test_BlockedArray_SetGet;

static class TestDescription_BlockedArrayTestSuite_test_BlockedArray_Fill : public CxxTest::RealTestDescription {
public:
 TestDescription_BlockedArrayTestSuite_test_BlockedArray_Fill() : CxxTest::RealTestDescription( Tests_BlockedArrayTestSuite, suiteDescription_BlockedArrayTestSuite, 47, "test_BlockedArray_Fill" ) {}
 void runTest() { suite_BlockedArrayTestSuite.test_BlockedArray_Fill(); }
} testDescription_BlockedArrayTestSuite_test_BlockedArray_Fill;

