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

static class TestDescription_BBox3DTestSuite_test_BBox3D_DefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_DefaultConstr() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 25, "test_BBox3D_DefaultConstr" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_DefaultConstr(); }
} testDescription_BBox3DTestSuite_test_BBox3D_DefaultConstr;

static class TestDescription_BBox3DTestSuite_test_BBox3D_DefaultConstrWithCorners : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_DefaultConstrWithCorners() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 32, "test_BBox3D_DefaultConstrWithCorners" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_DefaultConstrWithCorners(); }
} testDescription_BBox3DTestSuite_test_BBox3D_DefaultConstrWithCorners;

static class TestDescription_BBox3DTestSuite_test_BBox3D_Volume : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_Volume() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 42, "test_BBox3D_Volume" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_Volume(); }
} testDescription_BBox3DTestSuite_test_BBox3D_Volume;

static class TestDescription_BBox3DTestSuite_test_BBox3D_NegativeVolume : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_NegativeVolume() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 48, "test_BBox3D_NegativeVolume" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_NegativeVolume(); }
} testDescription_BBox3DTestSuite_test_BBox3D_NegativeVolume;

static class TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideCompletely : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideCompletely() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 58, "test_BBox3D_PointInsideCompletely" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_PointInsideCompletely(); }
} testDescription_BBox3DTestSuite_test_BBox3D_PointInsideCompletely;

static class TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnFace : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnFace() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 64, "test_BBox3D_PointInsideOnFace" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_PointInsideOnFace(); }
} testDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnFace;

static class TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnEdge : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnEdge() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 70, "test_BBox3D_PointInsideOnEdge" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_PointInsideOnEdge(); }
} testDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnEdge;

static class TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnVertex : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnVertex() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 76, "test_BBox3D_PointInsideOnVertex" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_PointInsideOnVertex(); }
} testDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnVertex;

static class TestDescription_BBox3DTestSuite_test_BBox3D_PointNotInside : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_PointNotInside() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 82, "test_BBox3D_PointNotInside" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_PointNotInside(); }
} testDescription_BBox3DTestSuite_test_BBox3D_PointNotInside;

static class TestDescription_BBox3DTestSuite_test_BBox3D_TriangleInside : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_TriangleInside() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 88, "test_BBox3D_TriangleInside" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_TriangleInside(); }
} testDescription_BBox3DTestSuite_test_BBox3D_TriangleInside;

static class TestDescription_BBox3DTestSuite_test_BBox3D_TriangleInsidePartially : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_TriangleInsidePartially() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 98, "test_BBox3D_TriangleInsidePartially" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_TriangleInsidePartially(); }
} testDescription_BBox3DTestSuite_test_BBox3D_TriangleInsidePartially;

static class TestDescription_BBox3DTestSuite_test_BBox3D_TriangleNotInside : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_TriangleNotInside() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 108, "test_BBox3D_TriangleNotInside" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_TriangleNotInside(); }
} testDescription_BBox3DTestSuite_test_BBox3D_TriangleNotInside;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect1 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect1() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 119, "test_BBox3D_RayIntersect1" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayIntersect1(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayIntersect1;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect2 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect2() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 129, "test_BBox3D_RayIntersect2" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayIntersect2(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayIntersect2;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect3 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect3() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 139, "test_BBox3D_RayIntersect3" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayIntersect3(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayIntersect3;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect4 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect4() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 149, "test_BBox3D_RayIntersect4" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayIntersect4(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayIntersect4;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect1 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect1() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 159, "test_BBox3D_RayNotIntersect1" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayNotIntersect1(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect1;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect2 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect2() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 169, "test_BBox3D_RayNotIntersect2" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayNotIntersect2(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect2;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect3 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect3() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 180, "test_BBox3D_RayNotIntersect3" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayNotIntersect3(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect3;

