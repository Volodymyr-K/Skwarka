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
#include <cxxtest/ParenPrinter.h>

int main( int argc, char *argv[] ) {
    CxxTest::ParenPrinter tmp;
    return CxxTest::Main<CxxTest::ParenPrinter>( tmp, argc, argv );
}
bool DummySuite_init = false;
#include "./Main.h"

static DummySuite suite_DummySuite;

static CxxTest::List Tests_DummySuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_DummySuite( "./Main.h", 5, "DummySuite", suite_DummySuite, Tests_DummySuite );

static class TestDescription_DummySuite_testDummy : public CxxTest::RealTestDescription {
public:
 TestDescription_DummySuite_testDummy() : CxxTest::RealTestDescription( Tests_DummySuite, suiteDescription_DummySuite, 8, "testDummy" ) {}
 void runTest() { suite_DummySuite.testDummy(); }
} testDescription_DummySuite_testDummy;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
