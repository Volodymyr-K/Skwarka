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
CxxTest::StaticSuiteDescription suiteDescription_KDTreeTestSuite( "./KDTree.test.h", 25, "KDTreeTestSuite", suite_KDTreeTestSuite, Tests_KDTreeTestSuite );

static class TestDescription_KDTreeTestSuite_test_KDTree_GetNumberOfPoints : public CxxTest::RealTestDescription {
public:
 TestDescription_KDTreeTestSuite_test_KDTree_GetNumberOfPoints() : CxxTest::RealTestDescription( Tests_KDTreeTestSuite, suiteDescription_KDTreeTestSuite, 54, "test_KDTree_GetNumberOfPoints" ) {}
 void runTest() { suite_KDTreeTestSuite.test_KDTree_GetNumberOfPoints(); }
} testDescription_KDTreeTestSuite_test_KDTree_GetNumberOfPoints;

static class TestDescription_KDTreeTestSuite_test_KDTree_GetAllPoints : public CxxTest::RealTestDescription {
public:
 TestDescription_KDTreeTestSuite_test_KDTree_GetAllPoints() : CxxTest::RealTestDescription( Tests_KDTreeTestSuite, suiteDescription_KDTreeTestSuite, 59, "test_KDTree_GetAllPoints" ) {}
 void runTest() { suite_KDTreeTestSuite.test_KDTree_GetAllPoints(); }
} testDescription_KDTreeTestSuite_test_KDTree_GetAllPoints;

static class TestDescription_KDTreeTestSuite_test_KDTree_NearestPoint : public CxxTest::RealTestDescription {
public:
 TestDescription_KDTreeTestSuite_test_KDTree_NearestPoint() : CxxTest::RealTestDescription( Tests_KDTreeTestSuite, suiteDescription_KDTreeTestSuite, 65, "test_KDTree_NearestPoint" ) {}
 void runTest() { suite_KDTreeTestSuite.test_KDTree_NearestPoint(); }
} testDescription_KDTreeTestSuite_test_KDTree_NearestPoint;

static class TestDescription_KDTreeTestSuite_test_KDTree_NearestPointWithFilter : public CxxTest::RealTestDescription {
public:
 TestDescription_KDTreeTestSuite_test_KDTree_NearestPointWithFilter() : CxxTest::RealTestDescription( Tests_KDTreeTestSuite, suiteDescription_KDTreeTestSuite, 104, "test_KDTree_NearestPointWithFilter" ) {}
 void runTest() { suite_KDTreeTestSuite.test_KDTree_NearestPointWithFilter(); }
} testDescription_KDTreeTestSuite_test_KDTree_NearestPointWithFilter;

static class TestDescription_KDTreeTestSuite_test_KDTree_NearestPoints : public CxxTest::RealTestDescription {
public:
 TestDescription_KDTreeTestSuite_test_KDTree_NearestPoints() : CxxTest::RealTestDescription( Tests_KDTreeTestSuite, suiteDescription_KDTreeTestSuite, 146, "test_KDTree_NearestPoints" ) {}
 void runTest() { suite_KDTreeTestSuite.test_KDTree_NearestPoints(); }
} testDescription_KDTreeTestSuite_test_KDTree_NearestPoints;

static class TestDescription_KDTreeTestSuite_test_KDTree_NearestPointsWithFilter : public CxxTest::RealTestDescription {
public:
 TestDescription_KDTreeTestSuite_test_KDTree_NearestPointsWithFilter() : CxxTest::RealTestDescription( Tests_KDTreeTestSuite, suiteDescription_KDTreeTestSuite, 193, "test_KDTree_NearestPointsWithFilter" ) {}
 void runTest() { suite_KDTreeTestSuite.test_KDTree_NearestPointsWithFilter(); }
} testDescription_KDTreeTestSuite_test_KDTree_NearestPointsWithFilter;

static class TestDescription_KDTreeTestSuite_test_KDTree_EmptyTree : public CxxTest::RealTestDescription {
public:
 TestDescription_KDTreeTestSuite_test_KDTree_EmptyTree() : CxxTest::RealTestDescription( Tests_KDTreeTestSuite, suiteDescription_KDTreeTestSuite, 244, "test_KDTree_EmptyTree" ) {}
 void runTest() { suite_KDTreeTestSuite.test_KDTree_EmptyTree(); }
} testDescription_KDTreeTestSuite_test_KDTree_EmptyTree;

static class TestDescription_KDTreeTestSuite_test_KDTree_VectorType : public CxxTest::RealTestDescription {
public:
 TestDescription_KDTreeTestSuite_test_KDTree_VectorType() : CxxTest::RealTestDescription( Tests_KDTreeTestSuite, suiteDescription_KDTreeTestSuite, 260, "test_KDTree_VectorType" ) {}
 void runTest() { suite_KDTreeTestSuite.test_KDTree_VectorType(); }
} testDescription_KDTreeTestSuite_test_KDTree_VectorType;

