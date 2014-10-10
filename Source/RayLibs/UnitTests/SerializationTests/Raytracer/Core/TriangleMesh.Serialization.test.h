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

#ifndef TRIANGLE_MESH_SERIALIZATION_TEST_H
#define TRIANGLE_MESH_SERIALIZATION_TEST_H

#include <cxxtest/TestSuite.h>
#include <Common/CommonSerialization.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/TriangleMesh.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <UnitTests/TestHelpers/TriangleMeshTestHelper.h>
#include <vector>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>

typedef boost::iostreams::basic_array_sink<char> SinkDevice;
typedef boost::iostreams::basic_array_source<char> SourceDevice;

class TriangleMeshSerializationTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_TriangleMesh_Serialization()
      {
      intrusive_ptr<TriangleMesh> p_mesh1=TriangleMeshHelper::ConstructSphere(Point3D_d(1,2,3), 3.14, 5);
      p_mesh1->SetInvertNormals(true);

        {
        boost::iostreams::stream_buffer<SinkDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_oarchive output_archive(buffer);
        output_archive << p_mesh1;
        } // archive and stream closed when destructors are called

      intrusive_ptr<TriangleMesh> p_mesh2;
        {
        boost::iostreams::stream_buffer<SourceDevice> buffer(m_data, m_buffer_size);
        boost::archive::binary_iarchive input_archive(buffer);
        input_archive >> p_mesh2;
        } // archive and stream closed when destructors are called

      TS_ASSERT_EQUALS(p_mesh1->GetNumberOfVertices(), p_mesh2->GetNumberOfVertices());
      TS_ASSERT_EQUALS(p_mesh1->GetNumberOfTriangles(), p_mesh2->GetNumberOfTriangles());
      TS_ASSERT_EQUALS(p_mesh1->GetArea(), p_mesh2->GetArea());
      TS_ASSERT_EQUALS(p_mesh1->GetBounds().m_min, p_mesh2->GetBounds().m_min);
      TS_ASSERT_EQUALS(p_mesh1->GetBounds().m_max, p_mesh2->GetBounds().m_max);

      TS_ASSERT_EQUALS(p_mesh1->GetUseShadingNormals(), p_mesh2->GetUseShadingNormals());
      TS_ASSERT_EQUALS(p_mesh1->GetInvertNormals(), p_mesh2->GetInvertNormals());

      TS_ASSERT_EQUALS(p_mesh1->GetTopologyInfo().m_manifold, p_mesh2->GetTopologyInfo().m_manifold);
      TS_ASSERT_EQUALS(p_mesh1->GetTopologyInfo().m_number_of_patches, p_mesh2->GetTopologyInfo().m_number_of_patches);
      TS_ASSERT_EQUALS(p_mesh1->GetTopologyInfo().m_solid, p_mesh2->GetTopologyInfo().m_solid);

      for(size_t i=0;i<p_mesh1->GetNumberOfVertices();++i)
        if (p_mesh1->GetVertex(i) != p_mesh2->GetVertex(i))
          {
          TS_FAIL("TriangleMesh serialization test failed.");
          return;
          }
      }

  private:
    const static size_t m_buffer_size=1024*1024;
    char m_data[m_buffer_size];
  };

#endif // TRIANGLE_MESH_SERIALIZATION_TEST_H