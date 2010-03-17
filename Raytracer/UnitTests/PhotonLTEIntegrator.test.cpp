/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./PhotonLTEIntegrator.test.h"

static PhotonLTEIntegratorTestSuite suite_PhotonLTEIntegratorTestSuite;

static CxxTest::List Tests_PhotonLTEIntegratorTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_PhotonLTEIntegratorTestSuite( "./PhotonLTEIntegrator.test.h", 16, "PhotonLTEIntegratorTestSuite", suite_PhotonLTEIntegratorTestSuite, Tests_PhotonLTEIntegratorTestSuite );

static class TestDescription_PhotonLTEIntegratorTestSuite_test_PhotonLTEIntegrator_SelfIlluminatedSphere : public CxxTest::RealTestDescription {
public:
 TestDescription_PhotonLTEIntegratorTestSuite_test_PhotonLTEIntegrator_SelfIlluminatedSphere() : CxxTest::RealTestDescription( Tests_PhotonLTEIntegratorTestSuite, suiteDescription_PhotonLTEIntegratorTestSuite, 28, "test_PhotonLTEIntegrator_SelfIlluminatedSphere" ) {}
 void runTest() { suite_PhotonLTEIntegratorTestSuite.test_PhotonLTEIntegrator_SelfIlluminatedSphere(); }
} testDescription_PhotonLTEIntegratorTestSuite_test_PhotonLTEIntegrator_SelfIlluminatedSphere;

static class TestDescription_PhotonLTEIntegratorTestSuite_test_PhotonLTEIntegrator_PointLightInSphere : public CxxTest::RealTestDescription {
public:
 TestDescription_PhotonLTEIntegratorTestSuite_test_PhotonLTEIntegrator_PointLightInSphere() : CxxTest::RealTestDescription( Tests_PhotonLTEIntegratorTestSuite, suiteDescription_PhotonLTEIntegratorTestSuite, 70, "test_PhotonLTEIntegrator_PointLightInSphere" ) {}
 void runTest() { suite_PhotonLTEIntegratorTestSuite.test_PhotonLTEIntegrator_PointLightInSphere(); }
} testDescription_PhotonLTEIntegratorTestSuite_test_PhotonLTEIntegrator_PointLightInSphere;

static class TestDescription_PhotonLTEIntegratorTestSuite_test_PhotonLTEIntegrator_InfinityLight : public CxxTest::RealTestDescription {
public:
 TestDescription_PhotonLTEIntegratorTestSuite_test_PhotonLTEIntegrator_InfinityLight() : CxxTest::RealTestDescription( Tests_PhotonLTEIntegratorTestSuite, suiteDescription_PhotonLTEIntegratorTestSuite, 110, "test_PhotonLTEIntegrator_InfinityLight" ) {}
 void runTest() { suite_PhotonLTEIntegratorTestSuite.test_PhotonLTEIntegrator_InfinityLight(); }
} testDescription_PhotonLTEIntegratorTestSuite_test_PhotonLTEIntegrator_InfinityLight;

