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

#ifndef BOX_FILTER_SERIALIZATION_TEST_H
#define BOX_FILTER_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/FilmFilter.h>
#include <Raytracer/FilmFilters/BoxFilter.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class BoxFilterSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_BoxFilter_Serialization()
      {
      intrusive_ptr<FilmFilter> p_filter1( new BoxFilter(1.0,2.0) );
        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_filter1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<FilmFilter> p_filter2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_filter2;
        } // archive and stream closed when destructors are called

      TS_ASSERT(dynamic_cast<BoxFilter*>(p_filter2.get()) != NULL); // Check run-time type.
      TS_ASSERT_EQUALS(p_filter2->GetXWidth(), 1.0);
      TS_ASSERT_EQUALS(p_filter2->GetYWidth(), 2.0);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // BOX_FILTER_SERIALIZATION_TEST_H
