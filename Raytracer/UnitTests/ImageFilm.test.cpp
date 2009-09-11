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

bool ImageFilmTestSuite_init = false;
#include "./ImageFilm.test.h"

static ImageFilmTestSuite suite_ImageFilmTestSuite;

static CxxTest::List Tests_ImageFilmTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ImageFilmTestSuite( "./ImageFilm.test.h", 12, "ImageFilmTestSuite", suite_ImageFilmTestSuite, Tests_ImageFilmTestSuite );

static class TestDescription_ImageFilmTestSuite_test_ImageFilm_DefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_ImageFilmTestSuite_test_ImageFilm_DefaultConstr() : CxxTest::RealTestDescription( Tests_ImageFilmTestSuite, suiteDescription_ImageFilmTestSuite, 26, "test_ImageFilm_DefaultConstr" ) {}
 void runTest() { suite_ImageFilmTestSuite.test_ImageFilm_DefaultConstr(); }
} testDescription_ImageFilmTestSuite_test_ImageFilm_DefaultConstr;

static class TestDescription_ImageFilmTestSuite_test_ImageFilm_Extent : public CxxTest::RealTestDescription {
public:
 TestDescription_ImageFilmTestSuite_test_ImageFilm_Extent() : CxxTest::RealTestDescription( Tests_ImageFilmTestSuite, suiteDescription_ImageFilmTestSuite, 34, "test_ImageFilm_Extent" ) {}
 void runTest() { suite_ImageFilmTestSuite.test_ImageFilm_Extent(); }
} testDescription_ImageFilmTestSuite_test_ImageFilm_Extent;

static class TestDescription_ImageFilmTestSuite_test_ImageFilm_Pixel : public CxxTest::RealTestDescription {
public:
 TestDescription_ImageFilmTestSuite_test_ImageFilm_Pixel() : CxxTest::RealTestDescription( Tests_ImageFilmTestSuite, suiteDescription_ImageFilmTestSuite, 43, "test_ImageFilm_Pixel" ) {}
 void runTest() { suite_ImageFilmTestSuite.test_ImageFilm_Pixel(); }
} testDescription_ImageFilmTestSuite_test_ImageFilm_Pixel;

static class TestDescription_ImageFilmTestSuite_test_ImageFilm_Clear : public CxxTest::RealTestDescription {
public:
 TestDescription_ImageFilmTestSuite_test_ImageFilm_Clear() : CxxTest::RealTestDescription( Tests_ImageFilmTestSuite, suiteDescription_ImageFilmTestSuite, 78, "test_ImageFilm_Clear" ) {}
 void runTest() { suite_ImageFilmTestSuite.test_ImageFilm_Clear(); }
} testDescription_ImageFilmTestSuite_test_ImageFilm_Clear;

