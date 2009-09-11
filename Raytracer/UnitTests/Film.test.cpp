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

bool FilmTestSuite_init = false;
#include "./Film.test.h"

static FilmTestSuite suite_FilmTestSuite;

static CxxTest::List Tests_FilmTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_FilmTestSuite( "./Film.test.h", 9, "FilmTestSuite", suite_FilmTestSuite, Tests_FilmTestSuite );

static class TestDescription_FilmTestSuite_test_Film_Constr : public CxxTest::RealTestDescription {
public:
 TestDescription_FilmTestSuite_test_Film_Constr() : CxxTest::RealTestDescription( Tests_FilmTestSuite, suiteDescription_FilmTestSuite, 12, "test_Film_Constr" ) {}
 void runTest() { suite_FilmTestSuite.test_Film_Constr(); }
} testDescription_FilmTestSuite_test_Film_Constr;

