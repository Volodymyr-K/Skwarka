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

#ifndef SCALE_TEXTURE_TEST_H
#define SCALE_TEXTURE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Textures/ScaleTexture.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/SpectrumCoef.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include "Mocks/TextureMock.h"

class ScaleTextureTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_ScaleTexture_Evaluate()
      {
      SpectrumCoef_d s1(1.0,2.0,3.0);
      Spectrum_d s2(0.5,2.5,3.2);
      intrusive_ptr<Texture<SpectrumCoef_d>> p_texture1(new TextureMock<SpectrumCoef_d>(s1));
      intrusive_ptr<Texture<Spectrum_d>> p_texture2(new TextureMock<Spectrum_d>(s2) );
      intrusive_ptr<Texture<Spectrum_d>> p_texture(new ScaleTexture<SpectrumCoef_d, Spectrum_d>(p_texture1, p_texture2) );

      DifferentialGeometry dg; // No need to fill with any data for the constant texture.
      Spectrum_d spectrum = p_texture->Evaluate(dg, 0);
      TS_ASSERT_EQUALS(spectrum, s1*s2);
      }
  };

#endif // SCALE_TEXTURE_TEST_H
