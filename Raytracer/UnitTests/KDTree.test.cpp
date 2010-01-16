/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./KDTree.test.h"

static KDTreeTestSuite suite_KDTreeTestSuite;

static CxxTest::List Tests_KDTreeTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_KDTreeTestSuite( "./KDTree.test.h", 11, "KDTreeTestSuite", suite_KDTreeTestSuite, Tests_KDTreeTestSuite );

static class TestDescription_KDTreeTestSuite_test_KDTree_NearestPoint : public CxxTest::RealTestDescription {
public:
 TestDescription_KDTreeTestSuite_test_KDTree_NearestPoint() : CxxTest::RealTestDescription( Tests_KDTreeTestSuite, suiteDescription_KDTreeTestSuite, 38, "test_KDTree_NearestPoint" ) {}
 void runTest() { suite_KDTreeTestSuite.test_KDTree_NearestPoint(); }
} testDescription_KDTreeTestSuite_test_KDTree_NearestPoint;

static class TestDescription_KDTreeTestSuite_test_KDTree_NearestPoints : public CxxTest::RealTestDescription {
public:
 TestDescription_KDTreeTestSuite_test_KDTree_NearestPoints() : CxxTest::RealTestDescription( Tests_KDTreeTestSuite, suiteDescription_KDTreeTestSuite, 77, "test_KDTree_NearestPoints" ) {}
 void runTest() { suite_KDTreeTestSuite.test_KDTree_NearestPoints(); }
} testDescription_KDTreeTestSuite_test_KDTree_NearestPoints;

static class TestDescription_KDTreeTestSuite_test_KDTree_VectorType : public CxxTest::RealTestDescription {
public:
 TestDescription_KDTreeTestSuite_test_KDTree_VectorType() : CxxTest::RealTestDescription( Tests_KDTreeTestSuite, suiteDescription_KDTreeTestSuite, 123, "test_KDTree_VectorType" ) {}
 void runTest() { suite_KDTreeTestSuite.test_KDTree_VectorType(); }
} testDescription_KDTreeTestSuite_test_KDTree_VectorType;

