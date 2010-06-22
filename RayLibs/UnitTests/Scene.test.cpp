/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Scene.test.h"

static SceneTestSuite suite_SceneTestSuite;

static CxxTest::List Tests_SceneTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SceneTestSuite( "./Scene.test.h", 17, "SceneTestSuite", suite_SceneTestSuite, Tests_SceneTestSuite );

static class TestDescription_SceneTestSuite_test_Scene_GetPrimitives : public CxxTest::RealTestDescription {
public:
 TestDescription_SceneTestSuite_test_Scene_GetPrimitives() : CxxTest::RealTestDescription( Tests_SceneTestSuite, suiteDescription_SceneTestSuite, 45, "test_Scene_GetPrimitives" ) {}
 void runTest() { suite_SceneTestSuite.test_Scene_GetPrimitives(); }
} testDescription_SceneTestSuite_test_Scene_GetPrimitives;

static class TestDescription_SceneTestSuite_test_Scene_GetWorldBounds : public CxxTest::RealTestDescription {
public:
 TestDescription_SceneTestSuite_test_Scene_GetWorldBounds() : CxxTest::RealTestDescription( Tests_SceneTestSuite, suiteDescription_SceneTestSuite, 50, "test_Scene_GetWorldBounds" ) {}
 void runTest() { suite_SceneTestSuite.test_Scene_GetWorldBounds(); }
} testDescription_SceneTestSuite_test_Scene_GetWorldBounds;

static class TestDescription_SceneTestSuite_test_Scene_Intersect : public CxxTest::RealTestDescription {
public:
 TestDescription_SceneTestSuite_test_Scene_Intersect() : CxxTest::RealTestDescription( Tests_SceneTestSuite, suiteDescription_SceneTestSuite, 60, "test_Scene_Intersect" ) {}
 void runTest() { suite_SceneTestSuite.test_Scene_Intersect(); }
} testDescription_SceneTestSuite_test_Scene_Intersect;

static class TestDescription_SceneTestSuite_test_Scene_IntersectTest1 : public CxxTest::RealTestDescription {
public:
 TestDescription_SceneTestSuite_test_Scene_IntersectTest1() : CxxTest::RealTestDescription( Tests_SceneTestSuite, suiteDescription_SceneTestSuite, 75, "test_Scene_IntersectTest1" ) {}
 void runTest() { suite_SceneTestSuite.test_Scene_IntersectTest1(); }
} testDescription_SceneTestSuite_test_Scene_IntersectTest1;

static class TestDescription_SceneTestSuite_test_Scene_IntersectTest2 : public CxxTest::RealTestDescription {
public:
 TestDescription_SceneTestSuite_test_Scene_IntersectTest2() : CxxTest::RealTestDescription( Tests_SceneTestSuite, suiteDescription_SceneTestSuite, 83, "test_Scene_IntersectTest2" ) {}
 void runTest() { suite_SceneTestSuite.test_Scene_IntersectTest2(); }
} testDescription_SceneTestSuite_test_Scene_IntersectTest2;

