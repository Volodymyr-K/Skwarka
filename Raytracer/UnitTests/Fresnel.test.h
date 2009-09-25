#ifndef FRESNEL_TEST_H
#define FRESNEL_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
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
        Spectrum_d R = fresnel(cos(theta));
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
      Spectrum_d R = fresnel(cos(theta));

      TS_ASSERT_EQUALS(R, Spectrum_d(1.0));
      }

    // Tests that the method always returns values in [0;1] range.
    void test_FresnelConductor_Range()
      {
      FresnelConductor fresnel(Spectrum_d(0.37), Spectrum_d(2.82));

      bool range_valid=true;
      for(double theta=0.0;theta<=M_PI_2;theta+=0.01)
        {
        Spectrum_d R = fresnel(cos(theta));
        if (InRange(R,0.0,1.0)==false)
          range_valid=false;
        }

      TS_ASSERT(range_valid);
      }

    // Tests that for angles near PI/2 FresnelConductor returns 1.0
    void test_FresnelConductor_Limit()
      {
      FresnelConductor fresnel(Spectrum_d(0.37), Spectrum_d(2.82));

      Spectrum_d R = fresnel(0.0);
      TS_ASSERT_EQUALS(R, Spectrum_d(1.0));
      }
  };

#endif // FRESNEL_TEST_H