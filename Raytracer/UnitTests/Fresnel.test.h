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
    void test_Fresnel_DielectricRange()
      {
      bool range_valid=true;
      for(double theta=0.0;theta<=M_PI;theta+=0.01)
        {
        double R = FresnelDielectricReflectance(1.5, 1.0, cos(theta));
        if (R<0.0 || R>1.0)
          range_valid=false;
        }

      TS_ASSERT(range_valid);
      }

    // Tests for the total internal reflection case.
    void test_Fresnel_DielectricTotalInternalReflection()
      {
      double theta=M_PI_2+0.1;
      double R = FresnelDielectricReflectance(1.5, 1.0, cos(theta));

      TS_ASSERT_EQUALS(R, 1.0);
      }

    // Tests that the method always returns values in [0;1] range.
    void test_Fresnel_ConductorRange()
      {
      bool range_valid=true;
      for(double theta=0.0;theta<=M_PI_2;theta+=0.01)
        {
        // These settings are for gold.
        double R = FresnelConductorReflectance(0.37, 2.82, cos(theta));
        if (R<0.0 || R>1.0)
          range_valid=false;
        }

      TS_ASSERT(range_valid);
      }
  };

#endif // FRESNEL_TEST_H