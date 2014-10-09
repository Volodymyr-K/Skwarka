#ifndef IMAGE_TEXTURE_TEST_H
#define IMAGE_TEXTURE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Textures/ImageTexture.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Mappings/UVMapping2D.h>

class ImageTextureTestSuite : public CxxTest::TestSuite
  {
  public:

    // Test when the entire image is filtered.
    void test_ImageTexture_Evaluate1()
      {
      std::vector<std::vector<Spectrum_f>> image;
      _CreateRedBueCheckerboardImage(100,90,image);

      intrusive_ptr<Mapping2D> p_mapping( new UVMapping2D() );
      intrusive_ptr<Texture<Spectrum_d>> p_texture(new ImageTexture<Spectrum_f,Spectrum_d>(image, p_mapping) );
      
      DifferentialGeometry dg;
      dg.m_uv=Point2D_d(0.5,0.5);
      dg.m_duv_dx=Vector2D_d(0.5,0.0);
      dg.m_duv_dy=Vector2D_d(0.0,0.5);

      Spectrum_d spectrum = p_texture->Evaluate(dg, 0);
      CustomAssertDelta(spectrum, Spectrum_d(0.5,0.0,0.5), 1e-3);
      }

    // Test outside of the texture (in no-repeat mode).
    void test_ImageTexture_Evaluate2()
      {
      std::vector<std::vector<Spectrum_f>> image;
      _CreateRedBueCheckerboardImage(100,90,image);

      intrusive_ptr<Mapping2D> p_mapping( new UVMapping2D() );
      intrusive_ptr<Texture<Spectrum_d>> p_texture(new ImageTexture<Spectrum_f,Spectrum_d>(image, p_mapping, false) );

      DifferentialGeometry dg;
      dg.m_uv=Point2D_d(2.5,-2.5);
      dg.m_duv_dx=Vector2D_d(1.0,0.0);
      dg.m_duv_dy=Vector2D_d(0.0,1.0);

      Spectrum_d spectrum = p_texture->Evaluate(dg, 0);
      CustomAssertDelta(spectrum, Spectrum_d(0.0,0.0,0.0), 1e-5);
      }

    void test_ImageTexture_Evaluate3()
      {
      std::vector<std::vector<Spectrum_f>> image;
      _CreateRedBueCheckerboardImage(4,4,image);

      intrusive_ptr<Mapping2D> p_mapping( new UVMapping2D() );
      intrusive_ptr<Texture<Spectrum_d>> p_texture(new ImageTexture<Spectrum_f,Spectrum_d>(image, p_mapping) );

      DifferentialGeometry dg;
      dg.m_uv=Point2D_d(0.375,0.375);
      dg.m_duv_dx=Vector2D_d(0.0,0.0);
      dg.m_duv_dy=Vector2D_d(0.0,0.0);

      Spectrum_d spectrum = p_texture->Evaluate(dg, 0);
      CustomAssertDelta(spectrum, Spectrum_d(0.0,0.0,1.0), 1e-3);
      }

  private:
    void _CreateRedBueCheckerboardImage(size_t i_width, size_t i_height, std::vector<std::vector<Spectrum_f>> &o_image)
      {
      o_image.assign(i_height,std::vector<Spectrum_f>(i_width,Spectrum_f()));

      for(size_t i=0;i<i_width;++i)
        for(size_t j=0;j<i_height;++j)
          if ((i+j)%2)
            o_image[j][i]=Spectrum_f(1,0,0);
          else
            o_image[j][i]=Spectrum_f(0,0,1);
      }

  };

#endif // IMAGE_TEXTURE_TEST_H
