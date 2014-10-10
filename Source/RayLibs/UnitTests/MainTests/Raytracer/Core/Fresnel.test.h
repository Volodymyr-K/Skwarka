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

#ifndef FRESNEL_TEST_H
#define FRESNEL_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/Constants.h>
#include <cmath>

class FresnelTestSuite : public CxxTest::TestSuite
  {
  public:
    // Tests that the method always returns values in [0;1] range.
    void test_FresnelDielectric_Range()
      {
      FresnelDielectric fresnel(1.5,1.0);

      bool range_valid=true;
      for(double theta=0.0;theta<=M_PI;theta+=0.01)
        {
        SpectrumCoef_d R = fresnel(cos(theta));
        if (InRange(R,0.0,1.0)==false)
          range_valid=false;
        }

      TS_ASSERT(range_valid);
      }

    // Tests for the total internal reflection case.
    void test_FresnelDielectric_TotalInternalReflection()
      {
      FresnelDielectric fresnel(1.5,1.0);

      double theta=M_PI_2+0.1;
      SpectrumCoef_d R = fresnel(cos(theta));

      TS_ASSERT_EQUALS(R, SpectrumCoef_d(1.0));
      }

    // Tests that the method always returns values in [0;1] range.
    void test_FresnelConductor_Range()
      {
      FresnelConductor fresnel(SpectrumCoef_d(0.37), SpectrumCoef_d(2.82));

      bool range_valid=true;
      for(double theta=0.0;theta<=M_PI_2;theta+=0.01)
        {
        SpectrumCoef_d R = fresnel(cos(theta));
        if (InRange(R,0.0,1.0)==false)
          range_valid=false;
        }

      TS_ASSERT(range_valid);
      }

    // Tests that for angles near PI/2 FresnelConductor returns 1.0
    void test_FresnelConductor_Limit()
      {
      FresnelConductor fresnel(SpectrumCoef_d(0.37), SpectrumCoef_d(2.82));

      SpectrumCoef_d R = fresnel(0.0);
      TS_ASSERT_EQUALS(R, SpectrumCoef_d(1.0));
      }

    // Tests that fresnel with approximated parameters generates the same reflectance value that was used for estimating.
    void test_ApproximateFresnelParameters()
      {
      SpectrumCoef_d refractive_index(0.17,0.44,1.41), absorption(3.97,2.26,1.5);
      SpectrumCoef_d color = FresnelConductor(refractive_index, absorption)(1.0);

      SpectrumCoef_d refractive_index2, absorption2;
      ApproximateFresnelParameters(color, refractive_index2, absorption2);

      SpectrumCoef_d color2 = FresnelConductor(refractive_index2, absorption2)(1.0);

      CustomAssertDelta(color, color2,(1e-10));
      }

  };

#endif // FRESNEL_TEST_H