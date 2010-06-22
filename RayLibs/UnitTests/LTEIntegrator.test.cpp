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
CxxTest::StaticSuiteDescription suiteDescription_LTEIntegratorTestSuite( "./LTEIntegrator.test.h", 15, "LTEIntegratorTestSuite", suite_LTEIntegratorTestSuite, Tests_LTEIntegratorTestSuite );

static class TestDescription_LTEIntegratorTestSuite_test_LTEIntegratorTestSuite_RadianceInsideSphere_NoMedia : public CxxTest::RealTestDescription {
public:
 TestDescription_LTEIntegratorTestSuite_test_LTEIntegratorTestSuite_RadianceInsideSphere_NoMedia() : CxxTest::RealTestDescription( Tests_LTEIntegratorTestSuite, suiteDescription_LTEIntegratorTestSuite, 53, "test_LTEIntegratorTestSuite_RadianceInsideSphere_NoMedia" ) {}
 void runTest() { suite_LTEIntegratorTestSuite.test_LTEIntegratorTestSuite_RadianceInsideSphere_NoMedia(); }
} testDescription_LTEIntegratorTestSuite_test_LTEIntegratorTestSuite_RadianceInsideSphere_NoMedia;

static class TestDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceOutsideSphere_NoMedia : public CxxTest::RealTestDescription {
public:
 TestDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceOutsideSphere_NoMedia() : CxxTest::RealTestDescription( Tests_LTEIntegratorTestSuite, suiteDescription_LTEIntegratorTestSuite, 64, "test_LTEIntegrator_RadianceOutsideSphere_NoMedia" ) {}
 void runTest() { suite_LTEIntegratorTestSuite.test_LTEIntegrator_RadianceOutsideSphere_NoMedia(); }
} testDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceOutsideSphere_NoMedia;

static class TestDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceInsideSphere_WithMedia : public CxxTest::RealTestDescription {
public:
 TestDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceInsideSphere_WithMedia() : CxxTest::RealTestDescription( Tests_LTEIntegratorTestSuite, suiteDescription_LTEIntegratorTestSuite, 75, "test_LTEIntegrator_RadianceInsideSphere_WithMedia" ) {}
 void runTest() { suite_LTEIntegratorTestSuite.test_LTEIntegrator_RadianceInsideSphere_WithMedia(); }
} testDescription_LTEIntegratorTestSuite_test_LTEIntegrator_RadianceInsideSphere_WithMedia;

