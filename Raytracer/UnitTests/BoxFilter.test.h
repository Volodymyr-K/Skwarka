#ifndef BOX_FILTER_TEST_H
#define BOX_FILTER_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/FilmFilter.h>
#include <Raytracer/FilmFilters/BoxFilter.h>

class BoxFilterTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_BoxFilter_EvaluateInside1()
      {
      BoxFilter filter(1.0,2.0);

      TS_ASSERT_EQUALS(filter.Evaluate(0.0,0.0), 1.0);
      }

    void test_BoxFilter_EvaluateInside2()
      {
      BoxFilter filter(1.0,2.0);

      TS_ASSERT_EQUALS(filter.Evaluate(-1.0,2.0), 1.0);
      }

    void test_BoxFilter_EvaluateOutside()
      {
      BoxFilter filter(1.0,2.0);

      TS_ASSERT_EQUALS(filter.Evaluate(-1.0,2.01), 0.0);
      }
  };

#endif // BOX_FILTER_TEST_H
