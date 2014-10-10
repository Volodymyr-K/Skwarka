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

#ifndef IMAGE_FILM_SERIALIZATION_FILM_TEST_H
#define IMAGE_FILM_SERIALIZATION_FILM_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/Film.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Films/ImageFilm.h>
#include <Raytracer/FilmFilters/BoxFilter.h>
#include <Math/ThreadSafeRandom.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class ImageFilmSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_ImageFilm_Serialization()
      {
      intrusive_ptr<FilmFilter> p_filter = intrusive_ptr<FilmFilter>(new BoxFilter(4.0,2.0));
      intrusive_ptr<Film> p_film1 = intrusive_ptr<Film>(new ImageFilm(100,50,p_filter));

      // Add some test data.
      for(size_t i=0;i<=10000;++i)
        {
        Point2D_d image_point=Point2D_d(100,50);
        Spectrum_d sp(RandomDouble(1.0),RandomDouble(1.0),RandomDouble(1.0));
        p_film1->AddSample(image_point,sp);
        }

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_film1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Film> p_film2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_film2;
        } // archive and stream closed when destructors are called

      TS_ASSERT(dynamic_cast<ImageFilm*>(p_film2.get()) != NULL); // Check run-time type.

      TS_ASSERT_EQUALS(p_film1->GetXResolution(), p_film2->GetXResolution());
      TS_ASSERT_EQUALS(p_film1->GetYResolution(), p_film2->GetYResolution());

      Point2D_i begin1, end1, begin2, end2;
      p_film1->GetSamplingExtent(begin1, end1);
      p_film2->GetSamplingExtent(begin2, end2);
      TS_ASSERT_EQUALS(begin1, begin2);
      TS_ASSERT_EQUALS(end1, end2);

      // Compare pixel values.
      for(size_t x=0;x<100;++x)
        for(size_t y=0;y<50;++y)
          {
          Spectrum_d sp1,sp2;
          p_film1->GetPixel(Point2D_i(x,y),sp1);
          p_film2->GetPixel(Point2D_i(x,y),sp2);
          if (sp1 != sp2)
            {
            TS_FAIL("ImageFilm serialization test failed.");
            return;
            }
          }
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // IMAGE_FILM_SERIALIZATION_FILM_TEST_H
