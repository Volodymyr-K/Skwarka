#ifndef INTERACTIVE_FILM_TEST_H
#define INTERACTIVE_FILM_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/Film.h>
#include <Raytracer/Core/Spectrum.h>
#include <UnitTests/Mocks/FilmFilterMock.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/Films/InteractiveFilm.h>
#include <Math/Point2D.h>

class InteractiveFilmTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      mp_filter = shared_ptr<FilmFilter>(new FilmFilterMock(4.0,2.0));
      mp_film = shared_ptr<InteractiveFilm>(new InteractiveFilm(100,50,mp_filter));
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_InteractiveFilm_Extent()
      {
      Point2D_i begin, end;
      mp_film->GetSamplingExtent(begin, end);

      TS_ASSERT_EQUALS(begin, Convert<int>( Point2D_d(-mp_filter->GetXWidth(),-mp_filter->GetYWidth()) ) );
      TS_ASSERT_EQUALS(end, Convert<int>( Point2D_d(100+mp_filter->GetXWidth(),50+mp_filter->GetYWidth()) ) );
      }

    void test_InteractiveFilm_CropWindowExtent()
      {
      Point2D_i begin, end;
      mp_film->SetCropWindow(Point2D_i(20,10),Point2D_i(80,40));
      mp_film->GetSamplingExtent(begin, end);

      TS_ASSERT_EQUALS(begin, Convert<int>( Point2D_d(20-mp_filter->GetXWidth(),10-mp_filter->GetYWidth()) ) );
      TS_ASSERT_EQUALS(end, Convert<int>( Point2D_d(80+mp_filter->GetXWidth(),40+mp_filter->GetYWidth()) ) );
      }

    void test_InteractiveFilm_Pixel()
      {
      Point2D_i test_point(15,15);
      Spectrum_f spectrum_acc;
      float alpha_acc=0.f;
      float weight_acc=0.f;

      for(size_t x=10;x<=20;++x)
        for(size_t y=10;y<=20;++y)
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


    void test_InteractiveFilm_Clear()
      {
      for(size_t x=0;x<100;++x)
        for(size_t y=0;y<50;++y)
          {
          Point2D_d image_point=Point2D_d(y+RandomDouble(1.0),x+RandomDouble(1.0));
          Spectrum_f sp((float)RandomDouble(1.0),(float)RandomDouble(1.0),(float)RandomDouble(1.0));
          float alpha = (float)RandomDouble(1.0);
          mp_film->AddSample(image_point,sp,alpha);
          }

      mp_film->ClearFilm();

      bool cleared=true;
      for(size_t x=0;x<100;++x)
        for(size_t y=0;y<50;++y)
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
    void test_InteractiveFilm_CropWindowPixels()
      {
      mp_film->SetCropWindow(Point2D_i(20,10),Point2D_i(80,40));

      for(size_t x=0;x<100;++x)
        for(size_t y=0;y<50;++y)
          {
          if (y==10 && x==54)
            x=x;
          Point2D_d image_point=Point2D_d(x+RandomDouble(1.0),y+RandomDouble(1.0));
          Spectrum_f sp((float)RandomDouble(1.0),(float)RandomDouble(1.0),(float)RandomDouble(1.0));
          float alpha = (float)RandomDouble(1.0);
          mp_film->AddSample(image_point,sp,alpha);
          }

      bool correct=true;
      for(size_t x=0;x<100;++x)
        for(size_t y=0;y<50;++y)
          {
          Spectrum_f spectrum_res;
          float alpha_res;
          bool pixel_read = mp_film->GetPixel(Point2D_i(x,y), spectrum_res, alpha_res, false);

          bool inside_crop_window = (x>=20 && x<80 && y>=10 && y<40);
          if (inside_crop_window != pixel_read)
            correct=false;
          }

      TS_ASSERT(correct);
      }

    // Test that after adding one sample other unfilled pixels have its spectrum and alpha value.
    void test_InteractiveFilm_Approximation()
      {
      Spectrum_f sp(1.f, 0.5f, 0.2f);
      mp_film->AddSample(Point2D_d(10.0,20.0),sp,0.5f);

      Spectrum_f spectrum_res;
      float alpha_res;
      bool pixel_read = mp_film->GetPixel(Point2D_i(20,30), spectrum_res, alpha_res, false);

      TS_ASSERT(pixel_read);
      TS_ASSERT_EQUALS(sp,spectrum_res);
      TS_ASSERT_EQUALS(0.5f,alpha_res);
      }

  private:
    shared_ptr<FilmFilter> mp_filter;
    shared_ptr<InteractiveFilm> mp_film;
  };

#endif // INTERACTIVE_FILM_TEST_H
