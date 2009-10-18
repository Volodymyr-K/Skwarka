#ifndef MAIN_TEST_H
#define MAIN_TEST_H

#include <cxxtest/TestSuite.h>

// This is a dummy test suite needed only to generate main() function 
// to run the other test suites.
class DummySuite : public CxxTest::TestSuite
  {
  public:
    void testDummy( void )
      {
      TS_ASSERT(true);
      }
  };

#endif // MAIN_TEST_H