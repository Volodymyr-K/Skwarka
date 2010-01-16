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
    void test_OrenNayar_Type()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(Spectrum_d(1.0),0.1) );

      TS_ASSERT(bxdf->GetType() == (BSDF_REFLECTION | BSDF_DIFFUSE));
      }

    void test_OrenNayar_Reciprocity()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(Spectrum_d(1.0),0.1) );

      Vector3D_d v1=Vector3D_d(0.2,0.5,0.5).Normalized();
      Vector3D_d v2=Vector3D_d(-0.1,0.3,0.1).Normalized();
      Spectrum_d val1 = bxdf->Evaluate(v1, v2);
      Spectrum_d val2 = bxdf->Evaluate(v2, v1);

      TS_ASSERT_EQUALS(val1,val2);
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

        if (exitant[2]<0.0 || pdf<0.0)
          correct=false;
        if (sp[0]<0.0 || sp[1]<0.0 || sp[2]<0.0)
          correct=false;
        }

      TS_ASSERT(correct);
      }

    // When sigma parameter is zero Oren-Nayar should give the same results as Lambertian.
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

    void test_OrenNayar_TotalScattering1()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(Spectrum_d(0.9),0.1) );

      size_t num_samples=100;
      std::vector<Point2D_d> samples(num_samples*num_samples);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),num_samples,num_samples,true);

      Spectrum_d total=bxdf->TotalScattering(Vector3D_d(0.5,0.5,0.5).Normalized(), SamplesSequence2D(&samples[0], (&samples[0]) + samples.size()));
      CustomAssertDelta(total, Spectrum_d(0.8974), 0.001); // This is an empirical value.
      }

    void test_OrenNayar_TotalScattering2()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(Spectrum_d(0.9),0.1) );

      size_t num_samples=400;
      std::vector<Point2D_d> samples(num_samples*num_samples);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),num_samples,num_samples,true);

      Spectrum_d total=bxdf->TotalScattering(true, SamplesSequence2D(&samples[0], (&samples[0]) + samples.size()));
      CustomAssertDelta(total, Spectrum_d(0.888), 0.002); // This is an empirical value.
      }
  };

#endif // OREN_NAYAR_TEST_H