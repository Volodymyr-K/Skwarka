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

#ifndef CONSTANT_TEXTURE_TEST_H
#define CONSTANT_TEXTURE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Core/DifferentialGeometry.h>

class ConstantTextureTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_ConstantTexture_Evaluate()
      {
      intrusive_ptr<Texture<Spectrum_d>> p_texture(new ConstantTexture<Spectrum_d>(Spectrum_d(1.0,2.0,3.0)) );

      DifferentialGeometry dg; // No need to fill with any data for the constant texture.
      Spectrum_d spectrum = p_texture->Evaluate(dg, 0);
      TS_ASSERT_EQUALS(spectrum, Spectrum_d(1.0,2.0,3.0));
      }
  };

#endif // CONSTANT_TEXTURE_TEST_H
