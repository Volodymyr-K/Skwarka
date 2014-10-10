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

#ifndef RGB24_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H
#define RGB24_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/ImageSources/RGBImageSource.h>
#include <Math/ThreadSafeRandom.h>
#include <vector>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class RGB24ImageSourceSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_RGB24ImageSource_Serialization()
      {
      double scale = 2.0;
      intrusive_ptr<ImageSource<Spectrum_f>> p_image_source1(new RGBImageSource<Spectrum_f>("TestData/red_200x100.tif", global_sRGB_E_ColorSystem, scale));
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
        TS_FAIL("RGB24ImageSource serialization test failed.");
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // RGB24_SPECTRUM_IMAGE_SOURCE_SERIALIZATION_TEST_H
