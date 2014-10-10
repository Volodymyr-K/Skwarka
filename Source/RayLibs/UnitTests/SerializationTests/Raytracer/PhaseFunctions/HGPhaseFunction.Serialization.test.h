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

#ifndef HG_PHASE_FUNCTION_SERIALIZATION_TEST_H
#define HG_PHASE_FUNCTION_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Vector3D.h>
#include <Raytracer/PhaseFunctions/HGPhaseFunction.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class HGPhaseFunctionSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_HGPhaseFunction_Serialization()
      {
      intrusive_ptr<const PhaseFunction> p_pf1(new HGPhaseFunction(0.5));

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_pf1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<const PhaseFunction> p_pf2;

        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_pf2;
        } // archive and stream closed when destructors are called

      Vector3D_d incoming=Vector3D_d(1,2,3).Normalized();
      Vector3D_d outgoing=Vector3D_d(1,2,-3).Normalized();
      double value1 = p_pf1->ScatteringPDF(incoming, outgoing);
      double value2 = p_pf2->ScatteringPDF(incoming, outgoing);
      TS_ASSERT_EQUALS(value1, value2);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // HG_PHASE_FUNCTION_SERIALIZATION_TEST_H