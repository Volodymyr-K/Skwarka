#ifndef MATRIX_TEST_H
#define MATRIX_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Math/Matrix.h>
#include <cstring>

class MatrixTestSuite : public CxxTest::TestSuite
  {
  public:
    // Most of the tests from this suite operate with the same sample matrix, so it is initialized in the setUp() method once.
    void setUp()
      {
      // Change these values carefully as many tests below depends on them.
      double elements[16]={
        1.0,1.1,1.2,1.3,
        -1.0,2.1,2.2,2.3,
        1.0,2.0,-2.0,2.0,
        2.5,2.5,-2.5,-2.5};

      // The elements form singular matrix.
      double elements2[16]={
        1.0,2.0,3.0,4.0,
        2.0,3.0,4.0,5.0,
        6.0,7.0,8.0,9.0,
        10.0,11.0,12.0,13.0};

      memcpy(m_elements,elements,sizeof(m_elements));
      memcpy(m_elements2,elements2,sizeof(m_elements2));

      m_matrix1=Matrix4x4_d(
        m_elements[0],m_elements[1],m_elements[2],m_elements[3],
        m_elements[4+0],m_elements[4+1],m_elements[4+2],m_elements[4+3],
        m_elements[8+0],m_elements[8+1],m_elements[8+2],m_elements[8+3],
        m_elements[12+0],m_elements[12+1],m_elements[12+2],m_elements[12+3]);

      m_matrix2=Matrix4x4_d(
        m_elements2[0],m_elements2[1],m_elements2[2],m_elements2[3],
        m_elements2[4+0],m_elements2[4+1],m_elements2[4+2],m_elements2[4+3],
        m_elements2[8+0],m_elements2[8+1],m_elements2[8+2],m_elements2[8+3],
        m_elements2[12+0],m_elements2[12+1],m_elements2[12+2],m_elements2[12+3]);
      } 

    void tearDown()
      {
      // Nothing to clear.
      }

    void testMatrixDefaultConstr(void)
      {
      Matrix4x4_d m;
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          TS_ASSERT_EQUALS(m.m_values[i][j], 0.0);
      }

    void testMatrixIdentityConstr(void)
      {
      Matrix4x4_d m(true);
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          TS_ASSERT_EQUALS(m.m_values[i][j], (i==j ? 1.0 : 0.0));
      }

    void testMatrixAdd(void)
      {
      Matrix4x4_d m_add=m_matrix1+m_matrix2;
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          TS_ASSERT_EQUALS(m_add.m_values[i][j], m_elements[4*i+j]+m_elements2[4*i+j]);
      }

    void testMatrixAddAssign(void)
      {
      Matrix4x4_d m_add=m_matrix1;
      m_add+=m_matrix2;
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          TS_ASSERT_EQUALS(m_add.m_values[i][j], m_elements[4*i+j]+m_elements2[4*i+j]);
      }

    void testMatrixSub(void)
      {
      Matrix4x4_d m_sub=m_matrix1-m_matrix2;
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          TS_ASSERT_EQUALS(m_sub.m_values[i][j], m_elements[4*i+j]-m_elements2[4*i+j]);
      }

    void testMatrixSubAssign(void)
      {
      Matrix4x4_d m_sub=m_matrix1;
      m_sub-=m_matrix2;
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          TS_ASSERT_EQUALS(m_sub.m_values[i][j], m_elements[4*i+j]-m_elements2[4*i+j]);
      }
    
    void testMatrixScalarMult(void)
      {
      Matrix4x4_d m_mult=m_matrix1*1.5;
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], m_elements[4*i+j]*1.5);
      }

    void testMatrixScalarMultAssign(void)
      {
      Matrix4x4_d m_mult=m_matrix1;
      m_mult*=1.5;
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], m_elements[4*i+j]*1.5);
      }

    void testMatrixScalarPreMult(void)
      {
      Matrix4x4_d m_mult=1.5*m_matrix1;
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], m_elements[4*i+j]*1.5);
      }

    void testMatrixPreMult(void)
      {
      Matrix4x4_d m_mult=m_matrix1;
      m_mult.PreMultiply(m_matrix2);
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          {
          double val=0;
          for(unsigned char k=0;k<4;++k) val+=m_matrix2.m_values[i][k]*m_matrix1.m_values[k][j];
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], val);
          }
      }

    void testMatrixPostMult(void)
      {
      Matrix4x4_d m_mult=m_matrix1;
      m_mult.PostMultiply(m_matrix2);
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          {
          double val=0;
          for(unsigned char k=0;k<4;++k) val+=m_matrix1.m_values[i][k]*m_matrix2.m_values[k][j];
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], val);
          }
      }

    void testMatrixOperatorMult(void)
      {
      Matrix4x4_d m_mult=m_matrix1*m_matrix2;
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          {
          double val=0;
          for(unsigned char k=0;k<4;++k) val+=m_matrix1.m_values[i][k]*m_matrix2.m_values[k][j];
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], val);
          }
      }

    void testMatrixTransposed(void)
      {
      Matrix4x4_d m_trans=m_matrix1.Transposed();
      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          TS_ASSERT_EQUALS(m_trans.m_values[i][j], m_elements[4*j+i]);
      }

    void testMatrixInverted(void)
      {
      Matrix4x4_d m_inv;
      bool not_singular=m_matrix1.Inverted(m_inv);
      TS_ASSERT(not_singular);

      // Now multiply it back to see if it is an entity matrix.
      Matrix4x4_d entity = m_inv;
      entity.PreMultiply(m_matrix1);

      for(unsigned char i=0;i<4;++i)
        for(unsigned char j=0;j<4;++j)
          TS_ASSERT_DELTA(entity.m_values[i][j], (i==j ? 1.0 : 0.0), (1e-9));
      }

    // Test for singular matrix inversion.
    void testMatrixInvertedFail(void)
      {
      Matrix4x4_d m_inv;
      bool not_singular=m_matrix2.Inverted(m_inv);
      TS_ASSERT(not_singular==false);
      }

  private:
    double m_elements[16], m_elements2[16];
    Matrix4x4_d m_matrix1, m_matrix2;
  };

#endif // MATRIX_TEST_H