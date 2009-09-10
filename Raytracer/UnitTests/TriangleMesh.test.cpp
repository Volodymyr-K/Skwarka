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

bool TriangleMeshTestSuite_init = false;
#include "./TriangleMesh.test.h"

static TriangleMeshTestSuite suite_TriangleMeshTestSuite;

static CxxTest::List Tests_TriangleMeshTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TriangleMeshTestSuite( "./TriangleMesh.test.h", 10, "TriangleMeshTestSuite", suite_TriangleMeshTestSuite, Tests_TriangleMeshTestSuite );

static class TestDescription_TriangleMeshTestSuite_test_TriangleMesh_Constr : public CxxTest::RealTestDescription {
public:
 TestDescription_TriangleMeshTestSuite_test_TriangleMesh_Constr() : CxxTest::RealTestDescription( Tests_TriangleMeshTestSuite, suiteDescription_TriangleMeshTestSuite, 13, "test_TriangleMesh_Constr" ) {}
 void runTest() { suite_TriangleMeshTestSuite.test_TriangleMesh_Constr(); }
} testDescription_TriangleMeshTestSuite_test_TriangleMesh_Constr;

static class TestDescription_TriangleMeshTestSuite_test_TriangleMesh_EmptyMesh : public CxxTest::RealTestDescription {
public:
 TestDescription_TriangleMeshTestSuite_test_TriangleMesh_EmptyMesh() : CxxTest::RealTestDescription( Tests_TriangleMeshTestSuite, suiteDescription_TriangleMeshTestSuite, 21, "test_TriangleMesh_EmptyMesh" ) {}
 void runTest() { suite_TriangleMeshTestSuite.test_TriangleMesh_EmptyMesh(); }
} testDescription_TriangleMeshTestSuite_test_TriangleMesh_EmptyMesh;

static class TestDescription_TriangleMeshTestSuite_test_TriangleMesh_SolidTopologyInfo : public CxxTest::RealTestDescription {
public:
 TestDescription_TriangleMeshTestSuite_test_TriangleMesh_SolidTopologyInfo() : CxxTest::RealTestDescription( Tests_TriangleMeshTestSuite, suiteDescription_TriangleMeshTestSuite, 32, "test_TriangleMesh_SolidTopologyInfo" ) {}
 void runTest() { suite_TriangleMeshTestSuite.test_TriangleMesh_SolidTopologyInfo(); }
} testDescription_TriangleMeshTestSuite_test_TriangleMesh_SolidTopologyInfo;

static class TestDescription_TriangleMeshTestSuite_test_TriangleMesh_NonSolidTopologyInfo : public CxxTest::RealTestDescription {
public:
 TestDescription_TriangleMeshTestSuite_test_TriangleMesh_NonSolidTopologyInfo() : CxxTest::RealTestDescription( Tests_TriangleMeshTestSuite, suiteDescription_TriangleMeshTestSuite, 41, "test_TriangleMesh_NonSolidTopologyInfo" ) {}
 void runTest() { suite_TriangleMeshTestSuite.test_TriangleMesh_NonSolidTopologyInfo(); }
} testDescription_TriangleMeshTestSuite_test_TriangleMesh_NonSolidTopologyInfo;

static class TestDescription_TriangleMeshTestSuite_test_TriangleMesh_NonManifoldTopologyInfo : public CxxTest::RealTestDescription {
public:
 TestDescription_TriangleMeshTestSuite_test_TriangleMesh_NonManifoldTopologyInfo() : CxxTest::RealTestDescription( Tests_TriangleMeshTestSuite, suiteDescription_TriangleMeshTestSuite, 59, "test_TriangleMesh_NonManifoldTopologyInfo" ) {}
 void runTest() { suite_TriangleMeshTestSuite.test_TriangleMesh_NonManifoldTopologyInfo(); }
} testDescription_TriangleMeshTestSuite_test_TriangleMesh_NonManifoldTopologyInfo;

static class TestDescription_TriangleMeshTestSuite_test_TriangleMesh_TriangleNormals : public CxxTest::RealTestDescription {
public:
 TestDescription_TriangleMeshTestSuite_test_TriangleMesh_TriangleNormals() : CxxTest::RealTestDescription( Tests_TriangleMeshTestSuite, suiteDescription_TriangleMeshTestSuite, 79, "test_TriangleMesh_TriangleNormals" ) {}
 void runTest() { suite_TriangleMeshTestSuite.test_TriangleMesh_TriangleNormals(); }
} testDescription_TriangleMeshTestSuite_test_TriangleMesh_TriangleNormals;

static class TestDescription_TriangleMeshTestSuite_test_TriangleMesh_DifferentialGeometry1 : public CxxTest::RealTestDescription {
public:
 TestDescription_TriangleMeshTestSuite_test_TriangleMesh_DifferentialGeometry1() : CxxTest::RealTestDescription( Tests_TriangleMeshTestSuite, suiteDescription_TriangleMeshTestSuite, 100, "test_TriangleMesh_DifferentialGeometry1" ) {}
 void runTest() { suite_TriangleMeshTestSuite.test_TriangleMesh_DifferentialGeometry1(); }
} testDescription_TriangleMeshTestSuite_test_TriangleMesh_DifferentialGeometry1;

static class TestDescription_TriangleMeshTestSuite_test_TriangleMesh_DifferentialGeometry2 : public CxxTest::RealTestDescription {
public:
 TestDescription_TriangleMeshTestSuite_test_TriangleMesh_DifferentialGeometry2() : CxxTest::RealTestDescription( Tests_TriangleMeshTestSuite, suiteDescription_TriangleMeshTestSuite, 130, "test_TriangleMesh_DifferentialGeometry2" ) {}
 void runTest() { suite_TriangleMeshTestSuite.test_TriangleMesh_DifferentialGeometry2(); }
} testDescription_TriangleMeshTestSuite_test_TriangleMesh_DifferentialGeometry2;

static class TestDescription_TriangleMeshTestSuite_test_TriangleMesh_DifferentialGeometry3 : public CxxTest::RealTestDescription {
public:
 TestDescription_TriangleMeshTestSuite_test_TriangleMesh_DifferentialGeometry3() : CxxTest::RealTestDescription( Tests_TriangleMeshTestSuite, suiteDescription_TriangleMeshTestSuite, 159, "test_TriangleMesh_DifferentialGeometry3" ) {}
 void runTest() { suite_TriangleMeshTestSuite.test_TriangleMesh_DifferentialGeometry3(); }
} testDescription_TriangleMeshTestSuite_test_TriangleMesh_DifferentialGeometry3;

static class TestDescription_TriangleMeshTestSuite_test_TriangleMesh_ShadingNormal : public CxxTest::RealTestDescription {
public:
 TestDescription_TriangleMeshTestSuite_test_TriangleMesh_ShadingNormal() : CxxTest::RealTestDescription( Tests_TriangleMeshTestSuite, suiteDescription_TriangleMeshTestSuite, 189, "test_TriangleMesh_ShadingNormal" ) {}
 void runTest() { suite_TriangleMeshTestSuite.test_TriangleMesh_ShadingNormal(); }
} testDescription_TriangleMeshTestSuite_test_TriangleMesh_ShadingNormal;

