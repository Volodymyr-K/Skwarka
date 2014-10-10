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

#ifndef SCALE_TEXTURE_SERIALIZATION_TEST_H
#define SCALE_TEXTURE_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <Raytracer/Textures/ScaleTexture.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class ScaleTextureSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_ScaleTexture_Serialization()
      {
      SpectrumCoef_d s1(1.0,2.0,3.0);
      Spectrum_d s2(0.5,2.5,3.2);
      intrusive_ptr<Texture<SpectrumCoef_d>> p_helper_texture1(new ConstantTexture<SpectrumCoef_d>(s1));
      intrusive_ptr<Texture<Spectrum_d>> p_helper_texture2(new ConstantTexture<Spectrum_d>(s2) );
      intrusive_ptr<Texture<Spectrum_d>> p_texture1(new ScaleTexture<SpectrumCoef_d, Spectrum_d>(p_helper_texture1, p_helper_texture2) );

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_texture1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Texture<Spectrum_d>> p_texture2;

        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_texture2;
        } // archive and stream closed when destructors are called

      DifferentialGeometry dg; // No need to fill with any data for the constant texture.
      Spectrum_d spectrum1 = p_texture1->Evaluate(dg, 0);
      Spectrum_d spectrum2 = p_texture2->Evaluate(dg, 0);
      TS_ASSERT_EQUALS(spectrum1, spectrum2);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // SCALE_TEXTURE_SERIALIZATION_TEST_H
