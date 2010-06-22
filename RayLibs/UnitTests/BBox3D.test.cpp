/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

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

static class TestDescription_BBox3DTestSuite_test_BBox3D_Area : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_Area() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 58, "test_BBox3D_Area" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_Area(); }
} testDescription_BBox3DTestSuite_test_BBox3D_Area;

static class TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideCompletely : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideCompletely() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 65, "test_BBox3D_PointInsideCompletely" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_PointInsideCompletely(); }
} testDescription_BBox3DTestSuite_test_BBox3D_PointInsideCompletely;

static class TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnFace : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnFace() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 71, "test_BBox3D_PointInsideOnFace" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_PointInsideOnFace(); }
} testDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnFace;

static class TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnEdge : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnEdge() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 77, "test_BBox3D_PointInsideOnEdge" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_PointInsideOnEdge(); }
} testDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnEdge;

static class TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnVertex : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnVertex() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 83, "test_BBox3D_PointInsideOnVertex" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_PointInsideOnVertex(); }
} testDescription_BBox3DTestSuite_test_BBox3D_PointInsideOnVertex;

static class TestDescription_BBox3DTestSuite_test_BBox3D_PointNotInside : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_PointNotInside() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 89, "test_BBox3D_PointNotInside" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_PointNotInside(); }
} testDescription_BBox3DTestSuite_test_BBox3D_PointNotInside;

static class TestDescription_BBox3DTestSuite_test_BBox3D_TriangleInside : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_TriangleInside() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 95, "test_BBox3D_TriangleInside" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_TriangleInside(); }
} testDescription_BBox3DTestSuite_test_BBox3D_TriangleInside;

static class TestDescription_BBox3DTestSuite_test_BBox3D_TriangleInsidePartially : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_TriangleInsidePartially() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 105, "test_BBox3D_TriangleInsidePartially" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_TriangleInsidePartially(); }
} testDescription_BBox3DTestSuite_test_BBox3D_TriangleInsidePartially;

static class TestDescription_BBox3DTestSuite_test_BBox3D_TriangleNotInside : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_TriangleNotInside() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 115, "test_BBox3D_TriangleNotInside" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_TriangleNotInside(); }
} testDescription_BBox3DTestSuite_test_BBox3D_TriangleNotInside;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect1 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect1() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 126, "test_BBox3D_RayIntersect1" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayIntersect1(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayIntersect1;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect2 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect2() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 139, "test_BBox3D_RayIntersect2" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayIntersect2(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayIntersect2;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect3 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect3() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 152, "test_BBox3D_RayIntersect3" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayIntersect3(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayIntersect3;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect4 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayIntersect4() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 165, "test_BBox3D_RayIntersect4" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayIntersect4(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayIntersect4;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect1 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect1() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 178, "test_BBox3D_RayNotIntersect1" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayNotIntersect1(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect1;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect2 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect2() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 188, "test_BBox3D_RayNotIntersect2" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayNotIntersect2(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect2;

static class TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect3 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect3() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 199, "test_BBox3D_RayNotIntersect3" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_RayNotIntersect3(); }
} testDescription_BBox3DTestSuite_test_BBox3D_RayNotIntersect3;

static class TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithPoint1 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithPoint1() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 209, "test_BBox3D_UnionWithPoint1" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_UnionWithPoint1(); }
} testDescription_BBox3DTestSuite_test_BBox3D_UnionWithPoint1;

static class TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithPoint2 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithPoint2() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 219, "test_BBox3D_UnionWithPoint2" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_UnionWithPoint2(); }
} testDescription_BBox3DTestSuite_test_BBox3D_UnionWithPoint2;

static class TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithPoint_InfinityValues : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithPoint_InfinityValues() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 231, "test_BBox3D_UnionWithPoint_InfinityValues" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_UnionWithPoint_InfinityValues(); }
} testDescription_BBox3DTestSuite_test_BBox3D_UnionWithPoint_InfinityValues;

static class TestDescription_BBox3DTestSuite_test_BBox3D_UniteWithPoint : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_UniteWithPoint() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 241, "test_BBox3D_UniteWithPoint" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_UniteWithPoint(); }
} testDescription_BBox3DTestSuite_test_BBox3D_UniteWithPoint;

static class TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithBox1 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithBox1() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 251, "test_BBox3D_UnionWithBox1" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_UnionWithBox1(); }
} testDescription_BBox3DTestSuite_test_BBox3D_UnionWithBox1;

static class TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithBox2 : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithBox2() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 261, "test_BBox3D_UnionWithBox2" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_UnionWithBox2(); }
} testDescription_BBox3DTestSuite_test_BBox3D_UnionWithBox2;

static class TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithBox_InfinityValues : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_UnionWithBox_InfinityValues() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 273, "test_BBox3D_UnionWithBox_InfinityValues" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_UnionWithBox_InfinityValues(); }
} testDescription_BBox3DTestSuite_test_BBox3D_UnionWithBox_InfinityValues;

static class TestDescription_BBox3DTestSuite_test_BBox3D_UniteWithBox : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_UniteWithBox() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 283, "test_BBox3D_UniteWithBox" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_UniteWithBox(); }
} testDescription_BBox3DTestSuite_test_BBox3D_UniteWithBox;

static class TestDescription_BBox3DTestSuite_test_BBox3D_Conversion : public CxxTest::RealTestDescription {
public:
 TestDescription_BBox3DTestSuite_test_BBox3D_Conversion() : CxxTest::RealTestDescription( Tests_BBox3DTestSuite, suiteDescription_BBox3DTestSuite, 293, "test_BBox3D_Conversion" ) {}
 void runTest() { suite_BBox3DTestSuite.test_BBox3D_Conversion(); }
} testDescription_BBox3DTestSuite_test_BBox3D_Conversion;

