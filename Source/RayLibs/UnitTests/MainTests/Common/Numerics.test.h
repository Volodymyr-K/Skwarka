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

#ifndef NUMERICS_TEST_H
#define NUMERICS_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Numerics.h>
#include <Math/Constants.h>
#include <limits>

// Disable compiler warning since we divide by zero intentionally.
#pragma warning( push )
#pragma warning( disable : 4723 )

class NumericsTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_IsNaN1()
      {
      double x=0.1;
      TS_ASSERT(IsNaN(x)==false);
      }

    void test_IsNaN2()
      {
      double x=DBL_INF;
      TS_ASSERT(IsNaN(x)==false);
      }

    void test_IsNaN3()
      {
      double x=std::numeric_limits<double>::quiet_NaN();
      TS_ASSERT(IsNaN(x));
      }

    void test_IsNaN4()
      {
      double x=0.0;
      x/=0.0;
      TS_ASSERT(IsNaN(x));
      }

    void test_IsPositiveInf1()
      {
      double x=0.1;
      TS_ASSERT(IsPositiveInf(x)==false);
      }

    void test_IsPositiveInf2()
      {
      double x=std::numeric_limits<double>::quiet_NaN();
      TS_ASSERT(IsPositiveInf(x)==false);
      }

    void test_IsPositiveInf3()
      {
      double x=-0.2;
      x/=0.0;
      TS_ASSERT(IsPositiveInf(x)==false);
      }

    void test_IsPositiveInf4()
      {
      double x=0.2;
      x/=0.0;
      TS_ASSERT(IsPositiveInf(x));
      }


    void test_IsNegativeInf1()
      {
      double x=0.1;
      TS_ASSERT(IsNegativeInf(x)==false);
      }

    void test_IsNegativeInf2()
      {
      double x=std::numeric_limits<double>::quiet_NaN();
      TS_ASSERT(IsNegativeInf(x)==false);
      }

    void test_IsNegativeInf3()
      {
      double x=0.2;
      x/=0.0;
      TS_ASSERT(IsNegativeInf(x)==false);
      }

    void test_IsNegativeInf4()
      {
      double x=-0.2;
      x/=0.0;
      TS_ASSERT(IsNegativeInf(x));
      }


    void test_IsInf1()
      {
      double x=0.1;
      TS_ASSERT(IsInf(x)==false);
      }

    void test_IsInf2()
      {
      double x=std::numeric_limits<double>::quiet_NaN();
      TS_ASSERT(IsInf(x)==false);
      }

    void test_IsInf3()
      {
      double x=0.2;
      x/=0.0;
      TS_ASSERT(IsInf(x));
      }

    void test_IsInf4()
      {
      double x=-0.2;
      x/=0.0;
      TS_ASSERT(IsInf(x));
      }

  };

#pragma warning( pop )

#endif // NUMERICS_TEST_H