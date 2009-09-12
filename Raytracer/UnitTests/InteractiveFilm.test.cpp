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

bool InteractiveFilmTestSuite_init = false;
#include "./InteractiveFilm.test.h"

static InteractiveFilmTestSuite suite_InteractiveFilmTestSuite;

static CxxTest::List Tests_InteractiveFilmTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_InteractiveFilmTestSuite( "./InteractiveFilm.test.h", 13, "InteractiveFilmTestSuite", suite_InteractiveFilmTestSuite, Tests_InteractiveFilmTestSuite );

static class TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Extent : public CxxTest::RealTestDescription {
public:
 TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Extent() : CxxTest::RealTestDescription( Tests_InteractiveFilmTestSuite, suiteDescription_InteractiveFilmTestSuite, 27, "test_InteractiveFilm_Extent" ) {}
 void runTest() { suite_InteractiveFilmTestSuite.test_InteractiveFilm_Extent(); }
} testDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Extent;

static class TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_CropWindowExtent : public CxxTest::RealTestDescription {
public:
 TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_CropWindowExtent() : CxxTest::RealTestDescription( Tests_InteractiveFilmTestSuite, suiteDescription_InteractiveFilmTestSuite, 36, "test_InteractiveFilm_CropWindowExtent" ) {}
 void runTest() { suite_InteractiveFilmTestSuite.test_InteractiveFilm_CropWindowExtent(); }
} testDescription_InteractiveFilmTestSuite_test_InteractiveFilm_CropWindowExtent;

static class TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Pixel : public CxxTest::RealTestDescription {
public:
 TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Pixel() : CxxTest::RealTestDescription( Tests_InteractiveFilmTestSuite, suiteDescription_InteractiveFilmTestSuite, 46, "test_InteractiveFilm_Pixel" ) {}
 void runTest() { suite_InteractiveFilmTestSuite.test_InteractiveFilm_Pixel(); }
} testDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Pixel;

static class TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Clear : public CxxTest::RealTestDescription {
public:
 TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Clear() : CxxTest::RealTestDescription( Tests_InteractiveFilmTestSuite, suiteDescription_InteractiveFilmTestSuite, 81, "test_InteractiveFilm_Clear" ) {}
 void runTest() { suite_InteractiveFilmTestSuite.test_InteractiveFilm_Clear(); }
} testDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Clear;

static class TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_CropWindowPixels : public CxxTest::RealTestDescription {
public:
 TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_CropWindowPixels() : CxxTest::RealTestDescription( Tests_InteractiveFilmTestSuite, suiteDescription_InteractiveFilmTestSuite, 111, "test_InteractiveFilm_CropWindowPixels" ) {}
 void runTest() { suite_InteractiveFilmTestSuite.test_InteractiveFilm_CropWindowPixels(); }
} testDescription_InteractiveFilmTestSuite_test_InteractiveFilm_CropWindowPixels;

static class TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Approximation : public CxxTest::RealTestDescription {
public:
 TestDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Approximation() : CxxTest::RealTestDescription( Tests_InteractiveFilmTestSuite, suiteDescription_InteractiveFilmTestSuite, 143, "test_InteractiveFilm_Approximation" ) {}
 void runTest() { suite_InteractiveFilmTestSuite.test_InteractiveFilm_Approximation(); }
} testDescription_InteractiveFilmTestSuite_test_InteractiveFilm_Approximation;

