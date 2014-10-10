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

#ifndef PERSPECTIVE_CAMERA_SERIALIZATION_FILM_TEST_H
#define PERSPECTIVE_CAMERA_SERIALIZATION_FILM_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Transform.h>
#include <Raytracer/Cameras/PerspectiveCamera.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/FilmFilters/BoxFilter.h>
#include <Raytracer/Films/ImageFilm.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class PerspectiveCameraSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_PerspectiveCamera_Extent()
      {
      Transform transformation = MakeLookAt(Point3D_d(1.0,0.0,0.0), Vector3D_d(0.0,1.0,0.0), Vector3D_d(0.0,0.0,1.0));
      intrusive_ptr<FilmFilter> p_filter = intrusive_ptr<FilmFilter>(new BoxFilter(4.0,2.0));
      intrusive_ptr<Film> p_film(new ImageFilm(100,50,p_filter));
      intrusive_ptr<Camera> p_camera1( new PerspectiveCamera(transformation, p_film, 0.0, 0.0, 2.0*M_PI_3) );

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_camera1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Camera> p_camera2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_camera2;
        } // archive and stream closed when destructors are called

      TS_ASSERT(dynamic_cast<PerspectiveCamera*>(p_camera2.get()) != NULL); // Check run-time type.

      TS_ASSERT_EQUALS(p_camera1->GetFilm()->GetXResolution(), p_camera2->GetFilm()->GetXResolution());
      TS_ASSERT_EQUALS(p_camera1->GetFilm()->GetYResolution(), p_camera2->GetFilm()->GetYResolution());

      Point2D_i begin1, end1, begin2, end2;
      p_camera1->GetFilm()->GetSamplingExtent(begin1, end1);
      p_camera2->GetFilm()->GetSamplingExtent(begin2, end2);
      TS_ASSERT_EQUALS(begin1, begin2);
      TS_ASSERT_EQUALS(end1, end2);

      Point2D_d image_point(12,23);
      Point2D_d lens_uv(0.9,0.3);
      Ray ray1,ray2;
      p_camera1->GenerateRay(image_point, lens_uv, ray1);
      p_camera2->GenerateRay(image_point, lens_uv, ray2);
      TS_ASSERT_EQUALS(ray1.m_origin, ray2.m_origin);
      TS_ASSERT_EQUALS(ray1.m_direction, ray2.m_direction);
      TS_ASSERT_EQUALS(ray1.m_min_t, ray2.m_min_t);
      TS_ASSERT_EQUALS(ray1.m_max_t, ray2.m_max_t);
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // PERSPECTIVE_CAMERA_SERIALIZATION_FILM_TEST_H
