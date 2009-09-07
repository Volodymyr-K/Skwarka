#ifndef FILM_TEST_H
#define FILM_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Raytracer/Core/Film.h>
#include <Raytracer/Core/Spectrum.h>
#include <UnitTests/Mocks/FilmFilterMock.h>
#include <Math/ThreadSafeRandom.h>

class FilmTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      mp_filter = shared_ptr<FilmFilter>(new FilmFilterMock(4.0,2.0));
      mp_film=shared_ptr<Film>(new Film(100,50,mp_filter));
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void testFilmDefaultConstr()
      {
      shared_ptr<FilmFilter> p_filter = shared_ptr<FilmFilter>(new FilmFilterMock(1.0,1.0));
      Film film(100,50,p_filter);

      TS_ASSERT(film.GetXResolution()==100 && film.GetYResolution()==50);
      }

    void testFilmExtent()
      {
      Point2D_i begin, end;
      mp_film->GetSampleExtent(begin, end);

      TS_ASSERT_EQUALS(begin, Convert<int>( Point2D_d(-mp_filter->GetXWidth(),-mp_filter->GetYWidth()) ) );
      TS_ASSERT_EQUALS(end, Convert<int>( Point2D_d(100+mp_filter->GetXWidth(),50+mp_filter->GetYWidth()) ) );
      }

    void testFilmPixel()
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
      mp_film->GetPixel(test_point, spectrum_res, alpha_res, false);

      TS_ASSERT_DELTA(spectrum_res[0], spectrum_acc[0], (1e-6));
      TS_ASSERT_DELTA(spectrum_res[1], spectrum_acc[1], (1e-6));
      TS_ASSERT_DELTA(spectrum_res[2], spectrum_acc[2], (1e-6));
      TS_ASSERT_DELTA(alpha_res, alpha_acc, (1e-6));
      }


    void testFilmClear()
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
          mp_film->GetPixel(Point2D_i(x,y), spectrum_res, alpha_res, false);
          if (spectrum_res.IsBlack()==false || alpha_res!=0.f)
            {
            cleared=false;
            break;
            }
          }

      TS_ASSERT(cleared);
      }

  private:
    shared_ptr<FilmFilter> mp_filter;
    shared_ptr<Film> mp_film;
  };

#endif // FILM_TEST_H
