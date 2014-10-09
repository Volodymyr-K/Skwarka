#ifndef SCALED_BXDF_TEST_H
#define SCALED_BXDF_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Math/SamplingRoutines.h>
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/BxDFs/ScaledBxDF.h>
#include "Mocks/BxDFMock.h"
#include <vector>

class ScaledBxDFTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_ScaledBxDF_Type()
      {
      SpectrumCoef_d scale(0.7);
      shared_ptr<BxDF> p_reference_bxdf = shared_ptr<BxDF>( new BxDFMock(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY), SpectrumCoef_d(0.8)) );
      shared_ptr<BxDF> p_scaled_bxdf = shared_ptr<BxDF>( new ScaledBxDF(p_reference_bxdf.get(), scale) );

      TS_ASSERT(p_reference_bxdf->GetType() == p_scaled_bxdf->GetType());
      }

    void test_ScaledBxDF_Evaluate()
      {
      SpectrumCoef_d scale(0.7);
      shared_ptr<BxDF> p_reference_bxdf = shared_ptr<BxDF>( new BxDFMock(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY), SpectrumCoef_d(0.8)) );
      shared_ptr<BxDF> p_scaled_bxdf = shared_ptr<BxDF>( new ScaledBxDF(p_reference_bxdf.get(), scale) );

      Vector3D_d incident = Vector3D_d(1,1,1).Normalized();
      Vector3D_d exitant = Vector3D_d(-1,1.1,0.2).Normalized();
      TS_ASSERT_EQUALS(p_reference_bxdf->Evaluate(incident, exitant)*scale, p_scaled_bxdf->Evaluate(incident, exitant));
      }

    void test_ScaledBxDF_Sample()
      {
      SpectrumCoef_d scale(0.7);
      shared_ptr<BxDF> p_reference_bxdf = shared_ptr<BxDF>( new BxDFMock(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY), SpectrumCoef_d(0.8)) );
      shared_ptr<BxDF> p_scaled_bxdf = shared_ptr<BxDF>( new ScaledBxDF(p_reference_bxdf.get(), scale) );
      
      Vector3D_d incident = Vector3D_d(1,1,1).Normalized();
      Point2D_d sample(0.1,0.7);
      double pdf1,pdf2;
      Vector3D_d exitant1, exitant2;

      SpectrumCoef_d s1 = p_reference_bxdf->Sample(incident, exitant1, sample, pdf1);
      SpectrumCoef_d s2 = p_scaled_bxdf->Sample(incident, exitant2, sample, pdf2);

      TS_ASSERT_EQUALS(s1*scale, s2);
      TS_ASSERT_EQUALS(exitant1, exitant2);
      TS_ASSERT_EQUALS(pdf1, pdf2);
      }

    void test_ScaledBxDF_PDF()
      {
      SpectrumCoef_d scale(0.7);
      shared_ptr<BxDF> p_reference_bxdf = shared_ptr<BxDF>( new BxDFMock(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY), SpectrumCoef_d(0.8)) );
      shared_ptr<BxDF> p_scaled_bxdf = shared_ptr<BxDF>( new ScaledBxDF(p_reference_bxdf.get(), scale) );

      Vector3D_d incident = Vector3D_d(1,1,1).Normalized();
      Vector3D_d exitant = Vector3D_d(-1,1.1,0.2).Normalized();
      TS_ASSERT_EQUALS(p_reference_bxdf->PDF(incident, exitant), p_scaled_bxdf->PDF(incident, exitant));
      }

    void test_ScaledBxDF_TotalScattering1()
      {
      SpectrumCoef_d scale(0.7);
      shared_ptr<BxDF> p_reference_bxdf = shared_ptr<BxDF>( new BxDFMock(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY), SpectrumCoef_d(0.8)) );
      shared_ptr<BxDF> p_scaled_bxdf = shared_ptr<BxDF>( new ScaledBxDF(p_reference_bxdf.get(), scale) );

      size_t num_samples_sqrt=100;
      std::vector<Point2D_d> samples(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),num_samples_sqrt,num_samples_sqrt,true);

      Vector3D_d incident = Vector3D_d(0.5,0.5,0.5).Normalized();
      SpectrumCoef_d total1 = p_reference_bxdf->TotalScattering(incident, SamplesSequence2D(&samples[0], (&samples[0]) + samples.size()));
      SpectrumCoef_d total2 = p_scaled_bxdf->TotalScattering(incident, SamplesSequence2D(&samples[0], (&samples[0]) + samples.size()));
      TS_ASSERT_EQUALS(total1*scale, total2);
      }

    void test_ScaledBxDF_TotalScattering2()
      {
      SpectrumCoef_d scale(0.7);
      shared_ptr<BxDF> p_reference_bxdf = shared_ptr<BxDF>( new BxDFMock(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY), SpectrumCoef_d(0.8)) );
      shared_ptr<BxDF> p_scaled_bxdf = shared_ptr<BxDF>( new ScaledBxDF(p_reference_bxdf.get(), scale) );

      size_t num_samples_sqrt=100;
      std::vector<Point2D_d> samples1(num_samples_sqrt*num_samples_sqrt), samples2(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples1.begin(),num_samples_sqrt,num_samples_sqrt,true);
      SamplingRoutines::StratifiedSampling2D(samples2.begin(),num_samples_sqrt,num_samples_sqrt,true);
      SamplingRoutines::Shuffle(samples2.begin(), samples2.size());

      SamplesSequence2D sequence1(&samples1[0], (&samples1[0]) + samples1.size());
      SamplesSequence2D sequence2(&samples2[0], (&samples2[0]) + samples2.size());

      SpectrumCoef_d total1=p_reference_bxdf->TotalScattering(true, sequence1, sequence2);
      SpectrumCoef_d total2=p_scaled_bxdf->TotalScattering(true, sequence1, sequence2);

      TS_ASSERT_EQUALS(total1*scale, total2);
      }
  };

#endif // SCALED_BXDF_TEST_H
