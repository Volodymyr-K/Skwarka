/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./LTEIntegrator.test.h"

static LTEIntegratorTestSuite suite_LTEIntegratorTestSuite;

static CxxTest::List Tests_LTEIntegratorTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_LTEIntegratorTestSuite( "./LTEIntegrator.test.h", 14, "LTEIntegratorTestSuite", suite_LTEIntegratorTestSuite, Tests_LTEIntegratorTestSuite );

static class TestDescription_LTEIntegratorTestSuite_test_LTEIntegratorTestSuite_RadianceInsideSphere_NoVolumeIntegrator : public CxxTest::RealTestDescription {
public:
 TestDescription_LTEIntegratorTestSuite_test_LTEIntegratorTestSuite_RadianceInsideSphere_NoVolumeIntegrator() : CxxTest::RealTestDescription( Tests_LTEIntegratorTestSuite, suiteDescription_LTEIntegratorTestSuite, 46, "test_LTEIntegratorTestSuite_RadianceInsideSphere_NoVolumeIntegrator" ) {}
 void runTest() { suite_LTEIntegratorTestSuite.test_LTEIntegratorTestSuite_RadianceInsideSphere_NoVolumeIntegrator(); }
} testDescription_LTEIntegratorTestSuite_test_LTEIntegratorTestSuite_RadianceInsideSphere_NoVolumeIntegrator;

static class TestDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceOutsideSphere_NoVolumeIntegrator : public CxxTest::RealTestDescription {
public:
 TestDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceOutsideSphere_NoVolumeIntegrator() : CxxTest::RealTestDescription( Tests_LTEIntegratorTestSuite, suiteDescription_LTEIntegratorTestSuite, 58, "test_LTEIntegrator_RadianceOutsideSphere_NoVolumeIntegrator" ) {}
 void runTest() { suite_LTEIntegratorTestSuite.test_LTEIntegrator_RadianceOutsideSphere_NoVolumeIntegrator(); }
} testDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceOutsideSphere_NoVolumeIntegrator;

static class TestDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceOutsideSphere_WithVolumeIntegrator : public CxxTest::RealTestDescription {
public:
 TestDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceOutsideSphere_WithVolumeIntegrator() : CxxTest::RealTestDescription( Tests_LTEIntegratorTestSuite, suiteDescription_LTEIntegratorTestSuite, 71, "test_LTEIntegrator_RadianceOutsideSphere_WithVolumeIntegrator" ) {}
 void runTest() { suite_LTEIntegratorTestSuite.test_LTEIntegrator_RadianceOutsideSphere_WithVolumeIntegrator(); }
} testDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceOutsideSphere_WithVolumeIntegrator;

static class TestDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceInsideSphere_WithVolumeIntegrators : public CxxTest::RealTestDescription {
public:
 TestDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceInsideSphere_WithVolumeIntegrators() : CxxTest::RealTestDescription( Tests_LTEIntegratorTestSuite, suiteDescription_LTEIntegratorTestSuite, 82, "test_LTEIntegrator_RadianceInsideSphere_WithVolumeIntegrators" ) {}
 void runTest() { suite_LTEIntegratorTestSuite.test_LTEIntegrator_RadianceInsideSphere_WithVolumeIntegrators(); }
} testDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceInsideSphere_WithVolumeIntegrators;

