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

static class TestDescription_FilmTestSuite_testFilmDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_FilmTestSuite_testFilmDefaultConstr() : CxxTest::RealTestDescription( Tests_FilmTestSuite, suiteDescription_FilmTestSuite, 25, "testFilmDefaultConstr" ) {}
 void runTest() { suite_FilmTestSuite.testFilmDefaultConstr(); }
} testDescription_FilmTestSuite_testFilmDefaultConstr;

static class TestDescription_FilmTestSuite_testFilmExtent : public CxxTest::RealTestDescription {
public:
 TestDescription_FilmTestSuite_testFilmExtent() : CxxTest::RealTestDescription( Tests_FilmTestSuite, suiteDescription_FilmTestSuite, 33, "testFilmExtent" ) {}
 void runTest() { suite_FilmTestSuite.testFilmExtent(); }
} testDescription_FilmTestSuite_testFilmExtent;

static class TestDescription_FilmTestSuite_testFilmPixel : public CxxTest::RealTestDescription {
public:
 TestDescription_FilmTestSuite_testFilmPixel() : CxxTest::RealTestDescription( Tests_FilmTestSuite, suiteDescription_FilmTestSuite, 42, "testFilmPixel" ) {}
 void runTest() { suite_FilmTestSuite.testFilmPixel(); }
} testDescription_FilmTestSuite_testFilmPixel;

static class TestDescription_FilmTestSuite_testFilmClear : public CxxTest::RealTestDescription {
public:
 TestDescription_FilmTestSuite_testFilmClear() : CxxTest::RealTestDescription( Tests_FilmTestSuite, suiteDescription_FilmTestSuite, 77, "testFilmClear" ) {}
 void runTest() { suite_FilmTestSuite.testFilmClear(); }
} testDescription_FilmTestSuite_testFilmClear;

