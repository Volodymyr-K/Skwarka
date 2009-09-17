#ifndef BXDF_TEST_H
#define BXDF_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/BxDF.h>
#include "Mocks/BxDFMock.h"
#include <Math/ThreadSafeRandom.h>
#include <Math/SamplingRoutines.h>
#include <vector>

class BxDFTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_BxDF_Constr()
      {
      BxDFMock bxdf(BxDFType(BSDF_TRANSMISSION | BSDF_GLOSSY));

      TS_ASSERT(bxdf.GetType() == (BSDF_TRANSMISSION | BSDF_GLOSSY));
      }

    void test_BxDF_MathcesFlags()
      {
      BxDFMock bxdf(BxDFType(BSDF_TRANSMISSION | BSDF_GLOSSY));

      TS_ASSERT(bxdf.MatchesFlags(BSDF_ALL));
      TS_ASSERT(bxdf.MatchesFlags(BSDF_ALL_TRANSMISSION));
      TS_ASSERT(bxdf.MatchesFlags(BSDF_ALL_REFLECTION)==false);
      TS_ASSERT(bxdf.MatchesFlags(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR))==false);
      }

    void test_BxDF_Sample()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new BxDFMock(BxDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE)) );
      size_t num_samples=1000;

      bool correct_hemisphere=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d exitant;
        Spectrum_d sp = bxdf->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);
      
        if (exitant[2]>0.0)
          correct_hemisphere=false;
        }

      TS_ASSERT(correct_hemisphere);
      }

    void test_BxDF_PDF()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new BxDFMock(BxDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE)) );
      size_t num_samples=1000;

      bool correct_pdf=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d exitant;
        Spectrum_d sp = bxdf->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);

        if (pdf<=0.0 || pdf!=bxdf->PDF(Vector3D_d(0.5,0.5,0.5).Normalized(),exitant))
          correct_pdf=false;
        }

      TS_ASSERT(correct_pdf);
      }

    void test_BxDF_TotalScattering1()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new BxDFMock(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)) );

      size_t num_samples=5000;
      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);

      Spectrum_d total=bxdf->TotalScattering(Vector3D_d(0.5,0.5,0.5).Normalized(), SamplesSequence2D(samples.begin(), samples.end()));

      TS_ASSERT(total[0]<=1.0 && total[1]<=1.0 && total[2]<=1.0);
      TS_ASSERT_DELTA(total[0], 1.0, 0.03);
      TS_ASSERT_DELTA(total[1], 1.0, 0.03);
      TS_ASSERT_DELTA(total[2], 1.0, 0.03);
      }

    void test_BxDF_TotalScattering2()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new BxDFMock(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)) );

      size_t num_samples=10000;
      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);

      Spectrum_d total=bxdf->TotalScattering(SamplesSequence2D(samples.begin(), samples.end()));

      TS_ASSERT(total[0]<=1.0 && total[1]<=1.0 && total[2]<=1.0);
      TS_ASSERT_DELTA(total[0], 1.0, 0.03);
      TS_ASSERT_DELTA(total[1], 1.0, 0.03);
      TS_ASSERT_DELTA(total[2], 1.0, 0.03);
      }
  };

#endif // BXDF_TEST_H