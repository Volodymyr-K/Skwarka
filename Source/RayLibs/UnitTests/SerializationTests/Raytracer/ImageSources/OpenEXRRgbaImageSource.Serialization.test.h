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

#ifndef OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H
#define OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/ImageSources/OpenEXRRgbaImageSource.h>
#include <Math/ThreadSafeRandom.h>
#include <ImfRgba.h>
#include <ImfArray.h>
#include <vector>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class OpenEXRRgbaImageSourceSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_OpenEXRRgbaImageSource_Serialization()
      {
      double scale = 2.0;
      intrusive_ptr<ImageSource<Spectrum_f>> p_image_source1( new OpenEXRRgbaImageSource<Spectrum_f>("TestData/EXR/grace_latlong.exr", false, scale) );
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_image_source1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<ImageSource<Spectrum_f>> p_image_source2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_image_source2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(p_image_source1->GetHeight(), p_image_source2->GetHeight());
      TS_ASSERT_EQUALS(p_image_source1->GetWidth(), p_image_source2->GetWidth());

      std::vector<std::vector<Spectrum_f>> image1{ p_image_source1->GetImage() };
      std::vector<std::vector<Spectrum_f>> image2{ p_image_source2->GetImage() };
      if (image1 != image2)
        TS_FAIL("OpenEXRRgbaImageSource serialization test failed.");
      }

  private:
    const static size_t m_buffer_size=10*1024*1024;
    char m_data[m_buffer_size];
  };

#endif // OPENEXR_RGBA_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H