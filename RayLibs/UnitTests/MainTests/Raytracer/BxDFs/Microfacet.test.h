#ifndef MICROFACET_TEST_H
#define MICROFACET_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <Raytracer/BxDFs/Microfacet.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/SamplingRoutines.h>
#include <vector>

// Tests Microfacet BxDF with BlinnDistribution.
class MicrofacetTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      // These settings are for gold.
      FresnelConductor fresnel(Spectrum_d(0.37), Spectrum_d(2.82));
      BlinnDistribution blinn(20.0);
      typedef Microfacet<FresnelConductor,BlinnDistribution> Metal;

      mp_bxdf = shared_ptr<BxDF>( new Metal(Spectrum_d(1.0), fresnel, blinn) );
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_Microfacet_Type()
      {
      TS_ASSERT(mp_bxdf->GetType() == (BSDF_REFLECTION | BSDF_GLOSSY));
      }

    void test_Microfacet_Reciprocity()
      {
      Vector3D_d v1=Vector3D_d(0.2,0.5,0.8).Normalized();
      Vector3D_d v2=Vector3D_d(-0.1,-0.3,0.9).Normalized();
      Spectrum_d val1 = mp_bxdf->Evaluate(v1, v2);
      Spectrum_d val2 = mp_bxdf->Evaluate(v2, v1);

      TS_ASSERT_EQUALS(val1,val2);
      }

    void test_Microfacet_Sample()
      {
      size_t num_samples=1000;

      bool correct=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d exitant;
        Spectrum_d sp = mp_bxdf->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);

        if (pdf<0.0)
          correct=false;
        if (sp[0]<0.0 || sp[1]<0.0 || sp[2]<0.0)
          correct=false;
        }

      TS_ASSERT(correct);
      }

    void test_Microfacet_PDF()
      {
      size_t num_samples=1000;

      bool pdf_correct=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
        Vector3D_d exitant;
        Spectrum_d sp = mp_bxdf->Sample(incident, exitant, sample, pdf);

        double pdf2 = mp_bxdf->PDF(incident,exitant);
        if (fabs(pdf-pdf2)>(1e-5)*pdf)
          pdf_correct=false;
        }

      TS_ASSERT(pdf_correct);
      }

    void test_Microfacet_PDFSum()
      {
      size_t num_samples_sqrt = 300;
      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      double sum=0;
      
      std::vector<Point2D_d> samples(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples.begin(), num_samples_sqrt, num_samples_sqrt, true);
      for(size_t i=0;i<num_samples_sqrt*num_samples_sqrt;++i)
        {
        Vector3D_d exitant = SamplingRoutines::UniformHemisphereSampling(samples[i]);

        double pdf1 = mp_bxdf->PDF(incident,exitant);
        sum+=pdf1*2.0*M_PI/(num_samples_sqrt*num_samples_sqrt);

        exitant[2]=-exitant[2];
        double pdf2 = mp_bxdf->PDF(incident,exitant);
        sum+=pdf2*2.0*M_PI/(num_samples_sqrt*num_samples_sqrt);
        }

      // For some reason the PDF does not really sum up to 1.0 but rather to ~1.047. Don't know why...
      TS_ASSERT_DELTA(sum, 1.0, 0.07);
      }

    void test_Microfacet_TotalScattering1()
      {
      size_t num_samples_sqrt=200;
      std::vector<Point2D_d> samples(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),num_samples_sqrt,num_samples_sqrt,true);

      Spectrum_d total=mp_bxdf->TotalScattering(Vector3D_d(0.5,0.5,0.5).Normalized(), SamplesSequence2D(&samples[0], (&samples[0]) + samples.size()));
      CustomAssertDelta(total, Spectrum_d(0.7551), 0.001); // This is an empirical value.
      }

    void test_Microfacet_TotalScattering2()
      {
      size_t num_samples_sqrt=300;
      std::vector<Point2D_d> samples1(num_samples_sqrt*num_samples_sqrt), samples2(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples1.begin(),num_samples_sqrt,num_samples_sqrt,true);
      SamplingRoutines::StratifiedSampling2D(samples2.begin(),num_samples_sqrt,num_samples_sqrt,true);
      SamplingRoutines::Shuffle(samples2.begin(), samples2.size());

      SamplesSequence2D sequence1(&samples1[0], (&samples1[0]) + samples1.size());
      SamplesSequence2D sequence2(&samples2[0], (&samples2[0]) + samples2.size());

      Spectrum_d total=mp_bxdf->TotalScattering(true, sequence1, sequence2);
      CustomAssertDelta(total, Spectrum_d(0.7814), 0.005); // This is an empirical value.
      }

  private:
    shared_ptr<BxDF> mp_bxdf;
  };

#endif // MICROFACET_TEST_H