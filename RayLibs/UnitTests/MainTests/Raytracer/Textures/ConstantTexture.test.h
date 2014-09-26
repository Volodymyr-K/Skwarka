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
