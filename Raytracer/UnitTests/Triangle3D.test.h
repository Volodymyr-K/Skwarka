#ifndef TRIANGLE3D_TEST_H
#define TRIANGLE3D_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Triangle3D.h>
#include <cstring>

class Triangle3DTestSuite : public CxxTest::TestSuite
  {
  public:
    // Most of the tests from this suite operate with the same vertices, so they are initialized in the setUp() method once.
    void setUp()
      {
      // Change these values carefully since many tests below depend on them.
      m_vertices[0]=Point3D_d(1.0,0.0,-1.0);
      m_vertices[1]=Point3D_d(1.5,3.5,4.0);
      m_vertices[2]=Point3D_d(5.0,4.0,-2.0);
      } 

    void tearDown()
      {
      // Nothing to clear.
      }

    void testTriangleDefaultConstr()
      {
      Triangle3D_d t;
      TS_ASSERT_EQUALS(t[0], Point3D_d(0.0,0.0,0.0));
      TS_ASSERT_EQUALS(t[1], Point3D_d(0.0,0.0,0.0));
      TS_ASSERT_EQUALS(t[2], Point3D_d(0.0,0.0,0.0));
      }

    void testTriangleConstrWithVertices()
      {
      Triangle3D_d t(m_vertices[0], m_vertices[1], m_vertices[2]);
      TS_ASSERT_EQUALS(t[0], m_vertices[0]);
      TS_ASSERT_EQUALS(t[1], m_vertices[1]);
      TS_ASSERT_EQUALS(t[2], m_vertices[2]);
      }

    void testTriangleNormal()
      {
      Triangle3D_d t(m_vertices[0], m_vertices[1], m_vertices[2]);
      Vector3D_d normal = t.GetNormal();
      CustomAssertDelta(normal, (Vector3D_d(m_vertices[1]-m_vertices[0]) ^ Vector3D_d(m_vertices[2]-m_vertices[1])).Normalized(), (1e-10));
      }

    void testTriangleNormalOrientation()
      {
      Triangle3D_d t1(m_vertices[0], m_vertices[1], m_vertices[2]);
      Vector3D_d normal1 = t1.GetNormal();

      Triangle3D_d t2(m_vertices[1], m_vertices[0], m_vertices[2]);
      Vector3D_d normal2 = t2.GetNormal();

      TS_ASSERT_EQUALS(normal1,normal2*(-1.0));
      }
  
    void testTriangleArea()
      {
      Triangle3D_d t(m_vertices[0], m_vertices[1], m_vertices[2]);
      double area=t.GetArea();
      TS_ASSERT_DELTA(area, 0.5*(Vector3D_d(m_vertices[1]-m_vertices[0]) ^ Vector3D_d(m_vertices[2]-m_vertices[1])).Length(), (1e-10));
      }

    void testTriangleIndexOperator()
      {
      Triangle3D_d t(m_vertices[0], m_vertices[1], m_vertices[2]);
      TS_ASSERT(t[0]==m_vertices[0] && t[1]==m_vertices[1] && t[2]==m_vertices[2]);
      }


  private:
    Point3D_d m_vertices[3];
  };

#endif // TRIANGLE3D_TEST_H