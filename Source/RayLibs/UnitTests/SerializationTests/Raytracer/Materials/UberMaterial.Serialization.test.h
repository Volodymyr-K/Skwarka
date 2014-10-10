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

#ifndef UBER_MATERIAL_SERIALIZATION_TEST_H
#define UBER_MATERIAL_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Materials/UberMaterial.h>
#include <Raytracer/Textures/ConstantTexture.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class UberMaterialSerializationTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_UberMaterial_Serialization()
      {
      intrusive_ptr<Texture<SpectrumCoef_d>> p_Kd( new ConstantTexture<SpectrumCoef_d>(SpectrumCoef_d(0.1,0.2,0.3)) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_Ks( new ConstantTexture<SpectrumCoef_d>(SpectrumCoef_d(0.3,0.1,0.05)) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_Kr( new ConstantTexture<SpectrumCoef_d>(SpectrumCoef_d(0.15,0.12,0.25)) );
      intrusive_ptr<Texture<SpectrumCoef_d>> p_opacity( new ConstantTexture<SpectrumCoef_d>(SpectrumCoef_d(0.2,0.2,0.2)) );
      intrusive_ptr<Texture<double>> p_roughness( new ConstantTexture<double>(0.1) );

      intrusive_ptr<Material> p_material1(new UberMaterial(p_Kd, p_Ks, p_Kr, p_roughness, p_opacity, 1.5));

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_material1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<Material> p_material2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_material2;
        } // archive and stream closed when destructors are called

      MemoryPool pool;
      DifferentialGeometry dg;
      dg.m_geometric_normal=dg.m_shading_normal=Vector3D_d(0.0,0.0,1.0);
      dg.m_tangent=Vector3D_d(1.0,0.0,0.0);

      SpectrumCoef_d val1 = p_material1->GetBSDF(dg, 0, pool)->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));
      SpectrumCoef_d val2 = p_material2->GetBSDF(dg, 0, pool)->Evaluate(Vector3D_d(0.0,0.0,1.0), Vector3D_d(0.0,0.0,1.0));
      TS_ASSERT_EQUALS(val1,val2);
      }

  private:
    const static size_t m_buffer_size=16384;
    char m_data[m_buffer_size];
  };

#endif // UBER_MATERIAL_SERIALIZATION_TEST_H
