/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Sphere.test.h"

static SphereTestSuite suite_SphereTestSuite;

static CxxTest::List Tests_SphereTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SphereTestSuite( "./Sphere.test.h", 10, "SphereTestSuite", suite_SphereTestSuite, Tests_SphereTestSuite );

static class TestDescription_SphereTestSuite_test_Sphere_BuildMesh : public CxxTest::RealTestDescription {
public:
 TestDescription_SphereTestSuite_test_Sphere_BuildMesh() : CxxTest::RealTestDescription( Tests_SphereTestSuite, suiteDescription_SphereTestSuite, 14, "test_Sphere_BuildMesh" ) {}
 void runTest() { suite_SphereTestSuite.test_Sphere_BuildMesh(); }
} testDescription_SphereTestSuite_test_Sphere_BuildMesh;

static class TestDescription_SphereTestSuite_test_Sphere_MeshTopology : public CxxTest::RealTestDescription {
public:
 TestDescription_SphereTestSuite_test_Sphere_MeshTopology() : CxxTest::RealTestDescription( Tests_SphereTestSuite, suiteDescription_SphereTestSuite, 25, "test_Sphere_MeshTopology" ) {}
 void runTest() { suite_SphereTestSuite.test_Sphere_MeshTopology(); }
} testDescription_SphereTestSuite_test_Sphere_MeshTopology;

static class TestDescription_SphereTestSuite_test_Sphere_MeshSize1 : public CxxTest::RealTestDescription {
public:
 TestDescription_SphereTestSuite_test_Sphere_MeshSize1() : CxxTest::RealTestDescription( Tests_SphereTestSuite, suiteDescription_SphereTestSuite, 38, "test_Sphere_MeshSize1" ) {}
 void runTest() { suite_SphereTestSuite.test_Sphere_MeshSize1(); }
} testDescription_SphereTestSuite_test_Sphere_MeshSize1;

static class TestDescription_SphereTestSuite_test_Sphere_MeshSize2 : public CxxTest::RealTestDescription {
public:
 TestDescription_SphereTestSuite_test_Sphere_MeshSize2() : CxxTest::RealTestDescription( Tests_SphereTestSuite, suiteDescription_SphereTestSuite, 49, "test_Sphere_MeshSize2" ) {}
 void runTest() { suite_SphereTestSuite.test_Sphere_MeshSize2(); }
} testDescription_SphereTestSuite_test_Sphere_MeshSize2;

static class TestDescription_SphereTestSuite_test_Sphere_NegativeRadius : public CxxTest::RealTestDescription {
public:
 TestDescription_SphereTestSuite_test_Sphere_NegativeRadius() : CxxTest::RealTestDescription( Tests_SphereTestSuite, suiteDescription_SphereTestSuite, 60, "test_Sphere_NegativeRadius" ) {}
 void runTest() { suite_SphereTestSuite.test_Sphere_NegativeRadius(); }
} testDescription_SphereTestSuite_test_Sphere_NegativeRadius;

static class TestDescription_SphereTestSuite_test_Sphere_NegativeSubdivisions : public CxxTest::RealTestDescription {
public:
 TestDescription_SphereTestSuite_test_Sphere_NegativeSubdivisions() : CxxTest::RealTestDescription( Tests_SphereTestSuite, suiteDescription_SphereTestSuite, 71, "test_Sphere_NegativeSubdivisions" ) {}
 void runTest() { suite_SphereTestSuite.test_Sphere_NegativeSubdivisions(); }
} testDescription_SphereTestSuite_test_Sphere_NegativeSubdivisions;

