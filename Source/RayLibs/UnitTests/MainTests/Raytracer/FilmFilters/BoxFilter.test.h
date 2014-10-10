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

#ifndef BOX_FILTER_TEST_H
#define BOX_FILTER_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
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
