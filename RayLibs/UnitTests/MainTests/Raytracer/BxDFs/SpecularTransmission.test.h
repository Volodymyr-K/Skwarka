#ifndef SPECULAR_TRANSMISSION_TEST_H
#define SPECULAR_TRANSMISSION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <Raytracer/BxDFs/SpecularTransmission.h>
#include <Raytracer/BxDFs/SpecularReflection.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/SamplingRoutines.h>
#include <vector>

class SpecularTransmissionTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_SpecularTransmission_Type()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularTransmission(Spectrum_d(1.0), 1.5, 1.0) );

      TS_ASSERT(bxdf->GetType() == (BSDF_TRANSMISSION | BSDF_SPECULAR));
      }

    void test_SpecularTransmission_Sample()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularTransmission(Spectrum_d(1.0), 1.5, 1.0) );

      Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

      double pdf;
      Vector3D_d incident=Vector3D_d(0.5,0.5,0.1).Normalized();
      Vector3D_d exitant;
      Spectrum_d sp = bxdf->Sample(incident, exitant, sample, pdf);

      TS_ASSERT(exitant[2]<0.0);
      TS_ASSERT(exitant.IsNormalized());
      TS_ASSERT_DELTA( sqrt(1.0-incident[2]*incident[2])*1.0, sqrt(1.0-exitant[2]*exitant[2])*1.5, (1e-10));
      TS_ASSERT_EQUALS(pdf,1.0);
      }

    void test_SpecularTransmission_SampleTotalInternalReflection()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularTransmission(Spectrum_d(1.0), 1.5, 1.0) );

      Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

      double pdf;
      Vector3D_d incident=Vector3D_d(0.5,0.5,-0.1).Normalized();
      Vector3D_d exitant;
      Spectrum_d sp = bxdf->Sample(incident, exitant, sample, pdf);

      TS_ASSERT_EQUALS(sp, Spectrum_d(0.0));
      TS_ASSERT(exitant[2]<0.0);
      TS_ASSERT(exitant.IsNormalized());
      TS_ASSERT_EQUALS(exitant, Vector3D_d(-incident[0],-incident[1],incident[2]));
      TS_ASSERT_EQUALS(pdf,1.0);
      }

    // Tests that PDF returns 0.0 even for the transmitted direction.
    void test_SpecularTransmission_PDF()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularTransmission(Spectrum_d(1.0), 1.5, 1.0) );

      Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

      double pdf;
      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      Vector3D_d exitant;
      Spectrum_d sp = bxdf->Sample(incident, exitant, sample, pdf);

      double pdf2 = bxdf->PDF(incident,exitant);

      TS_ASSERT_EQUALS(pdf2,0.0);
      }

    void test_SpecularTransmission_TotalScattering1()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularTransmission(Spectrum_d(1.0), 1.5, 1.0) );

      size_t num_samples=10000;
      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);

      Spectrum_d total=bxdf->TotalScattering(Vector3D_d(0.5,0.5,0.5).Normalized(), SamplesSequence2D(&samples[0], (&samples[0]) + samples.size()));
      CustomAssertDelta(total, Spectrum_d(0.931067), (1e-6)); // This is an empirical value.
      }

    void test_SpecularTransmission_TotalScattering2()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularTransmission(Spectrum_d(1.0), 1.5, 1.0) );

      size_t num_samples=50000;
      std::vector<Point2D_d> samples1(num_samples), samples2(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples1.begin(),num_samples,true);
      SamplingRoutines::LatinHypercubeSampling2D(samples2.begin(),num_samples,true);

      SamplesSequence2D sequence1(&samples1[0], (&samples1[0]) + samples1.size());
      SamplesSequence2D sequence2(&samples2[0], (&samples2[0]) + samples2.size());

      Spectrum_d total=bxdf->TotalScattering(true, sequence1, sequence2);
      CustomAssertDelta(total, Spectrum_d(0.908222), (1e-6)); // This is an empirical value.
      }

    // Tests when a total internal reflection occurs.
    void test_SpecularTransmission_TotalScattering3()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularTransmission(Spectrum_d(1.0), 1.5, 1.0) );

      size_t num_samples=500;
      std::vector<Point2D_d> samples1(num_samples*num_samples), samples2(num_samples*num_samples);
      SamplingRoutines::StratifiedSampling2D(samples1.begin(),num_samples,num_samples,true);
      SamplingRoutines::StratifiedSampling2D(samples2.begin(),num_samples,num_samples,true);

      SamplesSequence2D sequence1(&samples1[0], (&samples1[0]) + samples1.size());
      SamplesSequence2D sequence2(&samples2[0], (&samples2[0]) + samples2.size());

      Spectrum_d total=bxdf->TotalScattering(false, sequence1, sequence2);
      CustomAssertDelta(total, Spectrum_d(0.403654), (1e-4)); // This is an empirical value.
      }

    // Tests that total scattering for reflectance and transmittance sums up to 1.0
    void test_SpecularTransmission_TotalScatteringWithReflection()
      {
      shared_ptr<BxDF> transmittance = shared_ptr<BxDF>( new SpecularTransmission(Spectrum_d(1.0), 1.5, 1.0) );

      typedef SpecularReflection<FresnelDielectric> SpecularDielectric;
      FresnelDielectric fresnel(1.5, 1.0);
      shared_ptr<BxDF> reflectance = shared_ptr<BxDF>( new SpecularDielectric(Spectrum_d(1.0), fresnel) );

      size_t num_samples_sqrt=500;
      std::vector<Point2D_d> samples1(num_samples_sqrt*num_samples_sqrt), samples2(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples1.begin(),num_samples_sqrt,num_samples_sqrt,true);
      SamplingRoutines::StratifiedSampling2D(samples2.begin(),num_samples_sqrt,num_samples_sqrt,true);

      SamplesSequence2D sequence1(&samples1[0], (&samples1[0]) + samples1.size());
      SamplesSequence2D sequence2(&samples2[0], (&samples2[0]) + samples2.size());

      Spectrum_d total_1=transmittance->TotalScattering(false, sequence1, sequence2);
      Spectrum_d total_2=reflectance->TotalScattering(false, sequence2, sequence1);
      CustomAssertDelta(total_1+total_2, Spectrum_d(1.0), (1e-4)); // This is an empirical value.
      }

  };

#endif // SPECULAR_TRANSMISSION_TEST_H