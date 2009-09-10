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

bool FilmFilterTestSuite_init = false;
#include "./FilmFilter.test.h"

static FilmFilterTestSuite suite_FilmFilterTestSuite;

static CxxTest::List Tests_FilmFilterTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_FilmFilterTestSuite( "./FilmFilter.test.h", 9, "FilmFilterTestSuite", suite_FilmFilterTestSuite, Tests_FilmFilterTestSuite );

static class TestDescription_FilmFilterTestSuite_test_FilmFilter_DefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_FilmFilterTestSuite_test_FilmFilter_DefaultConstr() : CxxTest::RealTestDescription( Tests_FilmFilterTestSuite, suiteDescription_FilmFilterTestSuite, 12, "test_FilmFilter_DefaultConstr" ) {}
 void runTest() { suite_FilmFilterTestSuite.test_FilmFilter_DefaultConstr(); }
} testDescription_FilmFilterTestSuite_test_FilmFilter_DefaultConstr;

