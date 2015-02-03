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

#ifndef NOISE_ROUTINES_TEST_H
#define NOISE_ROUTINES_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/ThreadSafeRandom.h>
#include <Math/NoiseRoutines.h>

class NoiseTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_PerlinNoise_Range()
      {
      double mn=DBL_INF, mx=-DBL_INF;
      for (size_t i=0; i<10000; ++i)
        {
        Point3D_d point(RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0);
        double val = NoiseRoutines::PerlinNoise(point);
        if (val>mx) mx=val;
        if (val<mn) mn=val;
        }

      ASSERT(mn>=-1.04 && mn<=1.04); // Perlin noise can slightly break the [-1;1] range
      ASSERT(mx>=-1.04 && mx<=1.04);
      }

    void test_FBm_Range()
      {
      double mn=DBL_INF, mx=-DBL_INF;
      for (size_t i=0; i<10000; ++i)
        {
        Point3D_d point(RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0);
        double val = NoiseRoutines::FBm(point, Vector3D_d(0.1, 0.1, 0.0), Vector3D_d(-0.1, 0.1, 0.0), 0.5, 10);
        if (val>mx) mx=val;
        if (val<mn) mn=val;
        }

      ASSERT(mn>=-2.0 && mn<=2.0); // theoretical limit for omega=0.5
      ASSERT(mx>=-2.0 && mx<=2.0);
      }

    void test_Turbulence_Range()
      {
      double mn=DBL_INF, mx=-DBL_INF;
      for (size_t i=0; i<10000; ++i)
        {
        Point3D_d point(RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0);
        double val = NoiseRoutines::Turbulence(point, Vector3D_d(0.1, 0.1, 0.0), Vector3D_d(-0.1, 0.1, 0.0), 0.5, 10);
        if (val>mx) mx=val;
        if (val<mn) mn=val;
        }

      ASSERT(mn>=0.0 && mn<=2.0); // theoretical limit for omega=0.5
      ASSERT(mx>=0.0 && mx<=2.0);
      }

  };

#endif // NOISE_ROUTINES_TEST_H