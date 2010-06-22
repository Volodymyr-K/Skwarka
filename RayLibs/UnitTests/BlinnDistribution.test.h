#ifndef BLINN_DISTRIBUTION_TEST_H
#define BLINN_DISTRIBUTION_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Math/ThreadSafeRandom.h>

class BlinnDistributionTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_BlinnDistribution_EvaluateZeroExponent()
      {
      BlinnDistribution blinn(0.0);
      double value=blinn.Evaluate(Vector3D_d(0.5,-0.5,0.5).Normalized());
      TS_ASSERT_EQUALS(value,INV_PI);
      }

    void test_BlinnDistribution_Sample()
      {
      BlinnDistribution blinn(10.0);

      bool correct=true;
      for(size_t i=0;i<1000;++i)
        {
        Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));

        double pdf;
        Vector3D_d incident=Vector3D_d(0.5,-0.5,-0.5).Normalized();
        Vector3D_d exitant;
        blinn.Sample(incident, exitant, sample, pdf);
        if (exitant.IsNormalized()==false || pdf<0.0)
          correct=false;
        }

      TS_ASSERT(correct);
      }

    // Tests that for high exponents sampled directions are always in the same hemisphere.
    void test_BlinnDistribution_SampleHighExponent()
      {
      BlinnDistribution blinn(500.0);

      bool correct_hemisphere=true;
      for(size_t i=0;i<1000;++i)
        {
        Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));

        double pdf;
        Vector3D_d incident=Vector3D_d(0.5,-0.5,-0.5).Normalized();
        Vector3D_d exitant;
        blinn.Sample(incident, exitant, sample, pdf);

        if (exitant[2]>0.0)
          correct_hemisphere=false;
        }

      TS_ASSERT(correct_hemisphere);
      }

    // Tests that PDF() method returns the same PDF value as the Sample() method does.
    void test_BlinnDistribution_PDF()
      {
      BlinnDistribution blinn(10.0);

      bool correct_pdf=true;
      for(size_t i=0;i<1000;++i)
        {
        Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));

        double pdf;
        Vector3D_d incident=Vector3D_d(0.5,-0.5,-0.5).Normalized();
        Vector3D_d exitant;
        blinn.Sample(incident, exitant, sample, pdf);

        double pdf2=blinn.PDF(incident,exitant);
        if (fabs(pdf-pdf2)>(1e-5)*pdf)
          correct_pdf=false;
        }

      TS_ASSERT(correct_pdf);
      }
  };

#endif // BLINN_DISTRIBUTION_TEST_H