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

#ifndef INTERACTIVE_FILM_TEST_H
#define INTERACTIVE_FILM_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
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
      mp_filter = intrusive_ptr<FilmFilter>(new FilmFilterMock(4.0,2.0));
      mp_film = intrusive_ptr<InteractiveFilm>(new InteractiveFilm(100,50,mp_filter));
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
      Spectrum_d spectrum_acc;
      double weight_acc=0.f;

      for(size_t x=10;x<=20;++x)
        for(size_t y=10;y<=20;++y)
          {
          Point2D_d image_point=Point2D_d(y+RandomDouble(1.0),x+RandomDouble(1.0));
          Spectrum_d sp(RandomDouble(1.0),RandomDouble(1.0),RandomDouble(1.0));
          mp_film->AddSample(image_point,sp);

          double weight = mp_filter->Evaluate(0.5+test_point[0]-image_point[0], 0.5+test_point[1]-image_point[1]);
          spectrum_acc+=sp*weight;
          weight_acc+=weight;
          }

      spectrum_acc/=weight_acc;

      Spectrum_d spectrum_res;
      TS_ASSERT( mp_film->GetPixel(test_point, spectrum_res, false) );

      CustomAssertDelta(spectrum_res, spectrum_acc, (1e-6));
      }


    void test_InteractiveFilm_Clear()
      {
      for(size_t x=0;x<100;++x)
        for(size_t y=0;y<50;++y)
          {
          Point2D_d image_point=Point2D_d(y+RandomDouble(1.0),x+RandomDouble(1.0));
          Spectrum_d sp(RandomDouble(1.0),RandomDouble(1.0),RandomDouble(1.0));
          mp_film->AddSample(image_point,sp);
          }

      mp_film->ClearFilm();

      bool cleared=true;
      for(size_t x=0;x<100;++x)
        for(size_t y=0;y<50;++y)
          {
          Spectrum_d spectrum_res;
          if (mp_film->GetPixel(Point2D_i((int)x, (int)y), spectrum_res, false))
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
          Point2D_d image_point=Point2D_d(x+RandomDouble(1.0),y+RandomDouble(1.0));
          Spectrum_d sp(RandomDouble(1.0),RandomDouble(1.0),RandomDouble(1.0));
          mp_film->AddSample(image_point,sp);
          }

      bool correct=true;
      for(size_t x=0;x<100;++x)
        for(size_t y=0;y<50;++y)
          {
          Spectrum_d spectrum_res;
          bool pixel_read = mp_film->GetPixel(Point2D_i((int)x, (int)y), spectrum_res, false);

          bool inside_crop_window = (x>=20 && x<80 && y>=10 && y<40);
          if (inside_crop_window != pixel_read)
            correct=false;
          }

      TS_ASSERT(correct);
      }

    // Test that after adding one sample other unfilled pixels have its spectrum value.
    void test_InteractiveFilm_Approximation()
      {
      Spectrum_d sp(1.f, 0.5f, 0.2f);
      mp_film->AddSample(Point2D_d(10.0,20.0),sp);

      Spectrum_d spectrum_res;
      bool pixel_read = mp_film->GetPixel(Point2D_i(20,30), spectrum_res, false);

      TS_ASSERT(pixel_read);
      TS_ASSERT_EQUALS(sp,spectrum_res);
      }

    void test_InteractiveFilm_GetCropWindow()
      {
      mp_film->SetCropWindow(Point2D_i(20,10),Point2D_i(80,40));

      Point2D_i begin, end;
      mp_film->GetCropWindow(begin, end);

      TS_ASSERT_EQUALS(begin, Point2D_i(20,10));
      TS_ASSERT_EQUALS(end, Point2D_i(80,40));
      }

  private:
    intrusive_ptr<FilmFilter> mp_filter;
    intrusive_ptr<InteractiveFilm> mp_film;
  };

#endif // INTERACTIVE_FILM_TEST_H
