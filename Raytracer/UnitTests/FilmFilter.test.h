#ifndef FILM_FILTER_TEST_H
#define FILM_FILTER_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/FilmFilter.h>
#include <UnitTests/Mocks/FilmFilterMock.h>

class FilmFilterTestSuite : public CxxTest::TestSuite
  {
  public:
    void testFilmDefaultConstr()
      {
      FilmFilterMock filter(100.5,50.5);

      TS_ASSERT_EQUALS(filter.GetXWidth(),100.5);
      TS_ASSERT_EQUALS(filter.GetYWidth(),50.5);
      }
  };

#endif // FILM_FILTER_TEST_H
