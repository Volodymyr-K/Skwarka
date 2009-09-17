/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./SamplingRoutines.test.h"

static SamplingRoutinesTestSuite suite_SamplingRoutinesTestSuite;

static CxxTest::List Tests_SamplingRoutinesTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SamplingRoutinesTestSuite( "./SamplingRoutines.test.h", 13, "SamplingRoutinesTestSuite", suite_SamplingRoutinesTestSuite, Tests_SamplingRoutinesTestSuite );

static class TestDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSampling_RadiusRange : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSampling_RadiusRange() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 18, "test_ConcentricDiskSampling_RadiusRange" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_ConcentricDiskSampling_RadiusRange(); }
} testDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSampling_RadiusRange;

static class TestDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSampling_Covering : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSampling_Covering() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 38, "test_ConcentricDiskSampling_Covering" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_ConcentricDiskSampling_Covering(); }
} testDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSampling_Covering;

static class TestDescription_SamplingRoutinesTestSuite_test_UniformHemisphereSampling_Range : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_UniformHemisphereSampling_Range() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 72, "test_UniformHemisphereSampling_Range" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_UniformHemisphereSampling_Range(); }
} testDescription_SamplingRoutinesTestSuite_test_UniformHemisphereSampling_Range;

static class TestDescription_SamplingRoutinesTestSuite_test_UniformHemispherePDF : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_UniformHemispherePDF() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 94, "test_UniformHemispherePDF" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_UniformHemispherePDF(); }
} testDescription_SamplingRoutinesTestSuite_test_UniformHemispherePDF;

static class TestDescription_SamplingRoutinesTestSuite_test_CosineHemisphereSampling_Range : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_CosineHemisphereSampling_Range() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 100, "test_CosineHemisphereSampling_Range" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_CosineHemisphereSampling_Range(); }
} testDescription_SamplingRoutinesTestSuite_test_CosineHemisphereSampling_Range;

static class TestDescription_SamplingRoutinesTestSuite_test_CosineHemispherePDF : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_CosineHemispherePDF() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 122, "test_CosineHemispherePDF" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_CosineHemispherePDF(); }
} testDescription_SamplingRoutinesTestSuite_test_CosineHemispherePDF;

static class TestDescription_SamplingRoutinesTestSuite_test_Stratified1D_Range : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_Stratified1D_Range() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 139, "test_Stratified1D_Range" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_Stratified1D_Range(); }
} testDescription_SamplingRoutinesTestSuite_test_Stratified1D_Range;

static class TestDescription_SamplingRoutinesTestSuite_test_Stratified1D_Clumping : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_Stratified1D_Clumping() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 155, "test_Stratified1D_Clumping" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_Stratified1D_Clumping(); }
} testDescription_SamplingRoutinesTestSuite_test_Stratified1D_Clumping;

static class TestDescription_SamplingRoutinesTestSuite_test_Stratified2D_Range : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_Stratified2D_Range() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 166, "test_Stratified2D_Range" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_Stratified2D_Range(); }
} testDescription_SamplingRoutinesTestSuite_test_Stratified2D_Range;

static class TestDescription_SamplingRoutinesTestSuite_test_Stratified2DClumping : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_Stratified2DClumping() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 188, "test_Stratified2DClumping" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_Stratified2DClumping(); }
} testDescription_SamplingRoutinesTestSuite_test_Stratified2DClumping;

static class TestDescription_SamplingRoutinesTestSuite_test_LatinHypercube2D_Range : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_LatinHypercube2D_Range() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 199, "test_LatinHypercube2D_Range" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_LatinHypercube2D_Range(); }
} testDescription_SamplingRoutinesTestSuite_test_LatinHypercube2D_Range;

static class TestDescription_SamplingRoutinesTestSuite_test_LatinHypercube2D_Clumping : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_LatinHypercube2D_Clumping() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 221, "test_LatinHypercube2D_Clumping" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_LatinHypercube2D_Clumping(); }
} testDescription_SamplingRoutinesTestSuite_test_LatinHypercube2D_Clumping;

static class TestDescription_SamplingRoutinesTestSuite_test_Shuffle : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_Shuffle() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 232, "test_Shuffle" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_Shuffle(); }
} testDescription_SamplingRoutinesTestSuite_test_Shuffle;

