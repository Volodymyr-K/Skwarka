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

bool SamplingRoutinesTestSuite_init = false;
#include "./SamplingRoutines.test.h"

static SamplingRoutinesTestSuite suite_SamplingRoutinesTestSuite;

static CxxTest::List Tests_SamplingRoutinesTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SamplingRoutinesTestSuite( "./SamplingRoutines.test.h", 13, "SamplingRoutinesTestSuite", suite_SamplingRoutinesTestSuite, Tests_SamplingRoutinesTestSuite );

static class TestDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSamplingRadiusRange : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSamplingRadiusRange() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 18, "test_ConcentricDiskSamplingRadiusRange" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_ConcentricDiskSamplingRadiusRange(); }
} testDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSamplingRadiusRange;

static class TestDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSamplingCovering : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSamplingCovering() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 38, "test_ConcentricDiskSamplingCovering" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_ConcentricDiskSamplingCovering(); }
} testDescription_SamplingRoutinesTestSuite_test_ConcentricDiskSamplingCovering;

static class TestDescription_SamplingRoutinesTestSuite_test_Stratified1DRange : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_Stratified1DRange() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 73, "test_Stratified1DRange" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_Stratified1DRange(); }
} testDescription_SamplingRoutinesTestSuite_test_Stratified1DRange;

static class TestDescription_SamplingRoutinesTestSuite_test_Stratified1DClumping : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_Stratified1DClumping() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 89, "test_Stratified1DClumping" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_Stratified1DClumping(); }
} testDescription_SamplingRoutinesTestSuite_test_Stratified1DClumping;

static class TestDescription_SamplingRoutinesTestSuite_test_Stratified2DRange : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_Stratified2DRange() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 100, "test_Stratified2DRange" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_Stratified2DRange(); }
} testDescription_SamplingRoutinesTestSuite_test_Stratified2DRange;

static class TestDescription_SamplingRoutinesTestSuite_test_Stratified2DClumping : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_Stratified2DClumping() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 122, "test_Stratified2DClumping" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_Stratified2DClumping(); }
} testDescription_SamplingRoutinesTestSuite_test_Stratified2DClumping;

static class TestDescription_SamplingRoutinesTestSuite_test_LatinHypercube2DRange : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_LatinHypercube2DRange() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 133, "test_LatinHypercube2DRange" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_LatinHypercube2DRange(); }
} testDescription_SamplingRoutinesTestSuite_test_LatinHypercube2DRange;

static class TestDescription_SamplingRoutinesTestSuite_test_LatinHypercube2DClumping : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_LatinHypercube2DClumping() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 155, "test_LatinHypercube2DClumping" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_LatinHypercube2DClumping(); }
} testDescription_SamplingRoutinesTestSuite_test_LatinHypercube2DClumping;

static class TestDescription_SamplingRoutinesTestSuite_test_Shuffle : public CxxTest::RealTestDescription {
public:
 TestDescription_SamplingRoutinesTestSuite_test_Shuffle() : CxxTest::RealTestDescription( Tests_SamplingRoutinesTestSuite, suiteDescription_SamplingRoutinesTestSuite, 166, "test_Shuffle" ) {}
 void runTest() { suite_SamplingRoutinesTestSuite.test_Shuffle(); }
} testDescription_SamplingRoutinesTestSuite_test_Shuffle;

