/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ParenPrinter.h>

int main() {
 return CxxTest::ParenPrinter().run();
}
#include "./Main.test.h"

static DummySuite suite_DummySuite;

static CxxTest::List Tests_DummySuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_DummySuite( "./Main.test.h", 8, "DummySuite", suite_DummySuite, Tests_DummySuite );

static class TestDescription_DummySuite_testDummy : public CxxTest::RealTestDescription {
public:
 TestDescription_DummySuite_testDummy() : CxxTest::RealTestDescription( Tests_DummySuite, suiteDescription_DummySuite, 11, "testDummy" ) {}
 void runTest() { suite_DummySuite.testDummy(); }
} testDescription_DummySuite_testDummy;

#include <cxxtest/Root.cpp>
