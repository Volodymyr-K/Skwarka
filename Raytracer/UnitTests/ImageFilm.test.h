#ifndef IMAGE_FILM_TEST_H
#define IMAGE_FILM_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/Film.h>
#include <Raytracer/Core/Spectrum.h>
#include <UnitTests/Mocks/FilmFilterMock.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Films/ImageFilm.h>
#include <Math/Point2D.h>

class ImageFilmTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      mp_filter = shared_ptr<FilmFilter>(new FilmFilterMock(4.0,2.0));
      mp_film = shared_ptr<ImageFilm>(new ImageFilm(100,50,mp_filter));
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_ImageFilm_Extent()
      {
      Point2D_i begin, end;
      mp_film->GetSamplingExtent(begin, end);

      TS_ASSERT_EQUALS(begin, Convert<int>( Point2D_d(-mp_filter->GetXWidth(),-mp_filter->GetYWidth()) ) );
      TS_ASSERT_EQUALS(end, Convert<int>( Point2D_d(100+mp_filter->GetXWidth(),50+mp_filter->GetYWidth()) ) );
      }

    void test_ImageFilm_CropWindowExtent()
      {
      Point2D_i begin, end;
      mp_film->SetCropWindow(Point2D_i(20,10),Point2D_i(80,40));
      mp_film->GetSamplingExtent(begin, end);

      TS_ASSERT_EQUALS(begin, Convert<int>( Point2D_d(20-mp_filter->GetXWidth(),10-mp_filter->GetYWidth()) ) );
      TS_ASSERT_EQUALS(end, Convert<int>( Point2D_d(80+mp_filter->GetXWidth(),40+mp_filter->GetYWidth()) ) );
      }

    void test_ImageFilm_Pixel()
      {
      Point2D_i test_point(15,15);
      Spectrum_f spectrum_acc;
      float alpha_acc=0.f;
      float weight_acc=0.f;

      for(size_t y=10;y<=20;++y)
        for(size_t x=10;x<=20;++x)
          {
          Point2D_d image_point=Point2D_d(y+RandomDouble(1.0),x+RandomDouble(1.0));
          Spectrum_f sp((float)RandomDouble(1.0),(float)RandomDouble(1.0),(float)RandomDouble(1.0));
          float alpha = (float)RandomDouble(1.0);
          mp_film->AddSample(image_point,sp,alpha);

          float weight = (float) mp_filter->Evaluate(0.5+test_point[0]-image_point[0], 0.5+test_point[1]-image_point[1]);
          spectrum_acc+=sp*weight;
          alpha_acc+=alpha*weight;
          weight_acc+=weight;
          }

      spectrum_acc/=weight_acc;
      alpha_acc/=weight_acc;

      Spectrum_f spectrum_res;
      float alpha_res;
      TS_ASSERT( mp_film->GetPixel(test_point, spectrum_res, alpha_res, false) );

      TS_ASSERT_DELTA(spectrum_res[0], spectrum_acc[0], (1e-6));
      TS_ASSERT_DELTA(spectrum_res[1], spectrum_acc[1], (1e-6));
      TS_ASSERT_DELTA(spectrum_res[2], spectrum_acc[2], (1e-6));
      TS_ASSERT_DELTA(alpha_res, alpha_acc, (1e-6));
      }


    void test_ImageFilm_Clear()
      {
      for(size_t y=0;y<50;++y)
        for(size_t x=0;x<100;++x)
          {
          Point2D_d image_point=Point2D_d(y+RandomDouble(1.0),x+RandomDouble(1.0));
          Spectrum_f sp((float)RandomDouble(1.0),(float)RandomDouble(1.0),(float)RandomDouble(1.0));
          float alpha = (float)RandomDouble(1.0);
          mp_film->AddSample(image_point,sp,alpha);
          }

      mp_film->ClearFilm();

      bool cleared=true;
      for(size_t y=0;y<50;++y)
        for(size_t x=0;x<100;++x)
          {
          Spectrum_f spectrum_res;
          float alpha_res;
          if (mp_film->GetPixel(Point2D_i(x,y), spectrum_res, alpha_res, false))
            {
            cleared=false;
            break;
            }
          }

      TS_ASSERT(cleared);
      }

    // Test that GetPixel() method returns false outside of the cropping window.
    void test_ImageFilm_CropWindowPixels()
      {
      mp_film->SetCropWindow(Point2D_i(20,10),Point2D_i(80,40));
      for(size_t y=0;y<50;++y)
        for(size_t x=0;x<100;++x)
          {
          Point2D_d image_point=Point2D_d(y+RandomDouble(1.0),x+RandomDouble(1.0));
          Spectrum_f sp((float)RandomDouble(1.0),(float)RandomDouble(1.0),(float)RandomDouble(1.0));
          float alpha = (float)RandomDouble(1.0);
          mp_film->AddSample(image_point,sp,alpha);
          }

      Spectrum_f spectrum_res;
      float alpha_res;
      TS_ASSERT(mp_film->GetPixel(Point2D_i(5,10), spectrum_res, alpha_res, false)==false);
      TS_ASSERT(mp_film->GetPixel(Point2D_i(25,10), spectrum_res, alpha_res, false)==true);
      }

  private:
    shared_ptr<FilmFilter> mp_filter;
    shared_ptr<ImageFilm> mp_film;
  };

#endif // IMAGE_FILM_TEST_H
