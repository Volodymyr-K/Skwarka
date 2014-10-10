/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FILM_FILTER_TEST_H
#define FILM_FILTER_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/FilmFilter.h>
#include <UnitTests/Mocks/FilmFilterMock.h>

class FilmFilterTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_FilmFilter_DefaultConstr()
      {
      FilmFilterMock filter(100.5,50.5);

      TS_ASSERT_EQUALS(filter.GetXWidth(),100.5);
      TS_ASSERT_EQUALS(filter.GetYWidth(),50.5);
      }
  };

#endif // FILM_FILTER_TEST_H
