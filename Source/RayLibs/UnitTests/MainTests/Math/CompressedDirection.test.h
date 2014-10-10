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

#ifndef COMPRESSED_DIRECTION_TEST_H
#define COMPRESSED_DIRECTION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/Vector3D.h>
#include <Math/CompressedDirection.h>

class CompressedDirectionTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_CompressedDirection_ConversionAccuracyTest()
      {
      for(int x=-200;x<200;++x)
        for(int y=-200;y<200;++y)
          for(int z=-200;z<200;++z)
            if (x!=0 || y!=0 || z!=0)
              {
              Vector3D_d v = Vector3D_d(x,y,z).Normalized();

              CompressedDirection cd(v);
              Vector3D_d v2 = cd.ToVector3D<double>();

              if ((v-v2).Length() > 0.017)
                {
                TS_FAIL("Original and compressed vectors do not match.");
                return;
                }

              if (v2.IsNormalized() == false)
                {
                TS_FAIL("Compressed direction is not normalized.");
                return;
                }
              }
      }

    void test_CompressedDirection_ID()
      {
      for (size_t i=0;i<(1<<16);++i)
        {
        CompressedDirection dir = CompressedDirection::FromID((unsigned short)i);
        unsigned short j = dir.GetID();
        if (i!=j)
          {
          TS_FAIL("IDs do not match.");
          return;
          }
        }
      }

  };

#endif // COMPRESSED_DIRECTION_TEST_H