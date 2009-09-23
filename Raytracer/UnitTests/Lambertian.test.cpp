/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./Lambertian.test.h"

static LambertianTestSuite suite_LambertianTestSuite;

static CxxTest::List Tests_LambertianTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_LambertianTestSuite( "./Lambertian.test.h", 13, "LambertianTestSuite", suite_LambertianTestSuite, Tests_LambertianTestSuite );

static class TestDescription_LambertianTestSuite_test_Lambertian_Type : public CxxTest::RealTestDescription {
public:
 TestDescription_LambertianTestSuite_test_Lambertian_Type() : CxxTest::RealTestDescription( Tests_LambertianTestSuite, suiteDescription_LambertianTestSuite, 16, "test_Lambertian_Type" ) {}
 void runTest() { suite_LambertianTestSuite.test_Lambertian_Type(); }
} testDescription_LambertianTestSuite_test_Lambertian_Type;

static class TestDescription_LambertianTestSuite_test_Lambertian_Sample : public CxxTest::RealTestDescription {
public:
 TestDescription_LambertianTestSuite_test_Lambertian_Sample() : CxxTest::RealTestDescription( Tests_LambertianTestSuite, suiteDescription_LambertianTestSuite, 23, "test_Lambertian_Sample" ) {}
 void runTest() { suite_LambertianTestSuite.test_Lambertian_Sample(); }
} testDescription_LambertianTestSuite_test_Lambertian_Sample;

static class TestDescription_LambertianTestSuite_test_Lambertian_TotalScattering1 : public CxxTest::RealTestDescription {
public:
 TestDescription_LambertianTestSuite_test_Lambertian_TotalScattering1() : CxxTest::RealTestDescription( Tests_LambertianTestSuite, suiteDescription_LambertianTestSuite, 44, "test_Lambertian_TotalScattering1" ) {}
 void runTest() { suite_LambertianTestSuite.test_Lambertian_TotalScattering1(); }
} testDescription_LambertianTestSuite_test_Lambertian_TotalScattering1;

static class TestDescription_LambertianTestSuite_test_Lambertian_TotalScattering2 : public CxxTest::RealTestDescription {
public:
 TestDescription_LambertianTestSuite_test_Lambertian_TotalScattering2() : CxxTest::RealTestDescription( Tests_LambertianTestSuite, suiteDescription_LambertianTestSuite, 56, "test_Lambertian_TotalScattering2" ) {}
 void runTest() { suite_LambertianTestSuite.test_Lambertian_TotalScattering2(); }
} testDescription_LambertianTestSuite_test_Lambertian_TotalScattering2;

