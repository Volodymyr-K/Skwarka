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

#ifndef WRINKLED_TEXTURE_TEST_H
#define WRINKLED_TEXTURE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Textures/WrinkledTexture.h>
#include <Raytracer/Mappings/TransformMapping3D.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Core/DifferentialGeometry.h>

class WrinkledTextureTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_WrinkledTexture_Range()
      {
      intrusive_ptr<const Mapping3D> p_mapping(new TransformMapping3D());
      intrusive_ptr<Texture<double>> p_texture(new WrinkledTexture<double>(6, 0.5, p_mapping));

      double mn=DBL_INF, mx=-DBL_INF;
      for (size_t i=0; i<10000; ++i)
        {
        DifferentialGeometry dg;
        dg.m_point = Point3D_d(RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0, RandomDouble(100.0)-50.0);
        double val = p_texture->Evaluate(dg, 0);
        if (val>mx) mx=val;
        if (val<mn) mn=val;
        }

      TS_ASSERT(mx>mn);
      TS_ASSERT(mn>=0.0 && mn<=1.2); // empirical value
      TS_ASSERT(mx>=0.0 && mx<=1.2); // empirical value
      }
  };

#endif // WRINKLED_TEXTURE_TEST_H
