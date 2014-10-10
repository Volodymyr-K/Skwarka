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

#ifndef MATRIX_3X3_TEST_H
#define MATRIX_3X3_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Matrix3x3.h>
#include <cstring>

class Matrix3x3TestSuite : public CxxTest::TestSuite
  {
  public:
    // Most of the tests from this suite operate with the same sample matrix, so it is initialized in the setUp() method once.
    void setUp()
      {
      // Change these values carefully since many tests below depend on them.
      double elements[3][3]={
        1.0,1.1,1.2,
        -1.0,2.1,2.2,
        1.0,2.0,-2.0};

      // The elements form singular matrix.
      double elements2[3][3]={
        1.0,2.0,3.0,
        2.0,3.0,4.0,
        6.0,7.0,8.0};

      memcpy(m_elements,elements,sizeof(m_elements));
      memcpy(m_elements2,elements2,sizeof(m_elements2));

      m_matrix1=Matrix3x3_d(m_elements);
      m_matrix2=Matrix3x3_d(m_elements2);
      } 

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_Matrix3x3_DefaultConstr()
      {
      Matrix3x3_d m;
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          TS_ASSERT_EQUALS(m.m_values[i][j], 0.0);
      }

    void test_Matrix3x3_IdentityConstr()
      {
      Matrix3x3_d m(true);
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          TS_ASSERT_EQUALS(m.m_values[i][j], (i==j ? 1.0 : 0.0));
      }

    void test_Matrix3x3_Add()
      {
      Matrix3x3_d m_add=m_matrix1+m_matrix2;
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          TS_ASSERT_EQUALS(m_add.m_values[i][j], m_elements[i][j]+m_elements2[i][j]);
      }

    void test_Matrix3x3_AddAssign()
      {
      Matrix3x3_d m_add=m_matrix1;
      m_add+=m_matrix2;
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          TS_ASSERT_EQUALS(m_add.m_values[i][j], m_elements[i][j]+m_elements2[i][j]);
      }

    void test_Matrix3x3_Sub()
      {
      Matrix3x3_d m_sub=m_matrix1-m_matrix2;
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          TS_ASSERT_EQUALS(m_sub.m_values[i][j], m_elements[i][j]-m_elements2[i][j]);
      }

    void test_Matrix3x3_SubAssign()
      {
      Matrix3x3_d m_sub=m_matrix1;
      m_sub-=m_matrix2;
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          TS_ASSERT_EQUALS(m_sub.m_values[i][j], m_elements[i][j]-m_elements2[i][j]);
      }
    
    void test_Matrix3x3_ScalarMult()
      {
      Matrix3x3_d m_mult=m_matrix1*1.5;
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], m_elements[i][j]*1.5);
      }

    void test_Matrix3x3_ScalarMultAssign()
      {
      Matrix3x3_d m_mult=m_matrix1;
      m_mult*=1.5;
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], m_elements[i][j]*1.5);
      }

    void test_Matrix3x3_ScalarPreMult()
      {
      Matrix3x3_d m_mult=1.5*m_matrix1;
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], m_elements[i][j]*1.5);
      }

    void test_Matrix3x3_PreMult()
      {
      Matrix3x3_d m_mult=m_matrix1;
      m_mult.PreMultiply(m_matrix2);
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          {
          double val=0;
          for(unsigned char k=0;k<3;++k) val+=m_matrix2.m_values[i][k]*m_matrix1.m_values[k][j];
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], val);
          }
      }

    void test_Matrix3x3_PostMult()
      {
      Matrix3x3_d m_mult=m_matrix1;
      m_mult.PostMultiply(m_matrix2);
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          {
          double val=0;
          for(unsigned char k=0;k<3;++k) val+=m_matrix1.m_values[i][k]*m_matrix2.m_values[k][j];
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], val);
          }
      }

    void test_Matrix3x3_OperatorMult()
      {
      Matrix3x3_d m_mult=m_matrix1*m_matrix2;
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          {
          double val=0;
          for(unsigned char k=0;k<3;++k) val+=m_matrix1.m_values[i][k]*m_matrix2.m_values[k][j];
          TS_ASSERT_EQUALS(m_mult.m_values[i][j], val);
          }
      }

    void test_Matrix3x3_Transposed()
      {
      Matrix3x3_d m_trans=m_matrix1.Transposed();
      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          TS_ASSERT_EQUALS(m_trans.m_values[i][j], m_elements[j][i]);
      }

    void test_Matrix3x3_Inverted()
      {
      Matrix3x3_d m_inv;
      bool not_singular=m_matrix1.Inverted(m_inv);
      TS_ASSERT(not_singular);

      // Now multiply it back to see if it is an entity matrix.
      Matrix3x3_d entity = m_inv;
      entity.PreMultiply(m_matrix1);

      for(unsigned char i=0;i<3;++i)
        for(unsigned char j=0;j<3;++j)
          TS_ASSERT_DELTA(entity.m_values[i][j], (i==j ? 1.0 : 0.0), (1e-9));
      }

    void test_Matrix3x3_Multiply()
      {
      double v1[3], v2[3];
      v1[0]=1.0;v1[1]=2.0;v1[2]=3.0;

      m_matrix1.Multiply(v1,v2);
      for(unsigned char i=0;i<3;++i)
        TS_ASSERT_DELTA(v2[i], m_matrix1.m_values[i][0]*v1[0]+m_matrix1.m_values[i][1]*v1[1]+m_matrix1.m_values[i][2]*v1[2], (1e-9));
      }

    // Test for singular matrix inversion.
    void test_Matrix3x3_InvertedFail()
      {
      Matrix3x3_d m_inv;
      bool not_singular=m_matrix2.Inverted(m_inv);
      TS_ASSERT(not_singular==false);
      }

  private:
    double m_elements[3][3], m_elements2[3][3];
    Matrix3x3_d m_matrix1, m_matrix2;
  };

#endif // MATRIX_3X3_TEST_H