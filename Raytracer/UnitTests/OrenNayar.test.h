#ifndef OREN_NAYAR_TEST_H
#define OREN_NAYAR_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <Raytracer/BxDFs/OrenNayar.h>
#include <Raytracer/BxDFs/Lambertian.h>
#include <Math/SamplingRoutines.h>
#include <vector>

class OrenNayarTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_OrenNayar_Constr()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(Spectrum_d(1.0),0.1) );

      TS_ASSERT(bxdf->GetType() == (BSDF_REFLECTION | BSDF_DIFFUSE));
      }

    void test_OrenNayar_Sample()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(Spectrum_d(1.0),0.1) );
      size_t num_samples=1000;

      bool correct=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d exitant;
        Spectrum_d sp = bxdf->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);

        if (sp[0]<0||sp[0]>1.0||sp[1]<0||sp[1]>1.0||sp[2]<0||sp[2]>1.0)
          correct=false;
        }

      TS_ASSERT(correct);
      }

    // When sigma parameter is zero Oren Nayar should give the same results as Lambertian.
    void test_OrenNayar_ZeroVariance()
      {
      shared_ptr<BxDF> oren_nayar = shared_ptr<BxDF>( new OrenNayar(Spectrum_d(1.0),0.0) );
      shared_ptr<BxDF> lambertian = shared_ptr<BxDF>( new Lambertian(Spectrum_d(1.0)) );
      size_t num_samples=1000;

      bool equal=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d exitant;
        Spectrum_d sp1 = oren_nayar->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);
        Spectrum_d sp2 = lambertian->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);

        if (sp1!=sp2)
          equal=false;
        }

      TS_ASSERT(equal);
      }

    // Just tests the range, could not came up with a more intelligent test...
    void test_OrenNayar_TotalScattering1()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(Spectrum_d(0.9),0.1) );
      size_t num_samples=10000;
      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);

      Spectrum_d total=bxdf->TotalScattering(Vector3D_d(0.5,0.5,0.5).Normalized(), SamplesSequence2D(samples.begin(), samples.end()));
      TS_ASSERT(total[0]>=0 && total[0]<=0.9);
      TS_ASSERT(total[1]>=0 && total[1]<=0.9);
      TS_ASSERT(total[2]>=0 && total[2]<=0.9);
      }

    // Just tests the range, could not came up with a more intelligent test...
    void test_OrenNayar_TotalScattering2()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(Spectrum_d(0.9),0.1) );

      size_t num_samples=20000;
      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);

      Spectrum_d total=bxdf->TotalScattering(SamplesSequence2D(samples.begin(), samples.end()));
      TS_ASSERT(total[0]>=0 && total[0]<=0.93);
      TS_ASSERT(total[1]>=0 && total[1]<=0.93);
      TS_ASSERT(total[2]>=0 && total[2]<=0.93);
      }
  };

#endif // OREN_NAYAR_TEST_H