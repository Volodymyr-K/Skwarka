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

#ifndef SAMPLE_TEST_H
#define SAMPLE_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/Sample.h>
#include <vector>

class SampleTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Sample_Constr()
      {
      std::vector<size_t> sequences_1D_size(1,2), sequences_2D_size(2,2);
      Sample sample(sequences_1D_size,sequences_2D_size);

      TS_ASSERT_EQUALS(sequences_1D_size.size(), sample.GetNumberOfSamplesSequences1D());
      TS_ASSERT_EQUALS(sequences_2D_size.size(), sample.GetNumberOfSamplesSequences2D());
      }

    void test_Sample_ImagePoint()
      {
      std::vector<size_t> sequences_1D_size(1,2), sequences_2D_size(2,2);
      Sample sample(sequences_1D_size,sequences_2D_size);

      Point2D_d image_point(1.0,-2.0);
      sample.SetImagePoint(image_point);
      TS_ASSERT_EQUALS(image_point, sample.GetImagePoint());
      }

    void test_Sample_Sequences()
      {
      std::vector<size_t> sequences_1D_size, sequences_2D_size;
      sequences_1D_size.push_back(10);
      sequences_1D_size.push_back(20);
      sequences_1D_size.push_back(30);

      sequences_2D_size.push_back(15);
      sequences_2D_size.push_back(25);
      sequences_2D_size.push_back(35);

      Sample sample(sequences_1D_size, sequences_2D_size);
      double counter=0.0;

      // Fill 1D samples sequences with test values.
      for(size_t i=0;i<sample.GetNumberOfSamplesSequences1D();++i)
        {
        SamplesSequence1D sequence = sample.GetSamplesSequence1D(i);

        // Verify size of the sequence.
        TS_ASSERT_EQUALS(std::distance(sequence.m_begin,sequence.m_end), sequences_1D_size[i]);

        for(SamplesSequence1D::Iterator it=sequence.m_begin;it!=sequence.m_end;++it)
          {
          (*it)=counter;
          counter+=0.3;
          }
        }

      // Fill 2D samples sequences with test values.
      for(size_t i=0;i<sample.GetNumberOfSamplesSequences2D();++i)
        {
        SamplesSequence2D sequence = sample.GetSamplesSequence2D(i);

        // Verify size of the sequence.
        TS_ASSERT_EQUALS(std::distance(sequence.m_begin,sequence.m_end), sequences_2D_size[i]);

        for(SamplesSequence2D::Iterator it=sequence.m_begin;it!=sequence.m_end;++it)
          {
          (*it)=Point2D_d(counter,counter+0.3);
          counter+=0.6;
          }
        }

      bool coincide=true;
      counter=0.0;

      // Verify 1D samples sequences values.
      for(size_t i=0;i<sample.GetNumberOfSamplesSequences1D();++i)
        {
        SamplesSequence1D sequence = sample.GetSamplesSequence1D(i);
        for(SamplesSequence1D::Iterator it=sequence.m_begin;it!=sequence.m_end;++it)
          {
          if ((*it)!=counter) coincide=false;
          counter+=0.3;
          }
        }

      // Verify 2D samples sequences values.
      for(size_t i=0;i<sample.GetNumberOfSamplesSequences2D();++i)
        {
        SamplesSequence2D sequence = sample.GetSamplesSequence2D(i);
        for(SamplesSequence2D::Iterator it=sequence.m_begin;it!=sequence.m_end;++it)
          {
          if ((*it)!=Point2D_d(counter,counter+0.3)) coincide=false;
          counter+=0.6;
          }
        }

      TS_ASSERT(coincide);
      }
  };

#endif // SAMPLE_TEST_H
