/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_LONGLONG long long
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>

bool BBox3DTestSuite_init = false;
#include "./BBox3D.test.h"

static BBox3DTestSuite suite_BBox3DTestSuite;

static CxxTest::List Tests_BBox3DTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_BBox3DTestSuite( "./BBox3D.test.h", 8, "BBox3DTestSuite", suite_BBox3DTestSuite, Tests_BBox3DTestSuite );

static class TestDescription_BBox3DTestSuite_testBBoxDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxDefaultConstr() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 25, "testBBoxDefaultConstr" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxDefaultConstr(); }
} testDescription_BBox3DTestSuite_testBBoxDefaultConstr;

static class TestDescription_BBox3DTestSuite_testBBoxDefaultConstrWithCorners : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxDefaultConstrWithCorners() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 32, "testBBoxDefaultConstrWithCorners" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxDefaultConstrWithCorners(); }
} testDescription_BBox3DTestSuite_testBBoxDefaultConstrWithCorners;

static class TestDescription_BBox3DTestSuite_testBBoxVolume : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxVolume() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 42, "testBBoxVolume" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxVolume(); }
} testDescription_BBox3DTestSuite_testBBoxVolume;

static class TestDescription_BBox3DTestSuite_testBBoxNegativeVolume : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxNegativeVolume() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 48, "testBBoxNegativeVolume" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxNegativeVolume(); }
} testDescription_BBox3DTestSuite_testBBoxNegativeVolume;

static class TestDescription_BBox3DTestSuite_testBBoxPointInsideCompletely : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxPointInsideCompletely() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 58, "testBBoxPointInsideCompletely" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxPointInsideCompletely(); }
} testDescription_BBox3DTestSuite_testBBoxPointInsideCompletely;

static class TestDescription_BBox3DTestSuite_testBBoxPointInsideOnFace : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxPointInsideOnFace() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 64, "testBBoxPointInsideOnFace" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxPointInsideOnFace(); }
} testDescription_BBox3DTestSuite_testBBoxPointInsideOnFace;

static class TestDescription_BBox3DTestSuite_testBBoxPointInsideOnEdge : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxPointInsideOnEdge() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 70, "testBBoxPointInsideOnEdge" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxPointInsideOnEdge(); }
} testDescription_BBox3DTestSuite_testBBoxPointInsideOnEdge;

static class TestDescription_BBox3DTestSuite_testBBoxPointInsideOnVertex : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxPointInsideOnVertex() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 76, "testBBoxPointInsideOnVertex" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxPointInsideOnVertex(); }
} testDescription_BBox3DTestSuite_testBBoxPointInsideOnVertex;

static class TestDescription_BBox3DTestSuite_testBBoxPointNotInside : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxPointNotInside() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 82, "testBBoxPointNotInside" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxPointNotInside(); }
} testDescription_BBox3DTestSuite_testBBoxPointNotInside;

static class TestDescription_BBox3DTestSuite_testBBoxTriangleInside : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxTriangleInside() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 88, "testBBoxTriangleInside" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxTriangleInside(); }
} testDescription_BBox3DTestSuite_testBBoxTriangleInside;

static class TestDescription_BBox3DTestSuite_testBBoxTriangleInsidePartially : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxTriangleInsidePartially() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 98, "testBBoxTriangleInsidePartially" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxTriangleInsidePartially(); }
} testDescription_BBox3DTestSuite_testBBoxTriangleInsidePartially;

static class TestDescription_BBox3DTestSuite_testBBoxTriangleNotInside : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxTriangleNotInside() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 108, "testBBoxTriangleNotInside" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxTriangleNotInside(); }
} testDescription_BBox3DTestSuite_testBBoxTriangleNotInside;

static class TestDescription_BBox3DTestSuite_testBBoxRayIntersect1 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxRayIntersect1() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 119, "testBBoxRayIntersect1" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxRayIntersect1(); }
} testDescription_BBox3DTestSuite_testBBoxRayIntersect1;

static class TestDescription_BBox3DTestSuite_testBBoxRayIntersect2 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxRayIntersect2() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 129, "testBBoxRayIntersect2" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxRayIntersect2(); }
} testDescription_BBox3DTestSuite_testBBoxRayIntersect2;

static class TestDescription_BBox3DTestSuite_testBBoxRayIntersect3 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxRayIntersect3() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 139, "testBBoxRayIntersect3" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxRayIntersect3(); }
} testDescription_BBox3DTestSuite_testBBoxRayIntersect3;

static class TestDescription_BBox3DTestSuite_testBBoxRayIntersect4 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxRayIntersect4() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 149, "testBBoxRayIntersect4" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxRayIntersect4(); }
} testDescription_BBox3DTestSuite_testBBoxRayIntersect4;

static class TestDescription_BBox3DTestSuite_testBBoxRayNotIntersect1 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxRayNotIntersect1() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 159, "testBBoxRayNotIntersect1" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxRayNotIntersect1(); }
} testDescription_BBox3DTestSuite_testBBoxRayNotIntersect1;

static class TestDescription_BBox3DTestSuite_testBBoxRayNotIntersect2 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxRayNotIntersect2() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 169, "testBBoxRayNotIntersect2" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxRayNotIntersect2(); }
} testDescription_BBox3DTestSuite_testBBoxRayNotIntersect2;

static class TestDescription_BBox3DTestSuite_testBBoxRayNotIntersect3 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_testBBoxRayNotIntersect3() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 180, "testBBoxRayNotIntersect3" ) {}
 void runTest() { suite_BBox3DTestSuite.testBBoxRayNotIntersect3(); }
} testDescription_BBox3DTestSuite_testBBoxRayNotIntersect3;

