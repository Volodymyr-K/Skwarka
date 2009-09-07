/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_LONGLONG long long
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>

bool SpectrumTestSuite_init = false;
#include "./Spectrum.test.h"

static SpectrumTestSuite suite_SpectrumTestSuite;

static CxxTest::List Tests_SpectrumTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SpectrumTestSuite( "./Spectrum.test.h", 8, "SpectrumTestSuite", suite_SpectrumTestSuite, Tests_SpectrumTestSuite );

static class TestDescription_SpectrumTestSuite_testSpectrumDefaultConstr : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumDefaultConstr() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 11, "testSpectrumDefaultConstr" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumDefaultConstr(); }
} testDescription_SpectrumTestSuite_testSpectrumDefaultConstr;

static class TestDescription_SpectrumTestSuite_testSpectrumConstrWithCoords : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumConstrWithCoords() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 17, "testSpectrumConstrWithCoords" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumConstrWithCoords(); }
} testDescription_SpectrumTestSuite_testSpectrumConstrWithCoords;

static class TestDescription_SpectrumTestSuite_testSpectrumAdd : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumAdd() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 23, "testSpectrumAdd" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumAdd(); }
} testDescription_SpectrumTestSuite_testSpectrumAdd;

static class TestDescription_SpectrumTestSuite_testSpectrumAddAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumAddAssign() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 31, "testSpectrumAddAssign" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumAddAssign(); }
} testDescription_SpectrumTestSuite_testSpectrumAddAssign;

static class TestDescription_SpectrumTestSuite_testSpectrumSub : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumSub() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 40, "testSpectrumSub" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumSub(); }
} testDescription_SpectrumTestSuite_testSpectrumSub;

static class TestDescription_SpectrumTestSuite_testSpectrumSubAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumSubAssign() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 48, "testSpectrumSubAssign" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumSubAssign(); }
} testDescription_SpectrumTestSuite_testSpectrumSubAssign;

static class TestDescription_SpectrumTestSuite_testSpectrumMult : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumMult() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 57, "testSpectrumMult" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumMult(); }
} testDescription_SpectrumTestSuite_testSpectrumMult;

static class TestDescription_SpectrumTestSuite_testSpectrumPreMult : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumPreMult() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 64, "testSpectrumPreMult" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumPreMult(); }
} testDescription_SpectrumTestSuite_testSpectrumPreMult;

static class TestDescription_SpectrumTestSuite_testSpectrumMultAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumMultAssign() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 71, "testSpectrumMultAssign" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumMultAssign(); }
} testDescription_SpectrumTestSuite_testSpectrumMultAssign;

static class TestDescription_SpectrumTestSuite_testSpectrumDiv : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumDiv() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 79, "testSpectrumDiv" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumDiv(); }
} testDescription_SpectrumTestSuite_testSpectrumDiv;

static class TestDescription_SpectrumTestSuite_testSpectrumDivAssign : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumDivAssign() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 86, "testSpectrumDivAssign" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumDivAssign(); }
} testDescription_SpectrumTestSuite_testSpectrumDivAssign;

static class TestDescription_SpectrumTestSuite_testSpectrumDivByZero : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumDivByZero() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 97, "testSpectrumDivByZero" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumDivByZero(); }
} testDescription_SpectrumTestSuite_testSpectrumDivByZero;

static class TestDescription_SpectrumTestSuite_testSpectrumEqual : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumEqual() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 105, "testSpectrumEqual" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumEqual(); }
} testDescription_SpectrumTestSuite_testSpectrumEqual;

static class TestDescription_SpectrumTestSuite_testSpectrumNotEqual : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumNotEqual() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 113, "testSpectrumNotEqual" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumNotEqual(); }
} testDescription_SpectrumTestSuite_testSpectrumNotEqual;

static class TestDescription_SpectrumTestSuite_testSpectrumIndexOperator : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumIndexOperator() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 121, "testSpectrumIndexOperator" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumIndexOperator(); }
} testDescription_SpectrumTestSuite_testSpectrumIndexOperator;

static class TestDescription_SpectrumTestSuite_testSpectrumAddWeighted : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumAddWeighted() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 127, "testSpectrumAddWeighted" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumAddWeighted(); }
} testDescription_SpectrumTestSuite_testSpectrumAddWeighted;

static class TestDescription_SpectrumTestSuite_testSpectrumIsBlack : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumIsBlack() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 135, "testSpectrumIsBlack" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumIsBlack(); }
} testDescription_SpectrumTestSuite_testSpectrumIsBlack;

static class TestDescription_SpectrumTestSuite_testSpectrumXYZ : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumXYZ() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 143, "testSpectrumXYZ" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumXYZ(); }
} testDescription_SpectrumTestSuite_testSpectrumXYZ;

static class TestDescription_SpectrumTestSuite_testSpectrumLuminance : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumLuminance() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 151, "testSpectrumLuminance" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumLuminance(); }
} testDescription_SpectrumTestSuite_testSpectrumLuminance;

static class TestDescription_SpectrumTestSuite_testSpectrumOutputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumOutputStream() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 161, "testSpectrumOutputStream" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumOutputStream(); }
} testDescription_SpectrumTestSuite_testSpectrumOutputStream;

static class TestDescription_SpectrumTestSuite_testSpectrumInputStream : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumInputStream() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 170, "testSpectrumInputStream" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumInputStream(); }
} testDescription_SpectrumTestSuite_testSpectrumInputStream;

static class TestDescription_SpectrumTestSuite_testSpectrumConversion : public CxxTest::RealTestDescription {
public:
 TestDescription_SpectrumTestSuite_testSpectrumConversion() : CxxTest::RealTestDescription( Tests_SpectrumTestSuite, suiteDescription_SpectrumTestSuite, 179, "testSpectrumConversion" ) {}
 void runTest() { suite_SpectrumTestSuite.testSpectrumConversion(); }
} testDescription_SpectrumTestSuite_testSpectrumConversion;

