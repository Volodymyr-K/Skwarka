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

#ifndef MITCHELL_FILTER_TEST_H
#define MITCHELL_FILTER_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/FilmFilter.h>
#include <Raytracer/FilmFilters/MitchellFilter.h>

class MitchellFilterTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_MitchellFilter_DefaultParameters1()
      {
      MitchellFilter filter(1.0,2.0);

      double b, c;
      filter.GetParameters(b,c);
      TS_ASSERT_DELTA(b, 1.0/3.0, (1e-10));
      TS_ASSERT_DELTA(c, 1.0/3.0, (1e-10));
      }

    void test_MitchellFilter_DefaultParameters2()
      {
      MitchellFilter filter(1.0,2.0, 0.1);

      double b, c;
      filter.GetParameters(b,c);
      TS_ASSERT_DELTA(b+2.0*c, 1.0, (1e-10));
      }

    void test_MitchellFilter_EvaluateInside()
      {
      MitchellFilter filter(1.0,2.0);

      TS_ASSERT(filter.Evaluate(0.0,0.0) > 0.0);
      }

    void test_MitchellFilter_EvaluateOutside()
      {
      MitchellFilter filter(1.0,2.0);

      TS_ASSERT(filter.Evaluate(0.0,1e+10) < (1e-5));
      }
  };

#endif // MITCHELL_FILTER_TEST_H
