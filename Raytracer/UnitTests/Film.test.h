#ifndef FILM_TEST_H
#define FILM_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/Film.h>
#include "Mocks/FilmMock.h"

class FilmTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Film_Constr()
      {
      FilmMock film(100,50);

      TS_ASSERT(film.GetXResolution()==100 && film.GetYResolution()==50);
      }
  };

#endif // FILM_TEST_H
