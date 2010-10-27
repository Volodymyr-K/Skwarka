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
      intrusive_ptr<Texture<SpectrumCoef_d> > p_texture1(new TextureMock<SpectrumCoef_d>(s1));
      intrusive_ptr<Texture<Spectrum_d> > p_texture2(new TextureMock<Spectrum_d>(s2) );
      intrusive_ptr<Texture<Spectrum_d> > p_texture(new ScaleTexture<SpectrumCoef_d, Spectrum_d>(p_texture1, p_texture2) );

      DifferentialGeometry dg; // No need to fill with any data for the constant texture.
      Spectrum_d spectrum = p_texture->Evaluate(dg, 0);
      TS_ASSERT_EQUALS(spectrum, s1*s2);
      }
  };

#endif // SCALE_TEXTURE_TEST_H
