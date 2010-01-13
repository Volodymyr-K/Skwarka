#ifndef TRANSFORM_TEST_H
#define TRANSFORM_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Transform.h>
#include <Math/Constants.h>
#include <cstring>

class TransformTestSuite : public CxxTest::TestSuite
  {
  public:
    // Most of the tests from this suite operate with the same sample matrix, so it is initialized in the setUp() method once.
    void setUp()
      {
      // Change the matrix elements carefully since many tests below depend on them.

      // Scaling matrix.
      double elements[4][4]={
        1.0,0.0,0.0,0.0,
        0.0,2.0,0.0,0.0,
        0.0,0.0,3.0,0.0,
        0.0,0.0,0.0,2.0};

      // Translation matrix.
      double elements2[4][4]={
        1.0,0.0,0.0,-1.0,
        0.0,1.0,0.0,-2.5,
        0.0,0.0,1.0,1.5,
        0.0,0.0,0.0,1.0};

      memcpy(m_elements,elements,sizeof(m_elements));
      memcpy(m_elements2,elements2,sizeof(m_elements2));

      m_matrix1=Matrix4x4_d(m_elements);
      m_matrix2=Matrix4x4_d(m_elements2);

      // The test ray for the Make*() functions.
      m_origin=Point3D_d(1.0,2.0,3.0);
      m_direction=Vector3D_d(-1.0,0.5,2.0);
      m_test_ray=Ray(m_origin,m_direction);
      } 

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_Transform_DefaultConstr()
      {
      Transform t;
      Point3D_d p(1.0,2.0,3.0);
      Point3D_d p2;
      t(p,p2);
      CustomAssertDelta(p,p2,(1e-10));
      }

    void test_Transform_ConstrWithMatrixElements()
      {
      Transform t(m_elements);
      Point3D_d p(1.0,2.0,3.0);
      Point3D_d p2;
      t(p,p2);
      CustomAssertDelta(p2,Point3D_d(0.5,2.0,4.5),(1e-10));
      }

    void test_Transform_ConstrWithMatrix()
      {
      Transform t(m_matrix1);
      Point3D_d p(1.0,2.0,3.0);
      Point3D_d p2;
      t(p,p2);
      CustomAssertDelta(p2,Point3D_d(0.5,2.0,4.5),(1e-10));
      }

    void test_Transform_Inverted()
      {
      Transform t(m_matrix1);
      Transform t_inv = t.Inverted();
      Point3D_d p(0.5,2.0,4.5);
      Point3D_d p2;
      t_inv(p,p2);
      CustomAssertDelta(p2,Point3D_d(1.0,2.0,3.0),(1e-10));
      }

    void test_Transform_Point()
      {
      Transform t(m_matrix1);
      Point3D_d p(1.0,2.0,3.0);
      Point3D_d p2=t(p);
      CustomAssertDelta(p2,Point3D_d(0.5,2.0,4.5),(1e-10));
      }

    void test_Transform_Vector()
      {
      Transform t(m_matrix1);
      Vector3D_d v(1.0,2.0,3.0);
      Vector3D_d v2=t(v);
      CustomAssertDelta(v2,Vector3D_d(1.0,4.0,9.0),(1e-10));
      }

    void test_Transform_Ray()
      {
      Transform t(m_matrix1);
      Ray ray2=t(m_test_ray);
      CustomAssertDelta(ray2.m_origin,Point3D_d(m_origin[0]*0.5,m_origin[1],m_origin[2]*1.5),(1e-10));
      CustomAssertDelta(ray2.m_direction,Vector3D_d(m_direction[0],m_direction[1]*2.0,m_direction[2]*3.0),(1e-10));
      }

    void test_Transform_Composition()
      {
      Transform t(m_matrix1);
      Transform t2(m_matrix2);
      Transform composed = t*t2;
      Point3D_d p(1.0,2.0,3.0);
      Point3D_d p2=composed(p);

      CustomAssertDelta(p2,t(t2(p)),(1e-10));
      }

    void test_Transform_Translation()
      {
      Transform t=MakeTranslation(Vector3D_d(-1.0,2.0,-3.0));

      Ray ray2=t(m_test_ray);
      CustomAssertDelta(ray2.m_origin,Point3D_d(m_origin[0]-1.0,m_origin[1]+2.0,m_origin[2]-3.0),(1e-10));
      CustomAssertDelta(ray2.m_direction,m_direction,(1e-10));
      }

    void test_Transform_Scale()
      {
      Transform t=MakeScale(-0.1,1.0,0.3);

      Ray ray2=t(m_test_ray);
      CustomAssertDelta(ray2.m_origin,Point3D_d(-m_origin[0]*0.1,m_origin[1],m_origin[2]*0.3),(1e-10));
      CustomAssertDelta(ray2.m_direction,Vector3D_d(-m_direction[0]*0.1,m_direction[1],m_direction[2]*0.3),(1e-10));
      }

    void test_Transform_RotationX()
      {
      Transform t=MakeRotationX(M_PI_2);

      Ray ray2=t(m_test_ray);
      CustomAssertDelta(ray2.m_origin,Point3D_d(m_origin[0],-m_origin[2],m_origin[1]),(1e-10));
      CustomAssertDelta(ray2.m_direction,Vector3D_d(m_direction[0],-m_direction[2],m_direction[1]),(1e-10));
      }

    void test_Transform_RotationY()
      {
      Transform t=MakeRotationY(M_PI_2);

      Ray ray2=t(m_test_ray);
      CustomAssertDelta(ray2.m_origin,Point3D_d(m_origin[2],m_origin[1],-m_origin[0]),(1e-10));
      CustomAssertDelta(ray2.m_direction,Vector3D_d(m_direction[2],m_direction[1],-m_direction[0]),(1e-10));
      }

    void test_Transform_RotationZ()
      {
      Transform t=MakeRotationZ(M_PI_2);

      Ray ray2=t(m_test_ray);
      CustomAssertDelta(ray2.m_origin,Point3D_d(-m_origin[1],m_origin[0],m_origin[2]),(1e-10));
      CustomAssertDelta(ray2.m_direction,Vector3D_d(-m_direction[1],m_direction[0],m_direction[2]),(1e-10));
      }

    void test_Transform_Rotation()
      {
      Transform t=MakeRotation(2.0*M_PI_3,Vector3D_d(1.0,1.0,1.0));

      Ray ray2=t(m_test_ray);
      CustomAssertDelta(ray2.m_origin,Point3D_d(m_origin[2],m_origin[0],m_origin[1]),(1e-10));
      CustomAssertDelta(ray2.m_direction,Vector3D_d(m_direction[2],m_direction[0],m_direction[1]),(1e-10));
      }

    void test_Transform_MatchDirections()
      {
      Vector3D_d dir1 = Vector3D_d(1,2,3).Normalized();
      Vector3D_d dir2 = Vector3D_d(1.8,1,-0.5).Normalized();
      Transform t=MakeMatchDirections(dir1, dir2);

      Vector3D_d tranformed = t(dir1);

      CustomAssertDelta(dir2, tranformed, (1e-10));
      }

    void test_Transform_MatchDirections_SameAxis()
      {
      Vector3D_d dir1 = Vector3D_d(1,2,3).Normalized();
      Vector3D_d dir2 = Vector3D_d(1,2,3).Normalized();
      Transform t=MakeMatchDirections(dir1, dir2);

      Vector3D_d tranformed = t(dir1);

      CustomAssertDelta(dir2, tranformed, (1e-10));
      }

    void test_Transform_MatchDirections_OppositeAxis()
      {
      Vector3D_d dir1 = Vector3D_d(1,2,3).Normalized();
      Vector3D_d dir2 = Vector3D_d(-1,-2,-3).Normalized();
      Transform t=MakeMatchDirections(dir1, dir2);

      Vector3D_d tranformed = t(dir1);

      CustomAssertDelta(dir2, tranformed, (1e-10));
      }

    void test_Transform_LookAt()
      {
      Point3D_d origin(3.0,2.0,1.0);
      Vector3D_d dir(-1.0,-2.0,-1.0);
      Vector3D_d dir_norm=dir.Normalized();
      Vector3D_d up(0.0,0.0,1.0);

      // The function must normalize the direction, so we don't pass the normalized direction to test that.
      Transform t=MakeLookAt(origin,dir,up);

      Ray ray2=t(m_test_ray);

      Vector3D_d right = (dir^up).Normalized();
      Vector3D_d down = (dir^right).Normalized();
      CustomAssertDelta(ray2.m_origin, origin+m_origin[0]*right+m_origin[1]*down+m_origin[2]*dir_norm, (1e-10));
      CustomAssertDelta(ray2.m_direction, m_direction[0]*right+m_direction[1]*down+m_direction[2]*dir_norm, (1e-10));
      }

  private:
    double m_elements[4][4], m_elements2[4][4];
    Matrix4x4_d m_matrix1, m_matrix2;

    // Test ray data.
    Ray m_test_ray;
    Point3D_d m_origin;
    Vector3D_d m_direction;
  };

#endif // TRANSFORM_TEST_H