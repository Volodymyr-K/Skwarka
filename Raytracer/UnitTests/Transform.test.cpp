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

bool TransformTestSuite_init = false;
#include "./Transform.test.h"

static TransformTestSuite suite_TransformTestSuite;

static CxxTest::List Tests_TransformTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TransformTestSuite( "./Transform.test.h", 10, "TransformTestSuite", suite_TransformTestSuite, Tests_TransformTestSuite );

static class TestDescription_TransformTestSuite_test_Transform_DefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_DefaultConstr() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 49, "test_Transform_DefaultConstr" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_DefaultConstr(); }
} testDescription_TransformTestSuite_test_Transform_DefaultConstr;

static class TestDescription_TransformTestSuite_test_Transform_ConstrWithMatrixElements : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_ConstrWithMatrixElements() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 58, "test_Transform_ConstrWithMatrixElements" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_ConstrWithMatrixElements(); }
} testDescription_TransformTestSuite_test_Transform_ConstrWithMatrixElements;

static class TestDescription_TransformTestSuite_test_Transform_ConstrWithMatrix : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_ConstrWithMatrix() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 67, "test_Transform_ConstrWithMatrix" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_ConstrWithMatrix(); }
} testDescription_TransformTestSuite_test_Transform_ConstrWithMatrix;

static class TestDescription_TransformTestSuite_test_Transform_Inverted : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_Inverted() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 76, "test_Transform_Inverted" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_Inverted(); }
} testDescription_TransformTestSuite_test_Transform_Inverted;

static class TestDescription_TransformTestSuite_test_Transform_Point : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_Point() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 86, "test_Transform_Point" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_Point(); }
} testDescription_TransformTestSuite_test_Transform_Point;

static class TestDescription_TransformTestSuite_test_Transform_Vector : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_Vector() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 94, "test_Transform_Vector" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_Vector(); }
} testDescription_TransformTestSuite_test_Transform_Vector;

static class TestDescription_TransformTestSuite_test_Transform_Ray : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_Ray() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 102, "test_Transform_Ray" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_Ray(); }
} testDescription_TransformTestSuite_test_Transform_Ray;

static class TestDescription_TransformTestSuite_test_Transform_Composition : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_Composition() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 110, "test_Transform_Composition" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_Composition(); }
} testDescription_TransformTestSuite_test_Transform_Composition;

static class TestDescription_TransformTestSuite_test_Transform_Translation : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_Translation() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 121, "test_Transform_Translation" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_Translation(); }
} testDescription_TransformTestSuite_test_Transform_Translation;

static class TestDescription_TransformTestSuite_test_Transform_Scale : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_Scale() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 130, "test_Transform_Scale" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_Scale(); }
} testDescription_TransformTestSuite_test_Transform_Scale;

static class TestDescription_TransformTestSuite_test_Transform_RotationX : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_RotationX() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 139, "test_Transform_RotationX" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_RotationX(); }
} testDescription_TransformTestSuite_test_Transform_RotationX;

static class TestDescription_TransformTestSuite_test_Transform_RotationY : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_RotationY() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 148, "test_Transform_RotationY" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_RotationY(); }
} testDescription_TransformTestSuite_test_Transform_RotationY;

static class TestDescription_TransformTestSuite_test_Transform_RotationZ : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_RotationZ() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 157, "test_Transform_RotationZ" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_RotationZ(); }
} testDescription_TransformTestSuite_test_Transform_RotationZ;

static class TestDescription_TransformTestSuite_test_Transform_Rotation : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_Rotation() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 166, "test_Transform_Rotation" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_Rotation(); }
} testDescription_TransformTestSuite_test_Transform_Rotation;

static class TestDescription_TransformTestSuite_test_Transform_LookAt : public CxxTest::RealTestDescription {
public:
 TestDescription_TransformTestSuite_test_Transform_LookAt() : CxxTest::RealTestDescription( Tests_TransformTestSuite, suiteDescription_TransformTestSuite, 175, "test_Transform_LookAt" ) {}
 void runTest() { suite_TransformTestSuite.test_Transform_LookAt(); }
} testDescription_TransformTestSuite_test_Transform_LookAt;

