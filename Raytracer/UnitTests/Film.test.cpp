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
CxxTest::StaticSuiteDescription suiteDescription_FilmTestSuite( "./Film.test.h", 11, "FilmTestSuite", suite_FilmTestSuite, Tests_FilmTestSuite );

static class TestDescription_FilmTestSuite_test_Film_DefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_FilmTestSuite_test_Film_DefaultConstr() : CxxTest::RealTestDescription( Tests_FilmTestSuite, suiteDescription_FilmTestSuite, 25, "test_Film_DefaultConstr" ) {}
 void runTest() { suite_FilmTestSuite.test_Film_DefaultConstr(); }
} testDescription_FilmTestSuite_test_Film_DefaultConstr;

static class TestDescription_FilmTestSuite_test_Film_Extent : public CxxTest::RealTestDescription {
public:
 TestDescription_FilmTestSuite_test_Film_Extent() : CxxTest::RealTestDescription( Tests_FilmTestSuite, suiteDescription_FilmTestSuite, 33, "test_Film_Extent" ) {}
 void runTest() { suite_FilmTestSuite.test_Film_Extent(); }
} testDescription_FilmTestSuite_test_Film_Extent;

static class TestDescription_FilmTestSuite_test_Film_Pixel : public CxxTest::RealTestDescription {
public:
 TestDescription_FilmTestSuite_test_Film_Pixel() : CxxTest::RealTestDescription( Tests_FilmTestSuite, suiteDescription_FilmTestSuite, 42, "test_Film_Pixel" ) {}
 void runTest() { suite_FilmTestSuite.test_Film_Pixel(); }
} testDescription_FilmTestSuite_test_Film_Pixel;

static class TestDescription_FilmTestSuite_test_Film_Clear : public CxxTest::RealTestDescription {
public:
 TestDescription_FilmTestSuite_test_Film_Clear() : CxxTest::RealTestDescription( Tests_FilmTestSuite, suiteDescription_FilmTestSuite, 77, "test_Film_Clear" ) {}
 void runTest() { suite_FilmTestSuite.test_Film_Clear(); }
} testDescription_FilmTestSuite_test_Film_Clear;

