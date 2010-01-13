/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>

#include "./MemoryPool.test.h"

static MemoryPoolTestSuite suite_MemoryPoolTestSuite;

static CxxTest::List Tests_MemoryPoolTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_MemoryPoolTestSuite( "./MemoryPool.test.h", 9, "MemoryPoolTestSuite", suite_MemoryPoolTestSuite, Tests_MemoryPoolTestSuite );

static class TestDescription_MemoryPoolTestSuite_test_MemoryPool_WriteAndRead : public CxxTest::RealTestDescription {
public:
 TestDescription_MemoryPoolTestSuite_test_MemoryPool_WriteAndRead() : CxxTest::RealTestDescription( Tests_MemoryPoolTestSuite, suiteDescription_MemoryPoolTestSuite, 14, "test_MemoryPool_WriteAndRead" ) {}
 void runTest() { suite_MemoryPoolTestSuite.test_MemoryPool_WriteAndRead(); }
} testDescription_MemoryPoolTestSuite_test_MemoryPool_WriteAndRead;

static class TestDescription_MemoryPoolTestSuite_test_MemoryPool_BigChunk : public CxxTest::RealTestDescription {
public:
 TestDescription_MemoryPoolTestSuite_test_MemoryPool_BigChunk() : CxxTest::RealTestDescription( Tests_MemoryPoolTestSuite, suiteDescription_MemoryPoolTestSuite, 43, "test_MemoryPool_BigChunk" ) {}
 void runTest() { suite_MemoryPoolTestSuite.test_MemoryPool_BigChunk(); }
} testDescription_MemoryPoolTestSuite_test_MemoryPool_BigChunk;

static class TestDescription_MemoryPoolTestSuite_test_MemoryPool_FreeAll : public CxxTest::RealTestDescription {
public:
 TestDescription_MemoryPoolTestSuite_test_MemoryPool_FreeAll() : CxxTest::RealTestDescription( Tests_MemoryPoolTestSuite, suiteDescription_MemoryPoolTestSuite, 61, "test_MemoryPool_FreeAll" ) {}
 void runTest() { suite_MemoryPoolTestSuite.test_MemoryPool_FreeAll(); }
} testDescription_MemoryPoolTestSuite_test_MemoryPool_FreeAll;

static class TestDescription_MemoryPoolTestSuite_test_MemoryPool_Release : public CxxTest::RealTestDescription {
public:
 TestDescription_MemoryPoolTestSuite_test_MemoryPool_Release() : CxxTest::RealTestDescription( Tests_MemoryPoolTestSuite, suiteDescription_MemoryPoolTestSuite, 99, "test_MemoryPool_Release" ) {}
 void runTest() { suite_MemoryPoolTestSuite.test_MemoryPool_Release(); }
} testDescription_MemoryPoolTestSuite_test_MemoryPool_Release;

static class TestDescription_MemoryPoolTestSuite_test_MemoryPoolAllocator : public CxxTest::RealTestDescription {
public:
 TestDescription_MemoryPoolTestSuite_test_MemoryPoolAllocator() : CxxTest::RealTestDescription( Tests_MemoryPoolTestSuite, suiteDescription_MemoryPoolTestSuite, 116, "test_MemoryPoolAllocator" ) {}
 void runTest() { suite_MemoryPoolTestSuite.test_MemoryPoolAllocator(); }
} testDescription_MemoryPoolTestSuite_test_MemoryPoolAllocator;

