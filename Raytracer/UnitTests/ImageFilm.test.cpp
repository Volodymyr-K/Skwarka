/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./ImageFilm.test.h"

static ImageFilmTestSuite suite_ImageFilmTestSuite;

static CxxTest::List Tests_ImageFilmTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ImageFilmTestSuite( "./ImageFilm.test.h", 13, "ImageFilmTestSuite", suite_ImageFilmTestSuite, Tests_ImageFilmTestSuite );

static class TestDescription_ImageFilmTestSuite_test_ImageFilm_Extent : public CxxTest::RealTestDescription {
public:
 TestDescription_ImageFilmTestSuite_test_ImageFilm_Extent() : CxxTest::RealTestDescription( Tests_ImageFilmTestSuite, suiteDescription_ImageFilmTestSuite, 27, "test_ImageFilm_Extent" ) {}
 void runTest() { suite_ImageFilmTestSuite.test_ImageFilm_Extent(); }
} testDescription_ImageFilmTestSuite_test_ImageFilm_Extent;

static class TestDescription_ImageFilmTestSuite_test_ImageFilm_CropWindowExtent : public CxxTest::RealTestDescription {
public:
 TestDescription_ImageFilmTestSuite_test_ImageFilm_CropWindowExtent() : CxxTest::RealTestDescription( Tests_ImageFilmTestSuite, suiteDescription_ImageFilmTestSuite, 36, "test_ImageFilm_CropWindowExtent" ) {}
 void runTest() { suite_ImageFilmTestSuite.test_ImageFilm_CropWindowExtent(); }
} testDescription_ImageFilmTestSuite_test_ImageFilm_CropWindowExtent;

static class TestDescription_ImageFilmTestSuite_test_ImageFilm_Pixel : public CxxTest::RealTestDescription {
public:
 TestDescription_ImageFilmTestSuite_test_ImageFilm_Pixel() : CxxTest::RealTestDescription( Tests_ImageFilmTestSuite, suiteDescription_ImageFilmTestSuite, 46, "test_ImageFilm_Pixel" ) {}
 void runTest() { suite_ImageFilmTestSuite.test_ImageFilm_Pixel(); }
} testDescription_ImageFilmTestSuite_test_ImageFilm_Pixel;

static class TestDescription_ImageFilmTestSuite_test_ImageFilm_Clear : public CxxTest::RealTestDescription {
public:
 TestDescription_ImageFilmTestSuite_test_ImageFilm_Clear() : CxxTest::RealTestDescription( Tests_ImageFilmTestSuite, suiteDescription_ImageFilmTestSuite, 81, "test_ImageFilm_Clear" ) {}
 void runTest() { suite_ImageFilmTestSuite.test_ImageFilm_Clear(); }
} testDescription_ImageFilmTestSuite_test_ImageFilm_Clear;

static class TestDescription_ImageFilmTestSuite_test_ImageFilm_CropWindowPixels : public CxxTest::RealTestDescription {
public:
 TestDescription_ImageFilmTestSuite_test_ImageFilm_CropWindowPixels() : CxxTest::RealTestDescription( Tests_ImageFilmTestSuite, suiteDescription_ImageFilmTestSuite, 111, "test_ImageFilm_CropWindowPixels" ) {}
 void runTest() { suite_ImageFilmTestSuite.test_ImageFilm_CropWindowPixels(); }
} testDescription_ImageFilmTestSuite_test_ImageFilm_CropWindowPixels;

