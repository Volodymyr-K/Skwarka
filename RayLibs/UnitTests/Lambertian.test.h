#ifndef LAMBERTIAN_TEST_H
#define LAMBERTIAN_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <Raytracer/BxDFs/Lambertian.h>
#include <Math/SamplingRoutines.h>
#include <vector>

class LambertianTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Lambertian_Type()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new Lambertian(Spectrum_d(1.0)) );

      TS_ASSERT(bxdf->GetType() == (BSDF_REFLECTION | BSDF_DIFFUSE));
      }

    void test_Lambertian_Sample()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new Lambertian(Spectrum_d(1.0)) );
      size_t num_samples=1000;

      bool correct=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d exitant;
        Spectrum_d sp = bxdf->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);

        if (sp!=Spectrum_d(INV_PI))
          {
          TS_FAIL("Wrong Lambertian value");
          break;
          }

        if (exitant.IsNormalized() == false)
          {
          TS_FAIL("Sampled direction is not normalized");
          break;
          }
        }
      }

    void test_Lambertian_TotalScattering1()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new Lambertian(Spectrum_d(0.9)) );

      size_t num_samples=10;
      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);

      Spectrum_d total=bxdf->TotalScattering(Vector3D_d(0.5,0.5,0.5).Normalized(), SamplesSequence2D(&samples[0], (&samples[0]) + samples.size()));
      TS_ASSERT_EQUALS(total, Spectrum_d(0.9));
      }

    void test_Lambertian_TotalScattering2()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new Lambertian(Spectrum_d(0.9)) );

      size_t num_samples=10;
      std::vector<Point2D_d> samples1(num_samples), samples2(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples1.begin(),num_samples,true);
      SamplingRoutines::LatinHypercubeSampling2D(samples2.begin(),num_samples,true);

      SamplesSequence2D sequence1(&samples1[0], (&samples1[0]) + samples1.size());
      SamplesSequence2D sequence2(&samples2[0], (&samples2[0]) + samples2.size());

      Spectrum_d total=bxdf->TotalScattering(true, sequence1, sequence2);
      TS_ASSERT_EQUALS(total, Spectrum_d(0.9));
      }
  };

#endif // LAMBERTIAN_TEST_H